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
FormProcPtrLessThanParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx; 

    if (!ctx->er->assertArgNums("p<", node, 2, 2)) {
        return false;
    }

    symlist *lst = node->list;

    ParseResult p;
    bool res = FormProcInstParse(gen, 
                    fn, block, (*lst)[1], get_address, false, NULL, &p
               );
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p<", (*lst)[1], p.type, "1")) {
        return false;
    }

    ParseResult p2;
    res = FormProcInstParse(gen, 
               fn, p.block, (*lst)[2], get_address, false, NULL, &p2
          );
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p<", (*lst)[2], p2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(p2.block);
    llvm::Value *vres = llvm::cast<llvm::Value>(
                           builder.CreateICmpULT(p.value, p2.value)
                       );

    pr->set(p2.block, ctx->tr->type_bool, vres);

    p.block = p2.block;
    ParseResult temp;
    Operation::Destruct(ctx, &p, &temp);
    p2.block = temp.block;
    Operation::Destruct(ctx, &p2, &temp);
    pr->block = temp.block;

    return true;
}
}
