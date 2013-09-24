#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace UsingNamespace
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("using-namespace", node, 1, -1)) {
        return false;
    }

    symlist *lst = node->list;
    Node *n2 = (*lst)[1];
    n2 = gen->parseOptionalMacroCall(n2);
    if (!n2) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("using-namespace", n2, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("using-namespace", n2, "1")) {
        return false;
    }

    Token *t = n2->token;

    bool res = ctx->useNamespace(t->str_value.c_str());
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::NamespaceNotInScope,
            node,
            t->str_value.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;
    ++node_iter;

    pr->block = block;
    while (node_iter != lst->end()) {
        bool res = gen->parseFunctionBodyInstr(
                       fn, pr->block, (*node_iter), get_address, NULL, pr
                   );
        if (!res) {
            return false;
        }
        ++node_iter;
    }

    ctx->unuseNamespace();

    return true;
}
}
}
}
}
