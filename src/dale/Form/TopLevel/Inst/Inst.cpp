#include "Inst.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../CoreForms/CoreForms.h"

#include "../Namespace/Namespace.h"
#include "../UsingNamespace/UsingNamespace.h"
#include "../Include/Include.h"
#include "../Import/Import.h"
#include "../Do/Do.h"
#include "../GlobalVariable/GlobalVariable.h"
#include "../Function/Function.h"
#include "../Struct/Struct.h"
#include "../Macro/Macro.h"
#include "../Enum/Enum.h"
#include "../Def/Def.h"
#include "../Once/Once.h"
#include "../Module/Module.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool
FormTopLevelInstParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    ctx->deleteAnonymousNamespaces();

    if (!node) {
        return false;
    }

    if (!node->is_token && !node->is_list) {
        units->pop();
        if (!units->empty()) {
            Unit *unit = units->top();
            units->top()->ctx = unit->ctx;
            units->top()->once_tag = unit->once_tag;
            return true;
        }
        return false;
    }

    if (!node->is_list) {
        Error *e = new Error(OnlyListsAtTopLevel, node);
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *> *lst = node->list;
    if (!lst->size()) {
        Error *e = new Error(NoEmptyLists, node);
        ctx->er->addError(e);
        return false;
    }

    Node *form_node = (*lst)[0];
    if (!form_node->is_token) {
        Error *e = new Error(FirstListElementMustBeAtom, form_node);
        ctx->er->addError(e);
        return false;
    }

    Token *form_token = form_node->token;
    if (form_token->type != TokenType::String) {
        Error *e = new Error(FirstListElementMustBeSymbol, form_node);
        ctx->er->addError(e);
        return false;
    }

    const char *form = form_token->str_value.c_str();
    bool (*toplevel_form)(Units *units, Node *n) =
        CoreForms::getTopLevel(form);
    if (toplevel_form) {
        toplevel_form(units, node);
        return true;
    }

    Node *new_node = units->top()->mp->parsePotentialMacroCall(node);
    if (!new_node) {
        return false;
    }
    if (new_node != node) {
        return FormTopLevelInstParse(units, new_node);
    }

    Error *e = new Error(NotInScope, form_node, form);
    ctx->er->addError(e);
    return false;
}
}
