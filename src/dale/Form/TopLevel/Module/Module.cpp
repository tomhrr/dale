#include "Module.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool
isValidModuleName(const char *name)
{
    for (int i = 0; i < (int) strlen(name); ++i) {
        char c = name[i];
        if (!(isalnum(c) || (c == '-') || (c == '_') || (c == '.'))) {
            return false;
        }
    }
    return true;
}

bool
FormTopLevelModuleParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    assert(units->module_name.size() == 0);

    if (!ctx->er->assertArgNums("module", node, 1, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *name_node = (*lst)[1];
    name_node = units->top()->mp->parsePotentialMacroCall(name_node);
    if (!name_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("module", name_node, "1")) {
        return false;
    }

    const char *name = name_node->token->str_value.c_str();
    if (!isValidModuleName(name)) {
        Error *e = new Error(InvalidModuleName, name_node, name);
        ctx->er->addError(e);
        return false;
    }

    units->module_name.append("lib");
    units->module_name.append(name);

    if (lst->size() != 3) {
        return true;
    }

    Node *attr_nodes = (*lst)[2];
    attr_nodes = units->top()->mp->parsePotentialMacroCall(attr_nodes);
    if (!attr_nodes) {
        return false;
    }
    if (!ctx->er->assertArgIsList("module", attr_nodes, "2")) {
        return false;
    }
    std::vector<Node *> *attr_list = attr_nodes->list;

    if ((attr_list->size() == 0)
            || !(attr_list->at(0)->is_token)
            || (attr_list->at(0)->token->str_value.compare("attr"))) {
        Error *e = new Error(UnexpectedElement, attr_nodes, "attr", 0, 0);
        ctx->er->addError(e);
        return false;
    }

    for (std::vector<Node*>::iterator b = (attr_list->begin() + 1),
                                      e = attr_list->end();
            b != e;
            ++b) {
        if ((*b)->is_list) {
            Error *e = new Error(InvalidAttribute, (*b));
            ctx->er->addError(e);
            return false;
        }
        if (!((*b)->token->str_value.compare("cto"))) {
            units->cto = true;
        } else {
            Error *e = new Error(InvalidAttribute, (*b));
            ctx->er->addError(e);
            return false;
        }
    }

    return true;
}
}
