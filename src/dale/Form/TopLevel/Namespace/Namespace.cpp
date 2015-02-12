#include "Namespace.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../Inst/Inst.h"

namespace dale
{
bool
FormTopLevelNamespaceParse(Generator *gen,
           Node *node)
{
    Context *ctx = gen->units->top()->ctx;

    if (!ctx->er->assertArgNums("namespace", node, 1, -1)) {
        return false;
    }

    symlist *lst = node->list;
    Node *n = (*lst)[1];
    n = gen->getUnit()->mp->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("namespace", n, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("namespace", n, "1")) {
        return false;
    }

    Token *t = n->token;

    int success = ctx->activateNamespace(t->str_value.c_str());
    if (!success) {
        fprintf(stderr, "Internal error: cannot activate "
                "namespace '%s'.\n",
                t->str_value.c_str());
        abort();
    }

    std::vector<Node *>::iterator symlist_iter;
    symlist_iter = lst->begin();

    /* Skip the namespace token and the name token/form. */

    ++symlist_iter;
    ++symlist_iter;

    while (symlist_iter != lst->end()) {
        FormTopLevelInstParse(gen, (*symlist_iter));
        ctx->er->flush();
        ++symlist_iter;
    }

    ctx->deactivateNamespace(t->str_value.c_str());

    return true;
}
}
