#include "Function.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../CoreForms/CoreForms.h"
#include "../../Function/Function.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Units/Units.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../../llvm_Function.h"
#include "../Function/Function.h"
#include "../Linkage/Linkage.h"
#include "../Parameter/Parameter.h"
#include "../ProcBody/ProcBody.h"
#include "../Type/Type.h"
#include "../Utils/Utils.h"
#include "Config.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace dale::ErrorInst;

namespace dale {
bool parseFunctionAttributes(Context *ctx,
                             std::vector<Node *> *attr_list,
                             bool *always_inline, bool *cto) {
    for (std::vector<Node *>::iterator b = (attr_list->begin() + 1),
                                       e = attr_list->end();
         b != e; ++b) {
        if ((*b)->is_list) {
            Error *e = new Error(InvalidAttribute, (*b));
            ctx->er->addError(e);
            return false;
        }
        if (!((*b)->token->str_value.compare("inline"))) {
            *always_inline = true;
        } else if (!((*b)->token->str_value.compare("cto"))) {
            *cto = true;
        } else {
            Error *e = new Error(InvalidAttribute, (*b));
            ctx->er->addError(e);
            return false;
        }
    }
    return true;
}

bool parseParameters(Units *units, Node *args_node,
                     std::vector<Variable *> *fn_args_internal) {
    Context *ctx = units->top()->ctx;
    std::vector<Node *> *args = args_node->list;

    if (args->size() == 0) {
        Error *e = new Error(NoEmptyLists, args_node);
        ctx->er->addError(e);
        return false;
    }

    for (std::vector<Node *>::iterator b = args->begin(),
                                       e = args->end();
         b != e; ++b) {
        Variable *var = new Variable();
        var->type = NULL;

        FormParameterParse(units, var, (*b), false, false, true, false);
        if (var->type == NULL) {
            delete var;
            return false;
        }

        if (!var->type->is_reference && var->type->is_array) {
            delete var;
            Error *e =
                new Error(ArraysCannotBeFunctionParameters, (*b));
            ctx->er->addError(e);
            return false;
        }

        if (var->type->base_type == BaseType::Void) {
            delete var;
            if (args->size() != 1) {
                Error *e =
                    new Error(VoidMustBeTheOnlyParameter, args_node);
                ctx->er->addError(e);
                return false;
            }
            break;
        }

        if (var->type->base_type == BaseType::VarArgs) {
            if ((args->end() - b) != 1) {
                delete var;
                Error *e =
                    new Error(VarArgsMustBeLastParameter, args_node);
                ctx->er->addError(e);
                return false;
            }
            fn_args_internal->push_back(var);
            break;
        }

        if (var->type->is_function) {
            delete var;
            Error *e = new Error(NonPointerFunctionParameter, (*b));
            ctx->er->addError(e);
            return false;
        }

        fn_args_internal->push_back(var);
    }

    return true;
}

void addInitChannelsCall(Context *ctx, llvm::Function *llvm_fn) {
    std::vector<llvm::Value *> call_args;
    std::vector<Type *> params;
    Function *ic = ctx->getFunction("init-channels", &params, NULL, 0);
    assert(ic && ic->llvm_function &&
           "cannot find init-channels function");

    llvm::Function::iterator i = llvm_fn->begin();
    llvm::BasicBlock *b = &*i;

    if (b->empty()) {
        llvm::CallInst::Create(ic->llvm_function,
                               llvm::ArrayRef<llvm::Value *>(call_args),
                               "", b);
    } else {
        llvm::Instruction *fnp = b->getFirstNonPHI();
        if (fnp) {
            llvm::CallInst::Create(
                ic->llvm_function,
                llvm::ArrayRef<llvm::Value *>(call_args), "", fnp);
        } else {
            llvm::CallInst::Create(
                ic->llvm_function,
                llvm::ArrayRef<llvm::Value *>(call_args), "", b);
        }
    }
}

void parametersToTypes(std::vector<Variable *> *parameters,
                       std::vector<Type *> *types) {
    for (std::vector<Variable *>::iterator b = parameters->begin(),
                                           e = parameters->end();
         b != e; ++b) {
        Type *type = (*b)->type;
        types->push_back(type);
    }
}

bool parametersToLLVMTypes(Context *ctx,
                           std::vector<Variable *> *parameters,
                           std::vector<llvm::Type *> *types) {
    for (std::vector<Variable *>::iterator b = parameters->begin(),
                                           e = parameters->end();
         b != e; ++b) {
        Type *type = (*b)->type;
        if (type->is_reference || type->is_rvalue_reference) {
            type = ctx->tr->getPointerType(type);
        }
        if (type->base_type == BaseType::VarArgs) {
            break;
        }
        llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
        if (!llvm_type) {
            return false;
        }
        types->push_back(llvm_type);
    }

    return true;
}

Type *parseReturnType(Units *units, Context *ctx,
                      std::vector<Variable *> *parameters, Node *node) {
    /* For return type parsing, activate an anonymous namespace and
     * add all of the function parameters to it.  This is so that if
     * the return type uses a macro that depends on one of those
     * parameters, it will work properly. */

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    for (std::vector<Variable *>::iterator b = parameters->begin(),
                                           e = parameters->end();
         b != e; ++b) {
        ctx->addVariable((*b)->name.c_str(), (*b));
    }

    Type *ret_type =
        FormTypeParse(units, node, false, false, false, true);

    ctx->deactivateNamespace(anon_name.c_str());

    if (!ret_type) {
        return NULL;
    }

    if (ret_type->is_array) {
        Error *e = new Error(ReturnTypesCannotBeArrays, node);
        ctx->er->addError(e);
        return NULL;
    }

    return ret_type;
}

bool isValidDeclaration(Context *ctx, Node *node, const char *name,
                        int linkage, Function *fn) {
    Namespace *current_ns = (*(ctx->used_ns_nodes.rbegin()))->ns;
    std::vector<Type *> types;
    parametersToTypes(&fn->parameters, &types);
    Function *closest_fn = NULL;
    std::vector<bool> lvalues;
    for (std::vector<Type *>::iterator b = types.begin(),
                                       e = types.end();
         b != e; ++b) {
        lvalues.push_back((*b)->is_reference);
    }
    Function *matching_fn =
        (linkage == Linkage::Extern_C)
            ? current_ns->getFunction(name, NULL, NULL, false)
            : current_ns->getFunction(name, &types, &closest_fn, false,
                                      true, &lvalues);
    if (matching_fn && !matching_fn->is_macro &&
        !matching_fn->isVarArgs()) {
        if (!matching_fn->isEqualTo(fn)) {
            Error *e =
                new Error(RedeclarationOfFunctionOrMacro, node, name);
            ctx->er->addError(e);
            return false;
        }
        if (matching_fn->llvm_function &&
            matching_fn->llvm_function->size()) {
            Error *e =
                new Error(RedeclarationOfFunctionOrMacro, node, name);
            ctx->er->addError(e);
            return false;
        }
        if (!matching_fn->attrsAreEqual(fn)) {
            Error *e = new Error(AttributesOfDeclAndDefAreDifferent,
                                 node, name);
            ctx->er->addError(e);
            return false;
        }
    }

    Variable *matching_var = current_ns->getVariable(name);
    if (matching_var) {
        Error *e = new Error(RedeclarationOfDifferentKind, node, name);
        ctx->er->addError(e);
        return false;
    }

    return true;
}

bool FormFunctionParse(Units *units, Node *node, const char *name,
                       Function **new_fn, int override_linkage,
                       bool is_anonymous) {
    Context *ctx = units->top()->ctx;

    if (!is_anonymous) {
        units->prefunction_ns = ctx->ns();
    }

    if (CoreForms::existsNoOverride(name)) {
        Error *e = new Error(ThisCoreFormCannotBeOverridden, node);
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *> *lst = node->list;

    if (lst->size() < 4) {
        Error *e = new Error(IncorrectMinimumNumberOfArgs, node, "fn",
                             "3", (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    }

    int next_index = 1;
    bool always_inline = false;
    bool cto = units->cto;

    /* Whole modules, as well as specific functions, can be declared
     * compile-time-only.  If global CTO is enabled, that overrides
     * the absence of a CTO attribute here.  However, if a function
     * explicitly declares that it is CTO, that will override the
     * global setting. */

    Node *test = ((*lst)[next_index]);
    if (test->is_list && (*test->list)[0]->is_token &&
        !((*test->list)[0]->token->str_value.compare("attr"))) {
        bool res = parseFunctionAttributes(ctx, test->list,
                                           &always_inline, &cto);
        if (!res) {
            return false;
        }
        ++next_index;
    }

    int linkage = (override_linkage)
                      ? override_linkage
                      : FormLinkageParse(ctx, (*lst)[next_index]);
    if (!linkage) {
        return false;
    }
    if (!override_linkage) {
        ++next_index;
    }

    /* The return type is not parsed yet, because it may depend on the
     * types of the function parameters. */

    int return_type_index = next_index;

    Node *args_node = (*lst)[next_index + 1];
    if (!args_node->is_list) {
        Error *e = new Error(UnexpectedElement, args_node, "list",
                             "parameters", "symbol");
        ctx->er->addError(e);
        return false;
    }

    std::vector<Variable *> fn_args_internal;
    bool res = parseParameters(units, args_node, &fn_args_internal);
    if (!res) {
        return false;
    }

    if (linkage == Linkage::Extern_C) {
        for (std::vector<Variable *>::iterator
                 b = fn_args_internal.begin(),
                 e = fn_args_internal.end();
             b != e; ++b) {
            Type *type = (*b)->type;
            if (type->is_reference || type->is_rvalue_reference) {
                Error *e = new Error(NoRefsInExternC, args_node);
                ctx->er->addError(e);
                return false;
            }
        }
    }

    bool varargs = false;
    if (fn_args_internal.size() &&
        (fn_args_internal.back()->type->base_type ==
         BaseType::VarArgs)) {
        varargs = true;
    }

    std::vector<llvm::Type *> fn_args;
    res = parametersToLLVMTypes(ctx, &fn_args_internal, &fn_args);
    if (!res) {
        return false;
    }

    Type *ret_type = parseReturnType(units, ctx, &fn_args_internal,
                                     (*lst)[return_type_index]);
    if (ret_type == NULL) {
        return false;
    }

    llvm::Type *llvm_ret_type = ctx->toLLVMType(ret_type, NULL, true);
    if (!llvm_ret_type) {
        return false;
    }

    /* Create the LLVM function type.  If the retval attribute is
     * present, then the LLVM function type will have a return type of
     * void, and a pointer to a value of the return type will be the
     * final parameter. */

    if (ret_type->is_retval) {
        fn_args.push_back(ctx->toLLVMType(
            ctx->tr->getPointerType(ret_type), NULL, true));
        llvm_ret_type = ctx->toLLVMType(
            ctx->tr->getBasicType(BaseType::Void), NULL, true);
    }
    llvm::FunctionType *ft =
        getFunctionType(llvm_ret_type, fn_args, varargs);

    std::string symbol;
    ctx->ns()->functionNameToSymbol(name, &symbol, linkage,
                                    &fn_args_internal);

    Function *fn = new Function(ret_type, &fn_args_internal, NULL, 0,
                                &symbol, always_inline);
    fn->linkage = linkage;
    fn->cto = cto;
    if (units->top()->once_tag.length() > 0) {
        fn->once_tag = units->top()->once_tag;
    }
    if (!strcmp(name, "setf-copy-init") ||
        !strcmp(name, "setf-copy-assign") ||
        !strcmp(name, "setf-move-init") ||
        !strcmp(name, "setf-move-assign")) {
        fn->is_setf_fn = true;
    } else if (!strcmp(name, "destroy")) {
        fn->is_destructor = true;
    }

    if (fn->is_setf_fn) {
        if (!ret_type->isEqualTo(ctx->tr->type_bool)) {
            Error *e = new Error(SetfOverridesMustReturnBool,
                                 (*lst)[return_type_index]);
            ctx->er->addError(e);
            return false;
        }
    }

    res = isValidDeclaration(ctx, node, name, linkage, fn);
    if (!res) {
        return false;
    }

    int fn_linkage =
        (lst->size() == (unsigned)(next_index + 2))
            ? ctx->toLLVMLinkage(override_linkage)
            : ctx->toLLVMLinkage(linkage);

#if D_LLVM_VERSION_ORD <= 80
    llvm::Constant *fnc =
        units->top()->module->getOrInsertFunction(symbol.c_str(), ft);
    llvm::Function *llvm_fn = llvm::dyn_cast<llvm::Function>(fnc);
#else
    llvm::FunctionCallee llvm_fn_callee =
        units->top()->module->getOrInsertFunction(symbol.c_str(), ft);
    llvm::Function *llvm_fn =
        llvm::cast<llvm::Function>(llvm_fn_callee.getCallee());
    if (!llvm_fn) {
        llvm::Function *llvm_fn = llvm::Function::Create(
            ft, ctx->toLLVMLinkage(fn_linkage), symbol.c_str(),
            units->top()->module);
    }
#endif

    /* If llvm_fn is null, then the function already exists and the
     * extant function has a different prototype, so it's an invalid
     * redeclaration.  If llvm_fn is not null, but has content, then
     * it's an invalid redefinition.  These error conditions should be
     * caught by isValidDeclaration, but there's no harm in leaving
     * this check here as a stopgap. */
    if (!llvm_fn || llvm_fn->size()) {
        fprintf(stderr, "%s\n", symbol.c_str());
        assert(false && "uncaught invalid redeclaration");
        abort();
    }

    if (always_inline) {
        addInlineAttribute(llvm_fn);
    }

    llvm_fn->setCallingConv(llvm::CallingConv::C);
    linkVariablesToFunction(&fn_args_internal, llvm_fn);

    llvm::Value *llvm_return_value = NULL;
    if (ret_type->is_retval) {
        llvm::Function::arg_iterator llvm_arg_iter =
            llvm_fn->arg_begin();
        std::advance(llvm_arg_iter, fn->parameters.size());
        llvm_return_value = &*llvm_arg_iter;
        llvm_return_value->setName("retval");
    }

    fn->llvm_function = llvm_fn;
    if (!ctx->ns()->addFunction(name, fn, node)) {
        return false;
    }

    if (new_fn) {
        *new_fn = fn;
    }

    /* If the list has only four arguments, function is a
     * declaration and you can return straightaway. */

    if (lst->size() == (unsigned)(next_index + 2)) {
        return true;
    }

    units->top()->pushGlobalFunction(fn);
    FormProcBodyParse(units, node, fn, llvm_fn, (next_index + 2),
                      is_anonymous, llvm_return_value);
    units->top()->popGlobalFunction();

    if (!strcmp(name, "main") && (!strcmp(SYSTEM_NAME, "Darwin") ||
                                  !strcmp(SYSTEM_NAME, "FreeBSD")) &&
        ctx->getVariable("stdin") && ctx->getVariable("stdout") &&
        ctx->getVariable("stderr")) {
        addInitChannelsCall(ctx, llvm_fn);
    }

    if (units->debug) {
        moduleDebugPass(units->top()->module);
    }

    return true;
}
}
