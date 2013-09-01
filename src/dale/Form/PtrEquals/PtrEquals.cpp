#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace PtrEquals
{
bool execute(Generator *gen,
             Element::Function *fn,
             llvm::BasicBlock *block,
             Node *node,
             bool get_address,
             bool prefixed_with_core,
             ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    if (!gen->assertArgNums("p=", node, 2, 2)) {
        return false;
    }

    symlist *lst = node->list;

    ParseResult p1;
    bool mres = gen->parseFunctionBodyInstr(
                         fn, block, (*lst)[1], get_address, NULL, &p1
                     );
    if (!mres) {
        return false;
    }
    if (!gen->assertIsPointerType("p=", node, p1.type, "1")) {
        return false;
    }

    ParseResult p2;
    mres = gen->parseFunctionBodyInstr(
                          fn, p1.block, (*lst)[2], get_address, NULL,
                          &p2
                      );
    if (!mres) {
        return false;
    }
    if (!gen->assertIsPointerType("p=", node, p2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(p2.block);
    llvm::Value *res = llvm::cast<llvm::Value>(
                           builder.CreateICmpEQ(p1.value, p2.value)
                       );

    pr->set(p2.block, ctx->tr->getBasicType(Type::Bool), res);

    p1.block = p2.block;
    ParseResult ret;
    ret.block = p2.block;
    if (gen->hasRelevantDestructor(&p1)) {
        gen->destructIfApplicable(&p1, NULL, &ret);
        p2.block = ret.block;
    }
    if (gen->hasRelevantDestructor(&p2)) {
        gen->destructIfApplicable(&p2, NULL, &ret);
    }
    pr->block = ret.block;

    return true;
}
}
}
}
