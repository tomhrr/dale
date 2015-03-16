#include "Once.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../Linkage/Enum/Enum.h"
#include "../../Type/Type.h"
#include "../../ProcBody/ProcBody.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool
FormTopLevelOnceParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("once", node, 1, 1)) {
        return false;
    }
    std::vector<Node *> *lst = node->list;
    Node *once_tag_node = (*lst)[1];
    once_tag_node = units->top()->mp->parsePotentialMacroCall(once_tag_node);
    if (!once_tag_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("once", once_tag_node, "1")) {
        return false;
    }
    const char *once_name = once_tag_node->token->str_value.c_str();
    std::string once_tag(once_name);

    if (units->mr->included_once_tags.find(once_tag) !=
            units->mr->included_once_tags.end()) {
        if (units->size() == 1) {
            Error *e = new Error(CannotOnceTheLastOpenFile, once_tag_node);
            ctx->er->addError(e);
            return false;
        }
        units->pop();
        Unit *unit = units->top();
        units->top()->ctx = unit->ctx;
        units->top()->once_tag.clear();
        units->top()->once_tag = unit->once_tag;
    }
    units->mr->included_once_tags.insert(once_tag);
    units->top()->once_tag = once_tag;
    units->top()->setOnceTag(once_tag);

    return true;
}
}
