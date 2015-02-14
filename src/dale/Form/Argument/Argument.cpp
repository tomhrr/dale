#include "Argument.h"
#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Function/Function.h"
#include "../../CoreForms/CoreForms.h"
#include "../Linkage/Linkage.h"
#include "../Type/Type.h"
#include "../ProcBody/ProcBody.h"
#include "../../llvm_Function.h"
#include "Config.h"

namespace dale
{
bool FormArgumentParse(Units *units,
            Variable *var,
            Node *top,
            bool allow_anon_structs,
            bool allow_bitfields,
            bool allow_refs)
{
    Context *ctx = units->top()->ctx;

    var->linkage = Linkage::Auto;

    if (!top->is_list) {
        /* Can only be void or varargs. */
        Token *t = top->token;

        if (t->type != TokenType::String) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectSingleParameterType,
                top,
                "symbol", t->tokenType()
            );
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
            Error *e = new Error(
                ErrorInst::Generator::IncorrectSingleParameterType,
                top,
                "'void'/'...'"
            );
            std::string temp;
            temp.append("'")
            .append(t->str_value.c_str())
            .append("'");
            e->addArgString(&temp);
            ctx->er->addError(e);
            return false;
        }
    }
    symlist *lst = top->list;

    if (lst->size() != 2) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectParameterTypeNumberOfArgs,
            top,
            2, (int) lst->size()
        );
        ctx->er->addError(e);
        return false;
    }
    Node *nname = (*lst)[0];

    if (!nname->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            nname
        );
        ctx->er->addError(e);
        return false;
    }

    Token *tname = nname->token;

    if (tname->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            nname
        );
        ctx->er->addError(e);
        return false;
    }

    var->name.clear();
    var->name.append(tname->str_value.c_str());

    Type *type = FormTypeParse(units, (*lst)[1], allow_anon_structs,
                                    allow_bitfields, allow_refs);
    var->type = type;

    return true;
}
}
