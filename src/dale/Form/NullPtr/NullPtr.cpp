#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace NullPtr
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

    if (!gen->assertArgNums("nullptr", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;

    Element::Type *type = gen->parseType((*lst)[1], false, false);
    if (!type) {
        return false;
    }
    /* Create a pointer to the provided type. */
    Element::Type *ptype = ctx->tr->getPointerType(type);

    llvm::IRBuilder<> builder(block);
    llvm::Type *llvm_ptype =
        ctx->toLLVMType(ptype, NULL, false, false);
    if (!llvm_ptype) {
        return false;
    }

    llvm::Value *res =
        builder.CreateAlloca(llvm_ptype);

    llvm::Value *val =
        builder.CreateIntToPtr(
            ctx->nt->getLLVMZero(),
            llvm_ptype
        );

    builder.CreateStore(val, res);
    pr->set(block, ptype, val);

    return true;
}
}
}
}
