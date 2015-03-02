#include "Def.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../BasicTypes/BasicTypes.h"
#include "../../../CoreForms/CoreForms.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../Linkage/Enum/Enum.h"
#include "../../Type/Type.h"
#include "../../ProcBody/ProcBody.h"

#include "../GlobalVariable/GlobalVariable.h"
#include "../Function/Function.h"
#include "../Struct/Struct.h"
#include "../Macro/Macro.h"
#include "../Enum/Enum.h" 

namespace dale
{
bool
FormTopLevelDefParse(Units *units,
           Node *top)
{
    Context *ctx = units->top()->ctx;

    symlist *lst = top->list;

    if (lst->size() != 3) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectNumberOfArgs,
            top,
            "def", 2, (int) (lst->size() - 1)
        );
        ctx->er->addError(e);
        return false;
    }

    Node *n = (*lst)[1];

    n = units->top()->mp->parsePotentialMacroCall(n);
    if (!n) {
        return false;
    }
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "an atom", "1", "a list"
        );
        ctx->er->addError(e);
        return false;
    }

    Token *name = n->token;

    if (name->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "a symbol", "1", name->tokenType()
        );
        ctx->er->addError(e);
        return false;
    }

    n = (*lst)[2];

    if (!n->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "a list", "2", "an atom"
        );
        ctx->er->addError(e);
        return false;
    }

    n = units->top()->mp->parsePotentialMacroCall(n);
    if (!n) {
        return false;
    }

    symlist *sublst = n->list;

    Node *subn = (*sublst)[0];

    if (!subn->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "an atom", "2:1", "a list"
        );
        ctx->er->addError(e);
        return false;
    }

    Token *subt = subn->token;

    if (subt->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "a symbol", "2:1", subt->tokenType()
        );
        ctx->er->addError(e);
        return false;
    }

    if (!subt->str_value.compare("fn")) {
        FormTopLevelFunctionParse(units, top);
    } else if (!subt->str_value.compare("var")) {
        FormTopLevelGlobalVariableParse(units, top);
    } else if (!subt->str_value.compare("struct")) {
        FormTopLevelStructParse(units, top);
    } else if (!subt->str_value.compare("macro")) {
        FormTopLevelMacroParse(units, top);
    } else if (!subt->str_value.compare("enum")) {
        FormTopLevelEnumParse(units, top);
    } else {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "'fn'/'var'/'struct'/'macro'",
            "2:1"
        );
        std::string temp;
        temp.append("'");
        temp.append(subt->str_value);
        temp.append("'");
        e->addArgString(&temp);
        ctx->er->addError(e);
        return false;
    }

    return true;
}
}
