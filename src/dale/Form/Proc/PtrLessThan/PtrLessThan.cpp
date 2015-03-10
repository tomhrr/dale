#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"
#include "../../../Operation/Destruct/Destruct.h"

namespace dale
{
bool
FormProcPtrLessThanParse(Units *units, Function *fn,
                         llvm::BasicBlock *block, Node *node,
                         bool get_address, bool prefixed_with_core,
                         ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("p<", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ptr1_node = (*lst)[1];
    Node *ptr2_node = (*lst)[2];

    ParseResult pr_ptr1;
    bool res = FormProcInstParse(units, fn, block, ptr1_node, get_address,
                                 false, NULL, &pr_ptr1);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p<", ptr1_node, pr_ptr1.type, "1")) {
        return false;
    }

    ParseResult pr_ptr2;
    res = FormProcInstParse(units, fn, pr_ptr1.block, ptr2_node, get_address,
                            false, NULL, &pr_ptr2);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p<", ptr2_node, pr_ptr2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_ptr2.block);
    llvm::Value *cmp_res =
        llvm::cast<llvm::Value>(builder.CreateICmpULT(pr_ptr1.value,
                                                      pr_ptr2.value));

    pr_ptr1.block = pr_ptr2.block;
    ParseResult pr_destruct;
    res = Operation::Destruct(ctx, &pr_ptr1, &pr_destruct);
    if (!res) {
        return false;
    }
    pr_ptr2.block = pr_destruct.block;
    res = Operation::Destruct(ctx, &pr_ptr2, &pr_destruct);
    if (!res) {
        return false;
    }

    pr->set(pr_destruct.block, ctx->tr->type_bool, cmp_res);

    return true;
}
}
