#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../Inst/Inst.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace If
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    assert(node->list && "must receive a list!");

    Context *ctx = gen->ctx;

    if (!ctx->er->assertArgNums("if", node, 3, 3)) {
        return false;
    }

    symlist *lst = node->list;

    ParseResult pr_cond;
    bool res =
        Form::Proc::Inst::parse(gen, fn, block, (*lst)[1], get_address,
                               false,
                               NULL, &pr_cond);
    if (!res) {
        return false;
    }

    if (pr_cond.type->base_type != Type::Bool) {
        std::string temp;
        pr->type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            "if", "bool", "1", temp.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    llvm::IRBuilder<> builder4(pr_cond.block);

    llvm::BasicBlock *then_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "then", fn->llvm_function);
    llvm::BasicBlock *else_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "else", fn->llvm_function);

    builder4.CreateCondBr(pr_cond.value, then_block, else_block);

    ParseResult temp;
    res = gen->destructIfApplicable(&pr_cond, &builder4, &temp);
    if (!res) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    ParseResult pr_then;
    res =
        Form::Proc::Inst::parse(gen, fn, then_block, (*lst)[2], get_address,
                                false,
                               NULL, &pr_then);
    gen->scopeClose(fn, then_block, NULL, false);
    ctx->deactivateAnonymousNamespace();
    if (!res) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    ParseResult pr_else;
    res =
        Form::Proc::Inst::parse(gen, fn, else_block, (*lst)[3], get_address,
                                false,
                               NULL, &pr_else);
    gen->scopeClose(fn, else_block, NULL, false);
    ctx->deactivateAnonymousNamespace();
    if (!res) {
        return false;
    }

    /* If the last instruction in both of these blocks is already
     * a terminating instruction, or the parseResult said treat as
     * terminator, don't add a done block and don't add a phi
     * node. */

    llvm::Instruction *then_instr =
        (pr_then.block->size() > 0)
        ? &(pr_then.block->back())
        : NULL;

    llvm::Instruction *else_instr =
        (pr_else.block->size() > 0)
        ? &(pr_else.block->back())
        : NULL;

    int then_terminates =
        (then_instr && then_instr->isTerminator())
        || pr_then.treat_as_terminator;

    int else_terminates =
        (else_instr && else_instr->isTerminator())
        || pr_else.treat_as_terminator;

    if (then_terminates && else_terminates) {
        /* Nothing else to do here - note that this block should
         * not be used - if this is an if that returns a value,
         * then the correct block gets returned (the done block
         * down the bottom), but if it's terminating, then that's not
         * the case. */

        /* todo: this used to return pr_cond, which can't have been
         * right, but creating a new block may lead to other problems.
         * yep - because if this is used, there are problems, since it
         * has no predecessors. Let alone that it really should not be
         * used. */
        pr_cond.copyTo(pr);
        return true;
    }

    /* If the last instruction in one of these blocks is a terminator
     * or should be treated as such, a phi node is unnecessary. The
     * result of the if statement will be the last value from the
     * non-terminating block. */

    if (then_terminates && !else_terminates) {
        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "done_then_no_else", fn->llvm_function);

        llvm::IRBuilder<> builder2(pr_else.block);
        builder2.CreateBr(done_block);

        pr->set(done_block, pr_else.type,
              llvm::cast<llvm::Value>(pr_else.value));
        return true;
    }

    if (else_terminates && !then_terminates) {
        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "done_else_no_then", fn->llvm_function);

        llvm::IRBuilder<> builder2(pr_then.block);
        builder2.CreateBr(done_block);

        pr->set(done_block, pr_then.type,
              llvm::cast<llvm::Value>(pr_then.value));
        return true;
    }

    /* If neither branch terminates, then the values of both
     * branches must be of the same type. */

    if (!pr_then.type->isEqualTo(pr_else.type)) {
        std::string sthen;
        std::string selse;
        pr_then.type->toStringProper(&sthen);
        pr_else.type->toStringProper(&selse);
        Error *e = new Error(
            ErrorInst::Generator::IfBranchesHaveDifferentTypes,
            node,
            sthen.c_str(), selse.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    llvm::BasicBlock *done_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "done_phi", fn->llvm_function);

    llvm::IRBuilder<> builder(pr_then.block);
    builder.CreateBr(done_block);
    llvm::IRBuilder<> builder2(pr_else.block);
    builder2.CreateBr(done_block);

    llvm::IRBuilder<> builder3(done_block);

    llvm::Type *llvm_then_type =
        ctx->toLLVMType(pr_then.type, NULL, false, false);
    if (!llvm_then_type) {
        return false;
    }
    llvm::PHINode *pn =
        builder3.CreatePHI(llvm_then_type, 0);

    pn->addIncoming(pr_then.value, pr_then.block);
    pn->addIncoming(pr_else.value, pr_else.block);

    pr->set(done_block, pr_then.type,
          llvm::cast<llvm::Value>(pn));

    /* There's no need to re-copy the value here, since it's
     * coming straight out of then or else. */
    pr->freshly_copied = 1;

    return true;
}
}
}
}
}
