#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../Inst/Inst.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace PtrGreaterThan
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

    if (!ctx->er->assertArgNums("p>", node, 2, 2)) {
        return NULL;
    }

    symlist *lst = node->list;

    ParseResult p;
    bool res = Form::Proc::Inst::parse(gen, 
                    fn, block, (*lst)[1], get_address, false, NULL, &p
               );
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p>", (*lst)[1], p.type, "1")) {
        return false;
    }

    ParseResult p2;
    res = Form::Proc::Inst::parse(gen, 
               fn, p.block, (*lst)[2], get_address, false, NULL, &p2
          );
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p>", (*lst)[2], p2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(p2.block);
    llvm::Value *vres = llvm::cast<llvm::Value>(
                           builder.CreateICmpUGT(p.value, p2.value)
                       );

    pr->set(p2.block, ctx->tr->getBasicType(Type::Bool), vres);

    p.block = p2.block;
    ParseResult temp;
    gen->destructIfApplicable(&p, NULL, &temp);
    p2.block = temp.block;
    gen->destructIfApplicable(&p2, NULL, &temp);
    pr->block = temp.block;

    return true;
}
}
}
}
}
