#include "AddressOf.h"

#include <string>
#include <vector>

#include "../../../BaseType/BaseType.h"
#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Type/Type.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcAddressOfParse(Units *units, Function *fn,
                            llvm::BasicBlock *block, Node *node,
                            bool get_address, bool prefixed_with_core,
                            ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("#", node, 1, -1)) {
        return false;
    }
    std::vector<Node *> *lst = node->list;

    /* If the argument is a single token, and that token denotes a
     * variable, then return the address directly. */

    Node *target = (*lst)[1];
    const char *target_str =
        (target->is_token ? target->token->str_value.c_str() : NULL);

    if (target->is_token) {
        Variable *var = ctx->getVariable(target_str);
        if (var) {
            pr->set(block, ctx->tr->getPointerType(var->type),
                    var->value);
            return true;
        }
    }

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    bool res = FormProcInstParse(units, fn, block, (*lst)[1], false,
                                 false, NULL, pr);

    if (res) {
        if (!pr->value_is_lvalue) {
            Error *e = new Error(CannotTakeAddressOfNonLvalue, target);
            ctx->er->addError(e);
            return false;
        }

        ParseResult newpr;
        bool ga_res = pr->getAddressOfValue(ctx, &newpr);
        if (!ga_res) {
            return false;
        }

        newpr.copyTo(pr);
        pr->value_is_lvalue = 0;
        pr->address_of_value = NULL;
        pr->type_of_address_of_value = NULL;

        return true;
    }

    /* If standard parsing is unsuccessful, see if the target node
     * refers to a function.  If it does not, then return immediately,
     * since the error reporter already contains the errors related to
     * the previous parse instruction. */

    Function *target_fn =
        (target_str ? ctx->getFunction(target_str, NULL, NULL, 0)
                    : NULL);

    if (!target_str || !target_fn) {
        return false;
    }

    ctx->er->popErrors(error_count_begin);

    bool is_overloaded = ctx->isOverloadedFunction(target_str);
    int lst_size = lst->size();

    /* The target node refers to a function.  Handle the 'typed'
     * version here, when the function is overloaded or when types
     * have been provided (lst->size > 2). */

    if (is_overloaded || (lst_size > 2)) {
        std::vector<Type *> types;
        for (std::vector<Node *>::iterator b = lst->begin() + 2,
                                           e = lst->end();
             b != e; ++b) {
            Type *type = FormTypeParse(units, (*b), false, false);
            if (!type) {
                return false;
            }
            types.push_back(type);
        }
        if (types.empty()) {
            types.push_back(ctx->tr->type_void);
        }

        Function *closest_fn = NULL;
        target_fn =
            ctx->getFunction(target_str, &types, &closest_fn, 0);

        std::string args;
        typesToString(&types, &args);

        if (!target_fn) {
            Error *e;
            if (closest_fn) {
                std::string expected;
                typesToString(closest_fn->parameters.begin() +
                                  (closest_fn->is_macro ? 1 : 0),
                              closest_fn->parameters.end(), &expected);
                e = new Error(
                    OverloadedFunctionOrMacroNotInScopeWithClosest,
                    node, target_str, args.c_str(), expected.c_str());
            } else {
                e = new Error(OverloadedFunctionOrMacroNotInScope, node,
                              target_str, args.c_str());
            }
            ctx->er->addError(e);
            return false;
        }
    }

    if (target_fn->cto && (!fn->cto && !fn->is_macro)) {
        Error *e = new Error(CTOAddrFromNonCTO, node);
        ctx->er->addError(e);
        return false;
    }

    Type *type = new Type();
    type->is_function = 1;
    type->return_type = target_fn->return_type;

    for (std::vector<Variable *>::iterator
             b = target_fn->parameters.begin(),
             e = target_fn->parameters.end();
         b != e; ++b) {
        type->parameter_types.push_back((*b)->type);
    }

    pr->set(block, ctx->tr->getPointerType(type),
            llvm::cast<llvm::Value>(target_fn->llvm_function));
    return pr;
}
}
