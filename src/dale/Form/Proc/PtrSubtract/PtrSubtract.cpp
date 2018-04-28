#include "PtrSubtract.h"

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
bool FormProcPtrSubtractParse(Units *units, Function *fn,
                              llvm::BasicBlock *block, Node *node,
                              bool get_address, bool prefixed_with_core,
                              ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("p-", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ptr_node = (*lst)[1];
    Node *minuend_node = (*lst)[2];

    ParseResult ptr_pr;
    bool res = FormProcInstParse(units, fn, block, ptr_node,
                                 get_address, false, NULL, &ptr_pr);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p-", (*lst)[1], ptr_pr.type,
                                      "1")) {
        return false;
    }

    ParseResult minuend_pr;
    res = FormProcInstParse(units, fn, ptr_pr.block, minuend_node,
                            get_address, false, NULL, &minuend_pr);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerOrIntegerType("p-", (*lst)[2],
                                               minuend_pr.type, "2")) {
        return false;
    }

    block = minuend_pr.block;
    llvm::Value *minuend_value = NULL;

    if (minuend_pr.type->isIntegerType()) {
        size_t size =
            Operation::SizeofGet(units->top(), ptr_pr.type->points_to);

        ParseResult minuend_value_pr;
        res = Operation::Cast(
            ctx, block,
            llvm::ConstantInt::get(
                ctx->toLLVMType(ctx->tr->type_size, NULL, false), size),
            ctx->tr->type_size, ctx->tr->type_intptr, minuend_node, 0,
            &minuend_value_pr);
        if (!res) {
            return false;
        }
        block = minuend_value_pr.block;
        minuend_value = minuend_value_pr.getValue(ctx);
    }

    ParseResult ptr_cast_pr;
    res = Operation::Cast(ctx, block, ptr_pr.getValue(ctx), ptr_pr.type,
                          ctx->tr->type_intptr, ptr_node, 0,
                          &ptr_cast_pr);
    if (!res) {
        return false;
    }

    ParseResult minuend_cast_pr;
    res = Operation::Cast(ctx, ptr_cast_pr.block,
                          minuend_pr.getValue(ctx), minuend_pr.type,
                          ctx->tr->type_intptr, minuend_node, 0,
                          &minuend_cast_pr);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(minuend_cast_pr.block);
    llvm::Value *final_minuend =
        (minuend_pr.type->isIntegerType())
            ? builder.CreateMul(minuend_value,
                                minuend_cast_pr.getValue(ctx))
            : minuend_cast_pr.getValue(ctx);

    llvm::Value *sum = llvm::cast<llvm::Value>(
        builder.CreateSub(ptr_cast_pr.getValue(ctx), final_minuend));

    ParseResult final_res;
    Operation::Cast(ctx, minuend_cast_pr.block, sum,
                    ctx->tr->type_intptr, ptr_pr.type, node, 0,
                    &final_res);

    ptr_pr.block = final_res.block;
    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &ptr_pr, &destruct_pr);
    if (!res) {
        return false;
    }
    minuend_pr.block = destruct_pr.block;
    res = Operation::Destruct(ctx, &minuend_pr, &destruct_pr);
    if (!res) {
        return false;
    }

    final_res.block = destruct_pr.block;
    pr->set(final_res.block, ptr_pr.type, final_res.getValue(ctx));

    return true;
}
}
