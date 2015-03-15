#include "Macro.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../CoreForms/CoreForms.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../ProcBody/ProcBody.h"
#include "../../Argument/Argument.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool
FormTopLevelMacroParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    const char *name = (*node->list)[1]->token->str_value.c_str();

    if (CoreForms::exists(name)) {
        Error *e = new Error(NoCoreFormNameInMacro, (*node->list)[2]);
        ctx->er->addError(e);
        return false;
    }

    Node *top = (*node->list)[2];
    std::vector<Node *> *lst = top->list;

    if (lst->size() < 3) {
        Error *e = new Error(IncorrectMinimumNumberOfArgs, top,
                             "macro", 2, (int) (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    }

    int linkage = FormLinkageParse(ctx, (*lst)[1]);
    if (!linkage) {
        return false;
    }

    Type *ret_type = ctx->tr->type_pdnode;

    Node *macro_params = (*lst)[2];
    if (!macro_params->is_list) {
        Error *e = new Error(UnexpectedElement, macro_params,
                             "list", "macro parameters", "atom");
        ctx->er->addError(e);
        return false;
    }

    std::vector<Variable *> mc_params_internal;

    /* An implicit MContext argument is added to every macro. */

    Type *mc_type  = ctx->tr->getStructType("MContext");
    Type *pmc_type = ctx->tr->getPointerType(mc_type);
    Variable *mc_var = new Variable("mc", pmc_type);
    mc_var->linkage = Linkage::Auto;
    mc_params_internal.push_back(mc_var);

    bool past_first = false;
    bool varargs = false;

    std::vector<Node *> *params = macro_params->list;
    for (std::vector<Node *>::iterator b = params->begin(),
                                       e = params->end();
            b != e;
            ++b) {
        Variable *var = NULL;
        if (!(*b)->is_token) {
            var = new Variable();
            FormArgumentParse(units, var, (*b), false, false, false);
            if (!var->type) {
                return false;
            }
            mc_params_internal.push_back(var);
        } else {
            std::string *value = &((*b)->token->str_value);
            if (!value->compare("void")) {
                if (past_first || (params->size() > 1)) {
                    Error *e = new Error(VoidMustBeTheOnlyParameter,
                                         macro_params);
                    ctx->er->addError(e);
                    return false;
                }
                break;
            } else if (!value->compare("...")) {
                if ((e - b) != 1) {
                    Error *e = new Error(VarArgsMustBeLastParameter,
                                         macro_params);
                    ctx->er->addError(e);
                    return false;
                }
                var = new Variable();
                var->type = ctx->tr->type_varargs;
                var->linkage = Linkage::Auto;
                mc_params_internal.push_back(var);
                break;
            }
            var = new Variable();
            var->type = ret_type;
            var->linkage = Linkage::Auto;
            var->name.append((*b)->token->str_value);
            past_first = true;
            mc_params_internal.push_back(var);
        }
    }

    std::vector<llvm::Type*> mc_params;

    /* Convert to LLVM parameters.  The MContext argument is converted
     * as per its actual type.  The remaining arguments,
     * notwithstanding the macro parameters' 'actual' types, will
     * always be (p DNode)s. */

    mc_params.push_back(ctx->toLLVMType(mc_params_internal[0]->type,
                                        NULL, false));

    for (std::vector<Variable *>::iterator b = mc_params_internal.begin() + 1,
                                           e = mc_params_internal.end();
            b != e;
            ++b) {
        if ((*b)->type->base_type == BaseType::VarArgs) {
            varargs = true;
            break;
        }
        llvm::Type *temp = ctx->toLLVMType(ret_type, NULL, false);
        if (!temp) {
            return false;
        }
        mc_params.push_back(temp);
    }

    llvm::Type *temp = ctx->toLLVMType(ret_type, NULL, false);
    if (!temp) {
        return false;
    }

    llvm::FunctionType *ft =
        getFunctionType(
            temp,
            mc_params,
            varargs
        );

    std::string new_name;

    ctx->ns()->functionNameToSymbol(name,
                            &new_name,
                            linkage,
                            &mc_params_internal);

    if (units->top()->module->getFunction(llvm::StringRef(new_name.c_str()))) {
        Error *e = new Error(RedeclarationOfFunctionOrMacro, top, name);
        ctx->er->addError(e);
        return false;
    }

    llvm::Constant *fnc =
        units->top()->module->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    llvm::Function *fn = llvm::dyn_cast<llvm::Function>(fnc);

    /* This is probably unnecessary, given the previous
     * getFunction call. */
    if ((!fn) || (fn->size())) {
        Error *e = new Error(RedeclarationOfFunctionOrMacro, top, name);
        ctx->er->addError(e);
        return false;
    }

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(linkage));

    llvm::Function::arg_iterator lparams = fn->arg_begin();

    /* Note that the values of the Variables of the macro's
     * parameter list will not necessarily match the Types of
     * those variables (to support overloading). */

    for (std::vector<Variable *>::iterator b = mc_params_internal.begin(),
                                           e = mc_params_internal.end();
            b != e;
            ++b) {
        if ((*b)->type->base_type == BaseType::VarArgs) {
            break;
        }
        llvm::Value *temp = lparams;
        ++lparams;
        temp->setName((*b)->name.c_str());
        (*b)->value = temp;
    }

    /* Add the macro to the context. */
    Function *dfn =
        new Function(ret_type, &mc_params_internal, fn, 1,
                              &new_name);
    dfn->linkage = linkage;

    if (!ctx->ns()->addFunction(name, dfn, top)) {
        return false;
    }
    if (units->top()->once_tag.length() > 0) {
        dfn->once_tag = units->top()->once_tag;
    }

    /* If the list has only three arguments, the macro is a
     * declaration and you can return straightaway. */

    if (lst->size() == 3) {
        return true;
    }

    /* Previously, Generator had a variable called
     * has_defined_extern_macro, which was set here if the macro had
     * extern scope.  Later, if that variable wasn't set,
     * createConstantMergePass could be added to the pass list.  If
     * problems are had later with that pass, the absence of that
     * variable and associated behaviour probably have something to do
     * with it.  */

    int error_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    units->top()->pushGlobalFunction(dfn);
    FormProcBodyParse(units, top, dfn, fn, 3, 0);
    units->top()->popGlobalFunction();

    ctx->deactivateNamespace(anon_name.c_str());

    int error_post_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count != error_post_count) {
        std::map<std::string, std::vector<Function*>*
        >::iterator i = ctx->ns()->functions.find(name);
        if (i != ctx->ns()->functions.end()) {
            for (std::vector<Function *>::iterator
                    j = i->second->begin(),
                    k = i->second->end();
                    j != k;
                    ++j) {
                if ((*j)->is_macro) {
                    i->second->erase(j);
                    break;
                }
            }
        }
    }

    return true;
}
}
