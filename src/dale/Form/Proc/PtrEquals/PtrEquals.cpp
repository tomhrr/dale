#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcPtrEqualsParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("p=", node, 2, 2)) {
        return false;
    }

    symlist *lst = node->list;

    ParseResult p1;
    bool mres = FormProcInstParse(gen, 
                         fn, block, (*lst)[1], get_address, false, NULL, &p1
                     );
    if (!mres) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p=", node, p1.type, "1")) {
        return false;
    }

    ParseResult p2;
    mres = FormProcInstParse(gen, 
                          fn, p1.block, (*lst)[2], get_address, 
                          false, NULL,
                          &p2
                      );
    if (!mres) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p=", node, p2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(p2.block);
    llvm::Value *res = llvm::cast<llvm::Value>(
                           builder.CreateICmpEQ(p1.value, p2.value)
                       );

    pr->set(p2.block, ctx->tr->type_bool, res);

    p1.block = p2.block;
    ParseResult ret;
    ret.block = p2.block;
    Operation::Destruct(ctx, &p1, &ret);
    p2.block = ret.block;
    Operation::Destruct(ctx, &p2, &ret);
    pr->block = ret.block;

    return true;
}
}
