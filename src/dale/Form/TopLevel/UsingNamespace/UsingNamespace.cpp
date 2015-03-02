#include "UsingNamespace.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../Inst/Inst.h"

namespace dale
{
bool
FormTopLevelUsingNamespaceParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("using-namespace", node, 1, -1)) {
        return false;
    }

    symlist *lst = node->list;
    Node *n = (*lst)[1];
    n = units->top()->mp->parsePotentialMacroCall(n);
    if (!n) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("using-namespace", n, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("using-namespace", n, "1")) {
        return false;
    }

    Token *t = n->token;

    int res = ctx->useNamespace(t->str_value.c_str());
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::NamespaceNotInScope,
            n,
            t->str_value.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *>::iterator symlist_iter;
    symlist_iter = lst->begin();

    /* Skip the namespace token and the name token/form. */

    ++symlist_iter;
    ++symlist_iter;

    while (symlist_iter != lst->end()) {
        FormTopLevelInstParse(units, (*symlist_iter));
        ctx->er->flush();
        ++symlist_iter;
    }

    ctx->unuseNamespace();

    return true;
}
}
