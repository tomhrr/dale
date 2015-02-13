#include "Function.h"
#include "../../Generator/Generator.h"
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

namespace dale
{
bool 
FormFunctionParse(Generator *gen,
      Node *n,
      const char *name,
      Function **new_function,
      int override_linkage,
      int is_anonymous)
{
    Context *ctx = gen->units->top()->ctx;

    if (!name) {
        Node *name_node = (*(n->list))[1];
        name = name_node->token->str_value.c_str();
        n = (*(n->list))[2];
    }

    if (!is_anonymous) {
        gen->units->prefunction_ns = ctx->ns();
    }

    /* Ensure this isn't a no-override core form. */
    if (CoreForms::existsNoOverride(name)) {
        Error *e = new Error(
            ErrorInst::Generator::ThisCoreFormCannotBeOverridden,
            n
        );
        ctx->er->addError(e);
        return false;
    }

    symlist *lst = n->list;

    if (lst->size() < 4) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            n,
            "fn", "3"
        );
        char buf[100];
        sprintf(buf, "%d", (int) lst->size() - 1);
        e->addArgString(buf);
        ctx->er->addError(e);
        return false;
    }

    int next_index = 1;
    int always_inline = 0;
    /* Whole modules, as well as specific functions, can be
     * declared as being compile-time-only. If the global cto
     * value is set to one, that overrides a zero value here.
     * However, a global cto value of zero does not take
     * precedence when a function has explicitly declared that it
     * should be CTO. */
    int my_cto = 0;

    /* Function attributes. */

    Node *test = ((*lst)[next_index]);
    if (test->is_list
            && test->list->at(0)->is_token
            && !(test->list->at(0)->token
                 ->str_value.compare("attr"))) {
        symlist *attr_list = test->list;
        std::vector<Node*>::iterator b = attr_list->begin(),
                                     e = attr_list->end();
        ++b;
        for (; b != e; ++b) {
            if ((*b)->is_list) {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                ctx->er->addError(e);
                return false;
            }
            if (!((*b)->token->str_value.compare("inline"))) {
                always_inline = 1;
            } else if (!((*b)->token->str_value.compare("cto"))) {
                my_cto = 1;
            } else {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                ctx->er->addError(e);
                return false;
            }
        }
        ++next_index;
    }

    if (gen->units->cto) {
        my_cto = 1;
    }

    /* Linkage. */

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

    /* Store the return type index at this point. The return type
     * is not parsed yet, because it may depend on the types of
     * the function parameters. */

    int return_type_index = next_index;

    /* Parse arguments - push onto the list that gets created. */

    Node *nargs = (*lst)[next_index + 1];

    if (!nargs->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            nargs,
            "list", "parameters", "symbol"
        );
        ctx->er->addError(e);
        return false;
    }

    symlist *args = nargs->list;

    Variable *var;

    std::vector<Variable *> *fn_args_internal =
        new std::vector<Variable *>;

    /* Parse argument - need to keep names. */

    std::vector<Node *>::iterator node_iter;
    node_iter = args->begin();

    bool varargs = false;

    while (node_iter != args->end()) {
        var = new Variable();
        var->type = NULL;

        FormArgumentParse(gen, var, (*node_iter), false, false, true);
        if (var->type == NULL) {
            delete var;
            return false;
        }

        if (var->type->is_array) {
            delete var;
            Error *e = new Error(
                ErrorInst::Generator::ArraysCannotBeFunctionParameters,
                (*node_iter)
            );
            ctx->er->addError(e);
            return false;
        }

        if (var->type->base_type == BaseType::Void) {
            delete var;
            if (args->size() != 1) {
                Error *e = new Error(
                    ErrorInst::Generator::VoidMustBeTheOnlyParameter,
                    nargs
                );
                ctx->er->addError(e);
                return false;
            }
            break;
        }

        /* Have to check that none comes after this. */
        if (var->type->base_type == BaseType::VarArgs) {
            if ((args->end() - node_iter) != 1) {
                delete var;
                Error *e = new Error(
                    ErrorInst::Generator::VarArgsMustBeLastParameter,
                    nargs
                );
                ctx->er->addError(e);
                return false;
            }
            varargs = true;
            fn_args_internal->push_back(var);
            break;
        }

        if (var->type->is_function) {
            delete var;
            Error *e = new Error(
                ErrorInst::Generator::NonPointerFunctionParameter,
                (*node_iter)
            );
            ctx->er->addError(e);
            return false;
        }

        fn_args_internal->push_back(var);

        ++node_iter;
    }

    std::vector<llvm::Type*> fn_args;

    /* Convert to llvm args. */

    std::vector<Variable *>::iterator iter;
    iter = fn_args_internal->begin();

    while (iter != fn_args_internal->end()) {
        Type *type = (*iter)->type;
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
        ++iter;
    }

    /* Return type. First, activate an anonymous namespace and add
     * all of the function parameters to it. This is so that if
     * the return type uses a macro that depends on one of those
     * parameters, it will work properly. */

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    for (std::vector<Variable *>::iterator
            b = fn_args_internal->begin(),
            e = fn_args_internal->end();
            b != e;
            ++b) {
        ctx->ns()->addVariable((*b)->name.c_str(), (*b));
    }

    Type *r_type = 
        FormTypeParse(gen, (*lst)[return_type_index], false,
                          false, false, true);

    ctx->deactivateNamespace(anon_name.c_str());

    if (r_type == NULL) {
        return false;
    }
    if (r_type->is_array) {
        Error *e = new Error(
            ErrorInst::Generator::ReturnTypesCannotBeArrays,
            (*lst)[next_index]
        );
        ctx->er->addError(e);
        return false;
    }

    llvm::Type *llvm_r_type =
        ctx->toLLVMType(r_type, NULL, true);
    if (!llvm_r_type) {
        return false;
    }

    /* Create the LLVM function type. If the retval attribute is
     * present, then the LLVM function type will have a return type of
     * void, and a pointer to a value of the return type will be the
     * final parameter. */

    if (r_type->is_retval) {
        fn_args.push_back(ctx->toLLVMType(ctx->tr->getPointerType(r_type),
                                          NULL, true));
        llvm_r_type =
            ctx->toLLVMType(ctx->tr->getBasicType(BaseType::Void),
                            NULL, true);
    }

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_r_type,
            fn_args,
            varargs
        );

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name,
                                    &new_name,
                                    linkage,
                                    fn_args_internal);

    /* todo: extern_c functions in namespaces. */

    Function *dfn =
        new Function(r_type, fn_args_internal, NULL, 0,
                              &new_name, always_inline);
    dfn->linkage = linkage;
    dfn->cto = my_cto;
    if (!strcmp(name, "setf-copy")) {
        dfn->is_setf_fn = 1;
    } else if (!strcmp(name, "setf-assign")) {
        dfn->is_setf_fn = 1;
    } else if (!strcmp(name, "destroy")) {
        dfn->is_destructor = 1;
    }

    /* If the function is a setf function, the return type must be
     * bool. */

    if (dfn->is_setf_fn) {
        if (!r_type->isEqualTo(ctx->tr->type_bool)) {
            Error *e = new Error(
                ErrorInst::Generator::SetfOverridesMustReturnBool,
                (*lst)[return_type_index]
            );
            ctx->er->addError(e);
            return false;
        }
    }

    /* If the function already exists, but has a different
     * prototype, then fail. */

    llvm::Function *temp23;
    if ((temp23 = gen->units->top()->module->getFunction(new_name.c_str()))) {
        Function *temp25 =
            ctx->getFunction(new_name.c_str(), NULL,
                             NULL, 0);
        if (temp25 && !temp25->isEqualTo(dfn)) {
            Error *e = new Error(
                ErrorInst::Generator::RedeclarationOfFunctionOrMacro,
                n,
                name
            );
            ctx->er->addError(e);
            return false;
        }
        if (temp25 && !temp25->attrsAreEqual(dfn)) {
            Error *e = new Error(
                ErrorInst::Generator::AttributesOfDeclAndDefAreDifferent,
                n,
                name
            );
            ctx->er->addError(e);
            return false;
        }
    }

    if (gen->units->top()->once_tag.length() > 0) {
        dfn->once_tag = gen->units->top()->once_tag;
    }

    llvm::Constant *fnc =
        gen->units->top()->module->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    llvm::Function *fn = llvm::dyn_cast<llvm::Function>(fnc);

    /* If fn is null, then the function already exists and the
     * extant function has a different prototype, so it's an
     * invalid redeclaration. If fn is not null, but has content,
     * then it's an invalid redefinition. */

    if ((!fn) || (fn->size())) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfFunctionOrMacro,
            n,
            name
        );
        ctx->er->addError(e);
        return false;
    }

    if (always_inline) {
#if D_LLVM_VERSION_MINOR == 2
        fn->addFnAttr(llvm::Attributes::AlwaysInline);
#else
        fn->addFnAttr(llvm::Attribute::AlwaysInline);
#endif
    }

    fn->setCallingConv(llvm::CallingConv::C);
    fn->setLinkage(
        (lst->size() == (unsigned) (next_index + 2))
        ? (ctx->toLLVMLinkage(override_linkage))
        : (ctx->toLLVMLinkage(linkage))
    );

    llvm::Function::arg_iterator largs = fn->arg_begin();

    iter = fn_args_internal->begin();
    while (iter != fn_args_internal->end()) {
        if ((*iter)->type->base_type == BaseType::VarArgs) {
            break;
        }

        llvm::Value *temp = largs;
        ++largs;
        temp->setName((*iter)->name.c_str());
        (*iter)->value = temp;
        ++iter;
    }

    llvm::Value *lv_return_value = NULL;
    if (r_type->is_retval) {
        lv_return_value = largs;
        lv_return_value->setName("retval");
    }

    /* If this is an extern-c function, if any non-extern-c function
     * with this name already exists, don't add the current
     * function, and vice-versa.
     *
     * (Dropping this for now, because you may well want
     * non-extern-c and extern-c functions to coexist - think
     * 'read', 'write', 'open'.)
     */

    /* Add the function to the context. */
    dfn->llvm_function = fn;
    if (!ctx->ns()->addFunction(name, dfn, n)) {
        return false;
    }

    if (new_function) {
        *new_function = dfn;
    }

    /* If the list has only four arguments, function is a
     * declaration and you can return straightaway. */

    if (lst->size() == (unsigned) (next_index + 2)) {
        return true;
    }

    ctx->activateAnonymousNamespace();
    std::string anon_name2 = ctx->ns()->name;

    gen->getUnit()->pushGlobalFunction(dfn);
    FormProcBodyParse(gen, n, dfn, fn, (next_index + 2),
                          is_anonymous, lv_return_value);
    gen->getUnit()->popGlobalFunction();

    if (!strcmp(name, "main")
            && ctx->getVariable("stdin")
            && ctx->getVariable("stdout")
            && ctx->getVariable("stderr")) {

        std::vector<llvm::Value *> call_args;
        std::vector<Type *> tempparams;
        Function *ic =
            ctx->getFunction("init-channels", &tempparams,
                             NULL, 0);
        if (!ic or !ic->llvm_function) {
            fprintf(stderr,
                    "Internal error: cannot find init-channels "
                    "function.\n");
            abort();
        }

        llvm::Function::iterator i = fn->begin();
        llvm::BasicBlock *b = i;

        if (b->empty()) {
            llvm::CallInst::Create(
                ic->llvm_function,
                llvm::ArrayRef<llvm::Value*>(call_args),
                "",
                b
            );
        } else {
            llvm::Instruction *fnp = b->getFirstNonPHI();
            if (fnp) {
                llvm::CallInst::Create(
                    ic->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args),
                    "",
                    fnp
                );
            } else {
                llvm::CallInst::Create(
                    ic->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args),
                    "",
                    b
                );
            }
        }
    }

    ctx->deactivateNamespace(anon_name2.c_str());

    return true;
}
}
