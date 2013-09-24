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
namespace GetDNodes
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

    if (!ctx->er->assertArgNums("get-dnodes", node, 1, 1)) {
        return false;
    }

    llvm::Value *v = gen->IntNodeToStaticDNode(node->list->at(1), NULL);

    llvm::IRBuilder<> builder(block);

    pr->set(block, gen->type_pdnode, v);
    return true;
}
}
}
}
}
