#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace AddressOf
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "parseAddressOf must receive a list!");

    if (!ctx->er->assertArgNums("#", node, 1, -1)) {
        return false;
    }
    symlist *lst = node->list;

    /* If the argument is a single token, and that token
     * designates a variable, then return the address directly. */
    Node *nn = (*lst)[1];
    if (nn->is_token) {
        Element::Variable *var =
            ctx->getVariable(nn->token->str_value.c_str());
        if (var) {
            pr->set(block,
                    ctx->tr->getPointerType(var->type),
                    var->value);
            return true;
        }
    }

    int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);

    /* There's no destruction of the argument here, because
     * there's no corresponding copy with setf (or an overloaded
     * setf). (todo: this comment does not appear to be correct.) */

    bool res =
        gen->parseFunctionBodyInstr(
            fn, block, (*lst)[1], true, NULL, pr
        );

    int diff = ctx->er->getErrorTypeCount(ErrorType::Error)
               - error_count;

    if (!res) {
        std::vector<Error*> errors;
        while (diff--) {
            errors.push_back(ctx->er->popLastError());
        }

        /* If the second element is a token, check if it is the name
         * of a function. If it is, return a pointer to that
         * function. */

        Element::Function *fn;

        if (((*lst)[1])->is_token &&
                (fn =
                     ctx->getFunction(
                         ((*lst)[1])->token->str_value.c_str(),
                         NULL,
                         NULL,
                         0
                     )
                )
           ) {
            /* Also do the 'typed' version where types have been
             * provided (lst->size > 2), regardless of whether it's
             * overloaded. */

            int is_overloaded =
                ctx->isOverloadedFunction(
                    ((*lst)[1])->token->str_value.c_str()
                );
            int lst_size = lst->size();

            if (is_overloaded || lst_size > 2) {
                /* Parse each type, add it to a vector of types, get
                 * the relevant function, return it. */

                std::vector<Element::Type *> types;
                std::vector<Node *>::iterator iter = lst->begin();
                ++iter;
                ++iter;
                while (iter != lst->end()) {
                    Element::Type *type = gen->parseType((*iter),
                                                    false, false);
                    if (!type) {
                        return NULL;
                    }
                    types.push_back(type);
                    ++iter;
                }
                if (types.empty()) {
                    types.push_back(ctx->tr->getBasicType(Type::Void));
                }

                Element::Function *closest_fn = NULL;

                fn =
                    ctx->getFunction(((*lst)[1])->token->str_value.c_str(),
                                     &types,
                                     &closest_fn,
                                     0);

                std::vector<Element::Type *>::iterator titer =
                    types.begin();

                std::string args;
                while (titer != types.end()) {
                    (*titer)->toStringProper(&args);
                    ++titer;
                    if (titer != types.end()) {
                        args.append(" ");
                    }
                }
                if (!fn) {
                    Error *e;
                    if (closest_fn) {
                        std::string expected;
                        std::vector<Element::Variable *>::iterator viter;
                        viter = closest_fn->parameter_types->begin();
                        if (closest_fn->is_macro) {
                            ++viter;
                        }
                        while (viter != closest_fn->parameter_types->end()) {
                            (*viter)->type->toStringProper(&expected);
                            expected.append(" ");
                            ++viter;
                        }
                        if (expected.size() > 0) {
                            expected.erase(expected.size() - 1, 1);
                        }
                        e = new Error(
                            ErrorInst::Generator::OverloadedFunctionOrMacroNotInScopeWithClosest,
                            node,
                            ((*lst)[1])->token->str_value.c_str(),
                            args.c_str(),
                            expected.c_str()
                        );

                    } else {
                        e = new Error(
                            ErrorInst::Generator::OverloadedFunctionOrMacroNotInScope,
                            node,
                            ((*lst)[1])->token->str_value.c_str(),
                            args.c_str()
                        );
                    }
                    ctx->er->addError(e);
                    return NULL;
                }
            }

            Element::Type *type = new Element::Type();
            type->is_function = 1;
            type->return_type = fn->return_type;

            std::vector<Element::Type *> *parameter_types =
                new std::vector<Element::Type *>;

            std::vector<Element::Variable *>::iterator iter;

            iter = fn->parameter_types->begin();

            while (iter != fn->parameter_types->end()) {
                parameter_types->push_back((*iter)->type);
                ++iter;
            }

            type->parameter_types = parameter_types;
            pr->set(block, ctx->tr->getPointerType(type),
                       llvm::cast<llvm::Value>(fn->llvm_function)
                   );
        } else {
            for (std::vector<Error*>::iterator b = errors.begin(),
                    e = errors.end();
                    b != e;
                    ++b) {
                ctx->er->addError((*b));
            }
            return false;
        }
    }

    /* Confirm that pr's value "points to" something - this error
     * message should probably be "cannot take address of
     * non-lvalue" once confirmed all good. */

    if (!pr->type->points_to) {
        Error *e = new Error(
            ErrorInst::Generator::CannotTakeAddressOfNonLvalue,
            (*lst)[1]
        );
        ctx->er->addError(e);
        return false;
    }

    return true;
}
}
}
}
}
