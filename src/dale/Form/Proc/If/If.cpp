#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../Operation/CloseScope/CloseScope.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool
FormProcIfParse(Units *units, Function *fn, llvm::BasicBlock *block,
                Node *node, bool get_address, bool prefixed_with_core,
                ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("if", node, 3, 3)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *condition_node = (*lst)[1];
    Node *then_node      = (*lst)[2];
    Node *else_node      = (*lst)[3];

    ParseResult pr_cond;
    bool res = FormProcInstParse(units, fn, block, condition_node,
                                 get_address, false, NULL, &pr_cond);
    if (!res) {
        return false;
    }

    if (pr_cond.type->base_type != BaseType::Bool) {
        std::string type_str;
        pr->type->toString(&type_str);
        Error *e = new Error(IncorrectArgType, condition_node,
                             "if", "bool", "1", type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    llvm::BasicBlock *then_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "then", fn->llvm_function);
    llvm::BasicBlock *else_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "else", fn->llvm_function);

    llvm::IRBuilder<> builder_cond(pr_cond.block);
    builder_cond.CreateCondBr(pr_cond.value, then_block, else_block);

    ParseResult pr_destruct;
    res = Operation::Destruct(ctx, &pr_cond, &pr_destruct, &builder_cond);
    if (!res) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    ParseResult pr_then;
    res = FormProcInstParse(units, fn, then_block, then_node, get_address,
                            false, NULL, &pr_then);
    Operation::CloseScope(ctx, fn, then_block, NULL, false);
    ctx->deactivateAnonymousNamespace();
    if (!res) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    ParseResult pr_else;
    res = FormProcInstParse(units, fn, else_block, else_node, get_address,
                           false, NULL, &pr_else);
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
        (pr_then.block->size() > 0)
            ? &(pr_then.block->back())
            : NULL;

    llvm::Instruction *else_instr =
        (pr_else.block->size() > 0)
            ? &(pr_else.block->back())
            : NULL;

    bool then_terminates =
        (then_instr && then_instr->isTerminator())
            || pr_then.treat_as_terminator;

    bool else_terminates =
        (else_instr && else_instr->isTerminator())
            || pr_else.treat_as_terminator;

    if (then_terminates && else_terminates) {
        pr_cond.copyTo(pr);
        return true;
    }

    /* If the last instruction in one of these blocks is a terminator
     * or should be treated as such, a phi node is unnecessary.  The
     * result of the if statement will be the last value from the
     * non-terminating block. */

    if (then_terminates && !else_terminates) {
        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "done_then_no_else", fn->llvm_function);

        llvm::IRBuilder<> builder_final(pr_else.block);
        builder_final.CreateBr(done_block);

        pr->set(done_block, pr_else.type,
              llvm::cast<llvm::Value>(pr_else.value));
        return true;
    }

    if (else_terminates && !then_terminates) {
        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "done_else_no_then", fn->llvm_function);

        llvm::IRBuilder<> builder_final(pr_then.block);
        builder_final.CreateBr(done_block);

        pr->set(done_block, pr_then.type,
              llvm::cast<llvm::Value>(pr_then.value));
        return true;
    }

    /* If neither branch terminates, then the values of both branches
     * must be of the same type. */

    if (!pr_then.type->isEqualTo(pr_else.type)) {
        std::string then_type;
        std::string else_type;
        pr_then.type->toString(&then_type);
        pr_else.type->toString(&else_type);
        Error *e = new Error(IfBranchesHaveDifferentTypes,
                             node, then_type.c_str(), else_type.c_str());
        ctx->er->addError(e);
        return false;
    }

    llvm::BasicBlock *done_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "done_phi", fn->llvm_function);

    llvm::IRBuilder<> builder_then(pr_then.block);
    builder_then.CreateBr(done_block);
    llvm::IRBuilder<> builder_else(pr_else.block);
    builder_else.CreateBr(done_block);

    llvm::Type *llvm_then_type =
        ctx->toLLVMType(pr_then.type, NULL, false, false);
    if (!llvm_then_type) {
        return false;
    }

    llvm::IRBuilder<> builder_done(done_block);
    llvm::PHINode *pn = builder_done.CreatePHI(llvm_then_type, 0);

    pn->addIncoming(pr_then.value, pr_then.block);
    pn->addIncoming(pr_else.value, pr_else.block);

    pr->set(done_block, pr_then.type,
            llvm::cast<llvm::Value>(pn));

    /* There's no need to re-copy the value here, since it's coming
     * straight out of then or else. */
    pr->freshly_copied = 1;

    return true;
}
}
