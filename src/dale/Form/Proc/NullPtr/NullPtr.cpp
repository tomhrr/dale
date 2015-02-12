#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../Type/Type.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcNullPtrParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->units->top()->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("nullptr", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;

    Type *type = FormTypeParse(gen, (*lst)[1], false, false);
    if (!type) {
        return false;
    }
    /* Create a pointer to the provided type. */
    Type *ptype = ctx->tr->getPointerType(type);

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
