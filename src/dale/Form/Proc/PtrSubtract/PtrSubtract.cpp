#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcPtrSubtractParse(Units *units, Function *fn, llvm::BasicBlock *block,
                         Node *node, bool get_address, bool prefixed_with_core,
                         ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("p-", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ptr_node    = (*lst)[1];
    Node *minuend_node = (*lst)[2];

    ParseResult pr_ptr;
    bool res = FormProcInstParse(units, fn, block, ptr_node, get_address,
                                 false, NULL, &pr_ptr);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p-", (*lst)[1], pr_ptr.type, "1")) {
        return false;
    }

    ParseResult pr_minuend;
    res = FormProcInstParse(units, fn, pr_ptr.block, minuend_node,
                            get_address, false, NULL, &pr_minuend);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerOrIntegerType("p-", (*lst)[2],
                                               pr_minuend.type, "2")) {
        return false;
    }

    block = pr_minuend.block;
    llvm::Value *minuend_value;

    if (pr_minuend.type->isIntegerType()) {
        ParseResult size_pr;
        res = Operation::Sizeof(ctx, pr_minuend.block,
                                pr_ptr.type->points_to, &size_pr);
        if (!res) {
            return false;
        }

        ParseResult minuend_value_pr;
        res = Operation::Cast(ctx, size_pr.block, size_pr.value,
                              size_pr.type, ctx->tr->type_intptr,
                              minuend_node, 0, &minuend_value_pr);
        if (!res) {
            return false;
        }
        block = minuend_value_pr.block;
        minuend_value = minuend_value_pr.value;
    }

    ParseResult ptr_cast_pr;
    res = Operation::Cast(ctx, block, pr_ptr.value, pr_ptr.type,
                          ctx->tr->type_intptr, ptr_node, 0, &ptr_cast_pr);
    if (!res) {
        return false;
    }

    ParseResult minuend_cast_pr;
    res = Operation::Cast(ctx, ptr_cast_pr.block, pr_minuend.value,
                          pr_minuend.type, ctx->tr->type_intptr,
                          minuend_node, 0, &minuend_cast_pr);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(minuend_cast_pr.block);
    llvm::Value *final_minuend =
        (pr_minuend.type->isIntegerType())
            ? builder.CreateMul(minuend_value,
                                minuend_cast_pr.value)
            : minuend_cast_pr.value;

    llvm::Value *sum =
        llvm::cast<llvm::Value>(
            builder.CreateSub(ptr_cast_pr.value, final_minuend)
        );

    ParseResult final_res;
    Operation::Cast(ctx, minuend_cast_pr.block, sum, ctx->tr->type_intptr,
                    pr_ptr.type, node, 0, &final_res);

    pr_ptr.block = final_res.block;
    ParseResult pr_destruct;
    res = Operation::Destruct(ctx, &pr_ptr, &pr_destruct);
    if (!res) {
        return false;
    }
    pr_minuend.block = pr_destruct.block;
    res = Operation::Destruct(ctx, &pr_minuend, &pr_destruct);
    if (!res) {
        return false;
    }

    final_res.block = pr_destruct.block;
    pr->set(final_res.block, pr_ptr.type, final_res.value);

    return true;
}
}
