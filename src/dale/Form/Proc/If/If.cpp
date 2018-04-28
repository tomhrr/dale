#include "If.h"

#include <string>
#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/CloseScope/CloseScope.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvmUtils/llvmUtils.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcIfParse(Units *units, Function *fn,
                     llvm::BasicBlock *block, Node *node,
                     bool get_address, bool prefixed_with_core,
                     ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("if", node, 3, 3)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *condition_node = (*lst)[1];
    Node *then_node = (*lst)[2];
    Node *else_node = (lst->size() == 4) ? (*lst)[3] : NULL;

    ParseResult cond_pr;
    bool res = FormProcInstParse(units, fn, block, condition_node,
                                 get_address, false, NULL, &cond_pr);
    if (!res) {
        return false;
    }

    if (cond_pr.type->base_type != BaseType::Bool) {
        std::string type_str;
        cond_pr.type->toString(&type_str);
        Error *e = new Error(IncorrectArgType, condition_node, "if",
                             "bool", "1", type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(
        *getContext(), "then", fn->llvm_function);
    llvm::BasicBlock *else_block = llvm::BasicBlock::Create(
        *getContext(), "else", fn->llvm_function);

    llvm::IRBuilder<> builder_cond(cond_pr.block);
    builder_cond.CreateCondBr(
        builder_cond.CreateTrunc(cond_pr.getValue(ctx),
                                 llvm::Type::getInt1Ty(*getContext())),
        then_block, else_block);

    ParseResult destruct_pr;
    res =
        Operation::Destruct(ctx, &cond_pr, &destruct_pr, &builder_cond);
    if (!res) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    ParseResult then_pr;
    res = FormProcInstParse(units, fn, then_block, then_node,
                            get_address, false, NULL, &then_pr);
    Operation::CloseScope(ctx, fn, then_block, NULL, false);
    ctx->deactivateAnonymousNamespace();
    if (!res) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    ParseResult else_pr;
    res = FormProcInstParse(units, fn, else_block, else_node,
                            get_address, false, NULL, &else_pr);
    Operation::CloseScope(ctx, fn, else_block, NULL, false);
    ctx->deactivateAnonymousNamespace();
    if (!res) {
        return false;
    }

    /* If the last instruction in both of these blocks is already a
     * terminating instruction, or the parse result should be treated
     * as a terminator, then don't add a done block and don't add a
     * phi node. */

    llvm::Instruction *then_instr =
        (then_pr.block->size() > 0) ? &(then_pr.block->back()) : NULL;

    llvm::Instruction *else_instr =
        (else_pr.block->size() > 0) ? &(else_pr.block->back()) : NULL;

    bool then_terminates = (then_instr && then_instr->isTerminator()) ||
                           then_pr.treat_as_terminator;

    bool else_terminates = (else_instr && else_instr->isTerminator()) ||
                           else_pr.treat_as_terminator;

    if (then_terminates && else_terminates) {
        pr->set(cond_pr.block, ctx->tr->type_void, NULL);
        return true;
    }

    /* If the last instruction in one of these blocks is a terminator
     * or should be treated as such, a phi node is unnecessary.  The
     * if expression will evaluate to the last value from the
     * non-terminating block. */

    if (then_terminates && !else_terminates) {
        llvm::BasicBlock *done_block = llvm::BasicBlock::Create(
            *getContext(), "done_then_no_else", fn->llvm_function);

        llvm::IRBuilder<> builder_final(else_pr.block);
        else_pr.getValue(ctx);
        builder_final.CreateBr(done_block);

        pr->set(done_block, else_pr.type,
                llvm::cast<llvm::Value>(else_pr.getValue(ctx)));
        return true;
    }

    if (else_terminates && !then_terminates) {
        llvm::BasicBlock *done_block = llvm::BasicBlock::Create(
            *getContext(), "done_else_no_then", fn->llvm_function);

        llvm::IRBuilder<> builder_final(then_pr.block);
        then_pr.getValue(ctx);
        builder_final.CreateBr(done_block);

        pr->set(done_block, then_pr.type,
                llvm::cast<llvm::Value>(then_pr.getValue(ctx)));
        return true;
    }

    /* If the types don't match, or both types are void, then the if
     * expression will evaluate to void, and the phi node is
     * unnecessary. */

    if (!then_pr.type->isEqualTo(else_pr.type) ||
        (then_pr.type->isEqualTo(ctx->tr->type_void) &&
         else_pr.type->isEqualTo(ctx->tr->type_void))) {
        llvm::BasicBlock *done_block = llvm::BasicBlock::Create(
            *getContext(), "done_different_types", fn->llvm_function);

        llvm::IRBuilder<> builder_then_final(then_pr.block);
        builder_then_final.CreateBr(done_block);

        llvm::IRBuilder<> builder_else_final(else_pr.block);
        builder_else_final.CreateBr(done_block);

        pr->set(done_block, ctx->tr->type_void, NULL);
        return true;
    }

    llvm::BasicBlock *done_block = llvm::BasicBlock::Create(
        *getContext(), "done_phi", fn->llvm_function);

    llvm::IRBuilder<> builder_then(then_pr.block);
    then_pr.getValue(ctx);
    builder_then.CreateBr(done_block);
    llvm::IRBuilder<> builder_else(else_pr.block);
    else_pr.getValue(ctx);
    builder_else.CreateBr(done_block);

    llvm::Type *llvm_then_type =
        ctx->toLLVMType(then_pr.type, NULL, true, false);
    if (!llvm_then_type) {
        return false;
    }

    llvm::IRBuilder<> builder_done(done_block);
    llvm::PHINode *pn = builder_done.CreatePHI(llvm_then_type, 0);

    pn->addIncoming(then_pr.getValue(ctx), then_pr.block);
    pn->addIncoming(else_pr.getValue(ctx), else_pr.block);

    pr->set(done_block, then_pr.type, llvm::cast<llvm::Value>(pn));

    /* There's no need to re-copy the value here, since it's coming
     * straight out of then or else. */
    pr->freshly_copied = 1;

    return true;
}
}
