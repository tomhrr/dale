#include "Parameter.h"

#include <vector>

#include "../../CoreForms/CoreForms.h"
#include "../../Function/Function.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Units/Units.h"
#include "../../llvm_Function.h"
#include "../Linkage/Linkage.h"
#include "../ProcBody/ProcBody.h"
#include "../Type/Type.h"
#include "Config.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormParameterParse(Units *units, Variable *var, Node *node,
                        bool allow_anon_structs, bool allow_bitfields,
                        bool allow_refs, bool allow_anonymous) {
    Context *ctx = units->top()->ctx;

    var->linkage = Linkage::Auto;

    if (allow_anonymous) {
        int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);
        Type *type = FormTypeParse(units, node, allow_anon_structs,
                                   allow_bitfields, allow_refs);
        if (type) {
            var->name.clear();
            var->name.append("anon");
            var->type = type;
            return true;
        }
        ctx->er->popErrors(error_count);
    }

    if (!node->is_list) {
        /* Can only be void or varargs. */
        Token *t = node->token;

        if (t->type != TokenType::String) {
            Error *e = new Error(IncorrectSingleParameterType, node,
                                 "symbol", t->tokenType());
            ctx->er->addError(e);
            return false;
        }

        if (!strcmp(t->str_value.c_str(), "void")) {
            var->type = ctx->tr->type_void;
            return true;
        } else if (!strcmp(t->str_value.c_str(), "...")) {
            var->type = ctx->tr->type_varargs;
            return true;
        } else {
            Error *e = new Error(IncorrectSingleParameterType, node,
                                 "void/...", t->str_value.c_str());
            ctx->er->addError(e);
            return false;
        }
    }

    std::vector<Node *> *lst = node->list;

    if (lst->size() != 2) {
        Error *e = new Error(IncorrectParameterTypeNumberOfArgs, node,
                             2, static_cast<int>(lst->size()));
        ctx->er->addError(e);
        return false;
    }
    Node *name_node = (*lst)[0];

    if (!name_node->is_token) {
        Error *e = new Error(FirstListElementMustBeAtom, name_node);
        ctx->er->addError(e);
        return false;
    }

    Token *name_token = name_node->token;

    if (name_token->type != TokenType::String) {
        Error *e = new Error(FirstListElementMustBeSymbol, name_node);
        ctx->er->addError(e);
        return false;
    }

    var->name.clear();
    var->name.append(name_token->str_value.c_str());

    Type *type = FormTypeParse(units, (*lst)[1], allow_anon_structs,
                               allow_bitfields, allow_refs);
    if (!type) {
        return false;
    }

    var->type = type;

    return true;
}
}
