#include "PtrLessThan.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvmUtils/llvmUtils.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcPtrLessThanParse(Units *units, Function *fn,
                              llvm::BasicBlock *block, Node *node,
                              bool get_address, bool prefixed_with_core,
                              ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("p<", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ptr1_node = (*lst)[1];
    Node *ptr2_node = (*lst)[2];

    ParseResult ptr_pr1;
    bool res = FormProcInstParse(units, fn, block, ptr1_node,
                                 get_address, false, NULL, &ptr_pr1);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p<", ptr1_node, ptr_pr1.type,
                                      "1")) {
        return false;
    }

    ParseResult ptr_pr2;
    res = FormProcInstParse(units, fn, ptr_pr1.block, ptr2_node,
                            get_address, false, NULL, &ptr_pr2);
    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("p<", ptr2_node, ptr_pr2.type,
                                      "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(ptr_pr2.block);
    llvm::Value *cmp_res = builder.CreateZExt(
        llvm::cast<llvm::Value>(builder.CreateICmpULT(
            ptr_pr1.getValue(ctx), ptr_pr2.getValue(ctx))),
        llvm::Type::getInt8Ty(*getContext()));

    ptr_pr1.block = ptr_pr2.block;
    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &ptr_pr1, &destruct_pr);
    if (!res) {
        return false;
    }
    ptr_pr2.block = destruct_pr.block;
    res = Operation::Destruct(ctx, &ptr_pr2, &destruct_pr);
    if (!res) {
        return false;
    }

    pr->set(destruct_pr.block, ctx->tr->type_bool, cmp_res);

    return true;
}
}
