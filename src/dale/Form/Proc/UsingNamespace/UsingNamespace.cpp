#include "UsingNamespace.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcUsingNamespaceParse(Units *units, Function *fn,
                                 llvm::BasicBlock *block, Node *node,
                                 bool get_address,
                                 bool prefixed_with_core,
                                 ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("using-namespace", node, 1, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ns_name_node = (*lst)[1];

    ns_name_node =
        units->top()->mp->parsePotentialMacroCall(ns_name_node);
    if (!ns_name_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("using-namespace", ns_name_node,
                                  "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("using-namespace", ns_name_node,
                                     "1")) {
        return false;
    }

    const char *ns_name = ns_name_node->token->str_value.c_str();

    bool res = ctx->useNamespace(ns_name);
    if (!res) {
        Error *e = new Error(NamespaceNotInScope, node, ns_name);
        ctx->er->addError(e);
        return false;
    }

    pr->block = block;
    for (std::vector<Node *>::iterator b = (lst->begin() + 2),
                                       e = lst->end();
         b != e; ++b) {
        bool res = FormProcInstParse(units, fn, pr->block, (*b),
                                     get_address, false, NULL, pr);
        if (!res) {
            ctx->unuseNamespace();
            return false;
        }
    }
    ctx->unuseNamespace();

    return true;
}
}
