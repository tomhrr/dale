#include "PtrAdd.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcPtrAddParse(Units *units, Function *fn,
                         llvm::BasicBlock *block, Node *node,
                         bool get_address, bool prefixed_with_core,
                         ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("p+", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ptr_node = (*lst)[1];
    Node *addend_node = (*lst)[2];

    ParseResult ptr_pr;
    bool res = FormProcInstParse(units, fn, block, ptr_node,
                                 get_address, false, NULL, &ptr_pr);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p+", (*lst)[1], ptr_pr.type,
                                      "1")) {
        return false;
    }

    ParseResult addend_pr;
    res = FormProcInstParse(units, fn, ptr_pr.block, addend_node,
                            get_address, false, NULL, &addend_pr);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerOrIntegerType("p+", (*lst)[2],
                                               addend_pr.type, "2")) {
        return false;
    }

    block = addend_pr.block;
    llvm::Value *addend_value = NULL;

    if (addend_pr.type->isIntegerType()) {
        size_t size =
            Operation::SizeofGet(units->top(), ptr_pr.type->points_to);

        ParseResult addend_value_pr;
        res = Operation::Cast(
            ctx, block,
            llvm::ConstantInt::get(
                ctx->toLLVMType(ctx->tr->type_size, NULL, false), size),
            ctx->tr->type_size, ctx->tr->type_intptr, addend_node, 0,
            &addend_value_pr);
        if (!res) {
            return false;
        }
        block = addend_value_pr.block;
        addend_value = addend_value_pr.getValue(ctx);
    }

    ParseResult ptr_cast_pr;
    res = Operation::Cast(ctx, block, ptr_pr.getValue(ctx), ptr_pr.type,
                          ctx->tr->type_intptr, ptr_node, 0,
                          &ptr_cast_pr);
    if (!res) {
        return false;
    }

    ParseResult addend_cast_pr;
    res = Operation::Cast(
        ctx, ptr_cast_pr.block, addend_pr.getValue(ctx), addend_pr.type,
        ctx->tr->type_intptr, addend_node, 0, &addend_cast_pr);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(addend_cast_pr.block);
    llvm::Value *final_addend =
        (addend_pr.type->isIntegerType())
            ? builder.CreateMul(addend_value,
                                addend_cast_pr.getValue(ctx))
            : addend_cast_pr.getValue(ctx);

    llvm::Value *sum = llvm::cast<llvm::Value>(
        builder.CreateAdd(ptr_cast_pr.getValue(ctx), final_addend));

    ParseResult final_res;
    Operation::Cast(ctx, addend_cast_pr.block, sum,
                    ctx->tr->type_intptr, ptr_pr.type, node, 0,
                    &final_res);

    ptr_pr.block = final_res.block;
    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &ptr_pr, &destruct_pr);
    if (!res) {
        return false;
    }
    addend_pr.block = destruct_pr.block;
    res = Operation::Destruct(ctx, &addend_pr, &destruct_pr);
    if (!res) {
        return false;
    }

    final_res.block = destruct_pr.block;
    pr->set(final_res.block, ptr_pr.type, final_res.getValue(ctx));

    return true;
}
}
