#include "Function.h"
#include "../../Units/Units.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Function/Function.h"
#include "../../CoreForms/CoreForms.h"
#include "../Linkage/Linkage.h"
#include "../Type/Type.h"
#include "../Function/Function.h"
#include "../ProcBody/ProcBody.h"
#include "../Argument/Argument.h"
#include "../../llvm_Function.h"
#include "Config.h"

using namespace dale::ErrorInst;

namespace dale
{
bool
parseFunctionAttributes(Context *ctx, std::vector<Node *> *attr_list,
                        bool *always_inline, bool *cto)
{
    for (std::vector<Node*>::iterator b = (attr_list->begin() + 1),
                                      e = attr_list->end();
            b != e;
            ++b) {
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

bool
parseArguments(Units *units, Node *args_node,
               std::vector<Variable *> *fn_args_internal)
{
    Context *ctx = units->top()->ctx;
    std::vector<Node *> *args = args_node->list;

    for (std::vector<Node *>::iterator b = args->begin(),
                                       e = args->end();
            b != e;
            ++b) {
        Variable *var = new Variable();
        var->type = NULL;

        FormArgumentParse(units, var, (*b), false, false, true);
        if (var->type == NULL) {
            delete var;
            return false;
        }

        if (var->type->is_array) {
            delete var;
            Error *e = new Error(ArraysCannotBeFunctionParameters,
                                 (*b));
            ctx->er->addError(e);
            return false;
        }

        if (var->type->base_type == BaseType::Void) {
            delete var;
            if (args->size() != 1) {
                Error *e = new Error(VoidMustBeTheOnlyParameter,
                                     args_node);
                ctx->er->addError(e);
                return false;
            }
            break;
        }

        if (var->type->base_type == BaseType::VarArgs) {
            if ((args->end() - b) != 1) {
                delete var;
                Error *e = new Error(VarArgsMustBeLastParameter,
                                     args_node);
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

bool 
FormFunctionParse(Units *units, Node *node, const char *name,
                  Function **new_fn, int override_linkage,
                  bool is_anonymous)
{
    Context *ctx = units->top()->ctx;

    if (!name) {
        Node *name_node = (*(node->list))[1];
        name = name_node->token->str_value.c_str();
        node = (*(node->list))[2];
    }

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
    if (test->is_list
            && test->list->at(0)->is_token
            && !(test->list->at(0)->token->str_value.compare("attr"))) {
        bool res = parseFunctionAttributes(ctx, test->list,
                                           &always_inline, &cto);
        if (!res) {
            return false;
        }
        ++next_index;
    }

    int linkage =
        (override_linkage)
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
        Error *e = new Error(UnexpectedElement, args_node,
                             "list", "parameters", "symbol");
        ctx->er->addError(e);
        return false;
    }

    std::vector<Variable *> fn_args_internal;
    bool res = parseArguments(units, args_node, &fn_args_internal);
    if (!res) {
        return false;
    }
    bool varargs = false;
    if (fn_args_internal.size()
            && (fn_args_internal.back()->type->base_type == BaseType::VarArgs)) {
        varargs = true;
    }

    std::vector<llvm::Type*> fn_args;
    for (std::vector<Variable *>::iterator b = fn_args_internal.begin(),
                                           e = fn_args_internal.end();
            b != e;
            ++b) {
        Type *type = (*b)->type;
        if (type->is_reference) {
            type = ctx->tr->getPointerType(type);
        }
        if (type->base_type == BaseType::VarArgs) {
            break;
        }
        llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
        if (!llvm_type) {
            return false;
        }
        fn_args.push_back(llvm_type);
    }

    /* For return type parsing, activate an anonymous namespace and
     * add all of the function parameters to it.  This is so that if
     * the return type uses a macro that depends on one of those
     * parameters, it will work properly. */

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    for (std::vector<Variable *>::iterator b = fn_args_internal.begin(),
                                           e = fn_args_internal.end();
            b != e;
            ++b) {
        ctx->ns()->addVariable((*b)->name.c_str(), (*b));
    }

    Type *ret_type = FormTypeParse(units, (*lst)[return_type_index],
                                   false, false, false, true);

    ctx->deactivateNamespace(anon_name.c_str());

    if (ret_type == NULL) {
        return false;
    }
    if (ret_type->is_array) {
        Error *e = new Error(ReturnTypesCannotBeArrays, (*lst)[next_index]);
        ctx->er->addError(e);
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
        fn_args.push_back(ctx->toLLVMType(ctx->tr->getPointerType(ret_type),
                                          NULL, true));
        llvm_ret_type = ctx->toLLVMType(ctx->tr->getBasicType(BaseType::Void),
                                        NULL, true);
    }
    llvm::FunctionType *ft = getFunctionType(llvm_ret_type, fn_args,
                                             varargs);

    std::string symbol;
    ctx->ns()->functionNameToSymbol(name, &symbol, linkage,
                                    &fn_args_internal);

    Function *fn = new Function(ret_type, &fn_args_internal, NULL, 0,
                                 &symbol, always_inline);
    fn->linkage = linkage;
    fn->cto = cto;
    if (!strcmp(name, "setf-copy") || !strcmp(name, "setf-assign")) {
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

    llvm::Function *existing_llvm_fn =
        units->top()->module->getFunction(symbol.c_str());
    if (existing_llvm_fn) {
        Function *existing_fn = ctx->getFunction(symbol.c_str(), NULL,
                                                 NULL, 0);
        if (existing_fn && !existing_fn->isEqualTo(fn)) {
            Error *e = new Error(RedeclarationOfFunctionOrMacro,
                                 node, name);
            ctx->er->addError(e);
            return false;
        }
        if (existing_fn && !existing_fn->attrsAreEqual(fn)) {
            Error *e = new Error(AttributesOfDeclAndDefAreDifferent,
                                 node, name);
            ctx->er->addError(e);
            return false;
        }
    }

    if (units->top()->once_tag.length() > 0) {
        fn->once_tag = units->top()->once_tag;
    }

    llvm::Constant *fnc =
        units->top()->module->getOrInsertFunction(symbol.c_str(), ft);

    llvm::Function *llvm_fn = llvm::dyn_cast<llvm::Function>(fnc);

    /* If llvm_fn is null, then the function already exists and the extant
     * function has a different prototype, so it's an invalid
     * redeclaration.  If llvm_fn is not null, but has content, then it's an
     * invalid redefinition. */

    if (!llvm_fn || llvm_fn->size()) {
        Error *e = new Error(RedeclarationOfFunctionOrMacro, node, name);
        ctx->er->addError(e);
        return false;
    }

    if (always_inline) {
#if D_LLVM_VERSION_MINOR == 2
        llvm_fn->addFnAttr(llvm::Attributes::AlwaysInline);
#else
        llvm_fn->addFnAttr(llvm::Attribute::AlwaysInline);
#endif
    }

    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(
        (lst->size() == (unsigned) (next_index + 2))
            ? ctx->toLLVMLinkage(override_linkage)
            : ctx->toLLVMLinkage(linkage)
    );

    llvm::Function::arg_iterator llvm_arg_iter = llvm_fn->arg_begin();
    for (std::vector<Variable *>::iterator b = fn_args_internal.begin(),
                                           e = fn_args_internal.end();
            b != e;
            ++b) {
        if ((*b)->type->base_type == BaseType::VarArgs) {
            break;
        }
        llvm::Value *llvm_arg = llvm_arg_iter;
        ++llvm_arg_iter;
        llvm_arg->setName((*b)->name.c_str());
        (*b)->value = llvm_arg;
    }

    llvm::Value *llvm_return_value = NULL;
    if (ret_type->is_retval) {
        llvm_return_value = llvm_arg_iter;
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

    if (lst->size() == (unsigned) (next_index + 2)) {
        return true;
    }

    ctx->activateAnonymousNamespace();
    anon_name = ctx->ns()->name;

    units->top()->pushGlobalFunction(fn);
    FormProcBodyParse(units, node, fn, llvm_fn, (next_index + 2),
                      is_anonymous, llvm_return_value);
    units->top()->popGlobalFunction();

    /* Previously, the init-channels function was called at this
     * point, if it was present and the current function's name was
     * 'main'.  That function initialised stdin, stdout, and stderr by
     * calling fdopen.  However, that isn't safe: for example,
     * interleaved calls to printf and puts would yield
     * incorrectly-ordered output when fully buffered.  Consequently,
     * init-channels has been removed, and the compiler depends on the
     * handles being present as variables and preinitialised.  This is
     * likely not true for all platforms, given that it isn't required
     * by the C standard. */

    ctx->deactivateNamespace(anon_name.c_str());

    return true;
}
}
