#include "Def.h"

#include <string>
#include <vector>

#include "../../../BasicTypes/BasicTypes.h"
#include "../../../CoreForms/CoreForms.h"
#include "../../../Node/Node.h"
#include "../../../Units/Units.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../ProcBody/ProcBody.h"
#include "../../Type/Type.h"
#include "Config.h"

#include "../Function/Function.h"
#include "../GlobalVariable/GlobalVariable.h"
#include "../Macro/Macro.h"
#include "../Struct/Struct.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormTopLevelDefParse(Units *units, Node *node) {
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;
    if (lst->size() != 3) {
        Error *e = new Error(IncorrectNumberOfArgs, node, "def", 2,
                             static_cast<int>(lst->size() - 1));
        ctx->er->addError(e);
        return false;
    }

    Node *name_node = (*lst)[1];
    Node *value_node = (*lst)[2];

    name_node = units->top()->mp->parsePotentialMacroCall(name_node);
    if (!name_node) {
        return false;
    }
    if (!name_node->is_token) {
        Error *e = new Error(IncorrectArgType, name_node, "def",
                             "an atom", "1", "a list");
        ctx->er->addError(e);
        return false;
    }

    Token *name = name_node->token;

    if (name->type != TokenType::String) {
        Error *e = new Error(IncorrectArgType, name_node, "def",
                             "a symbol", "1", name->tokenType());
        ctx->er->addError(e);
        return false;
    }

    if (!value_node->is_list) {
        Error *e = new Error(IncorrectArgType, value_node, "def",
                             "a list", "2", "an atom");
        ctx->er->addError(e);
        return false;
    }

    value_node = units->top()->mp->parsePotentialMacroCall(value_node);
    if (!value_node) {
        return false;
    }

    std::vector<Node *> *sub_lst = value_node->list;
    Node *form_node = (*sub_lst)[0];
    if (!form_node->is_token) {
        Error *e = new Error(IncorrectArgType, form_node, "def",
                             "an atom", "2:1", "a list");
        ctx->er->addError(e);
        return false;
    }

    Token *form_token = form_node->token;
    if (form_token->type != TokenType::String) {
        Error *e =
            new Error(IncorrectArgType, form_node, "def", "a symbol",
                      "2:1", form_token->tokenType());
        ctx->er->addError(e);
        return false;
    }

    std::string form = form_token->str_value;

    std::vector<std::string> ns_parts;
    std::string ss_name(name->str_value);
    splitString(&ss_name, &ns_parts, '.');
    std::string unqualified_name = ns_parts.back();
    ns_parts.pop_back();
    ctx->activateNamespaces(&ns_parts);

    if (!form.compare("fn")) {
        FormTopLevelFunctionParse(units, value_node,
                                  unqualified_name.c_str());
    } else if (!form.compare("var")) {
        FormTopLevelGlobalVariableParse(units, value_node,
                                        unqualified_name.c_str());
    } else if (!form.compare("struct")) {
        FormTopLevelStructParse(units, value_node,
                                unqualified_name.c_str());
    } else if (!form.compare("macro")) {
        FormTopLevelMacroParse(units, value_node,
                               unqualified_name.c_str());
    } else {
        Error *e =
            new Error(IncorrectArgType, form_node, "def",
                      "fn/var/struct/macro", "2:1", form.c_str());
        ctx->er->addError(e);
        ctx->deactivateNamespaces(&ns_parts);
        return false;
    }

    ctx->deactivateNamespaces(&ns_parts);

    return true;
}
}
