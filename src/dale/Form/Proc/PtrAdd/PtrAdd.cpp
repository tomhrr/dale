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
FormProcPtrAddParse(Units *units, Function *fn, llvm::BasicBlock *block,
                    Node *node, bool get_address, bool prefixed_with_core,
                    ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("p+", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ptr_node    = (*lst)[1];
    Node *addend_node = (*lst)[2];

    ParseResult pr_ptr;
    bool res = FormProcInstParse(units, fn, block, ptr_node, get_address,
                                 false, NULL, &pr_ptr);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p+", (*lst)[1], pr_ptr.type, "1")) {
        return false;
    }

    ParseResult pr_addend;
    res = FormProcInstParse(units, fn, pr_ptr.block, addend_node,
                            get_address, false, NULL, &pr_addend);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerOrIntegerType("p+", (*lst)[2],
                                               pr_addend.type, "2")) {
        return false;
    }

    block = pr_addend.block;
    llvm::Value *addend_value;

    if (pr_addend.type->isIntegerType()) {
        ParseResult size_pr;
        res = Operation::Sizeof(ctx, pr_addend.block,
                                pr_ptr.type->points_to, &size_pr);
        if (!res) {
            return false;
        }

        ParseResult addend_value_pr;
        res = Operation::Cast(ctx, size_pr.block, size_pr.value,
                              size_pr.type, ctx->tr->type_intptr,
                              addend_node, 0, &addend_value_pr);
        if (!res) {
            return false;
        }
        block = addend_value_pr.block;
        addend_value = addend_value_pr.value;
    }

    ParseResult ptr_cast_pr;
    res = Operation::Cast(ctx, block, pr_ptr.value, pr_ptr.type,
                          ctx->tr->type_intptr, ptr_node, 0, &ptr_cast_pr);
    if (!res) {
        return false;
    }

    ParseResult addend_cast_pr;
    res = Operation::Cast(ctx, ptr_cast_pr.block, pr_addend.value,
                          pr_addend.type, ctx->tr->type_intptr,
                          addend_node, 0, &addend_cast_pr);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(addend_cast_pr.block);
    llvm::Value *final_addend =
        (pr_addend.type->isIntegerType())
            ? builder.CreateMul(addend_value,
                                addend_cast_pr.value)
            : addend_cast_pr.value;

    llvm::Value *sum =
        llvm::cast<llvm::Value>(
            builder.CreateAdd(ptr_cast_pr.value, final_addend)
        );

    ParseResult final_res;
    Operation::Cast(ctx, addend_cast_pr.block, sum, ctx->tr->type_intptr,
                    pr_ptr.type, node, 0, &final_res);

    pr_ptr.block = final_res.block;
    ParseResult pr_destruct;
    res = Operation::Destruct(ctx, &pr_ptr, &pr_destruct);
    if (!res) {
        return false;
    }
    pr_addend.block = pr_destruct.block;
    res = Operation::Destruct(ctx, &pr_addend, &pr_destruct);
    if (!res) {
        return false;
    }

    final_res.block = pr_destruct.block;
    pr->set(final_res.block, pr_ptr.type, final_res.value);

    return true;
}
}
