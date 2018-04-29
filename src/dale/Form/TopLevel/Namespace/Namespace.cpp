#include "Namespace.h"

#include <vector>

#include "../../../Node/Node.h"
#include "../../../Units/Units.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormTopLevelNamespaceParse(Units *units, Node *node) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("namespace", node, 1, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ns_node = (*lst)[1];
    ns_node = units->top()->mp->parsePotentialMacroCall(ns_node);
    if (!ns_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("namespace", ns_node, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("namespace", ns_node, "1")) {
        return false;
    }

    const char *ns_name = ns_node->token->str_value.c_str();
    bool res = ctx->activateNamespace(ns_name);
    assert(res && "cannot activate namespace");
    _unused(res);

    for (std::vector<Node *>::iterator b = (lst->begin() + 2),
                                       e = lst->end();
         b != e; ++b) {
        FormTopLevelInstParse(units, (*b));
        ctx->er->flush();
    }

    ctx->deactivateNamespace(ns_name);
    if (units->prefunction_ns) {
        if (!strcmp(ns_name, units->prefunction_ns->name.c_str())) {
            units->prefunction_ns = NULL;
        }
    }

    return true;
}
}
