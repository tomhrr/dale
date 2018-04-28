#include "UsingNamespace.h"

#include <vector>

#include "../../../Node/Node.h"
#include "../../../Units/Units.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormTopLevelUsingNamespaceParse(Units *units, Node *node) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("using-namespace", node, 1, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ns_node = (*lst)[1];
    ns_node = units->top()->mp->parsePotentialMacroCall(ns_node);
    if (!ns_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("using-namespace", ns_node, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("using-namespace", ns_node, "1")) {
        return false;
    }

    const char *name = ns_node->token->str_value.c_str();
    bool res = ctx->useNamespace(name);
    if (!res) {
        Error *e = new Error(NamespaceNotInScope, ns_node, name);
        ctx->er->addError(e);
        return false;
    }

    for (std::vector<Node *>::iterator b = (lst->begin() + 2),
                                       e = lst->end();
         b != e; ++b) {
        FormTopLevelInstParse(units, (*b));
        ctx->er->flush();
    }

    ctx->unuseNamespace();

    return true;
}
}
