#include "Goto.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"

namespace dale {
bool FormProcGotoParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("goto", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *label_node = (*lst)[1];

    if (!ctx->er->assertArgIsAtom("goto", label_node, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("goto", label_node, "1")) {
        return false;
    }

    const char *label_name = label_node->token->str_value.c_str();
    Label *label = fn->getLabel(label_name);

    if (!label) {
        /* If the label does not exist, then this goto becomes a
         * deferred goto. */

        DeferredGoto *dg = new DeferredGoto;
        dg->label_name.append(label_name);
        dg->ns = ctx->ns();
        dg->index = ctx->ns()->lv_index;
        dg->block_marker = block;

        Node *myn = new Node();
        node->copyTo(myn);
        dg->node = myn;

        if (block->size() == 0) {
            dg->marker = NULL;
        } else {
            llvm::Instruction *tinstr = &(block->back());
            dg->marker = tinstr;
        }

        fn->deferred_gotos.push_back(dg);

        /* Add a no-op instruction to the block, so that label parsing
         * does not add an implicit branch from the previous block to
         * the new block.  This will occur when the previous block
         * does not end with a terminator, which will be the case here
         * because the goto is deferred. */

        llvm::IRBuilder<> builder(block);
        builder.CreateBitCast(ctx->nt->getLLVMZero(),
                              ctx->nt->getNativeIntType());
    } else {
        /* Get all the variables that exist within the current scope
         * and have an index greater than the label's index.  Add a
         * Destruct call for each of these variables. */

        std::vector<Variable *> myvars;
        ctx->ns()->getVarsAfterIndex(label->index, &myvars);
        ParseResult destruct_pr;
        destruct_pr.block = block;
        destruct_pr.do_not_destruct = false;
        llvm::IRBuilder<> builder(destruct_pr.block);
        for (std::vector<Variable *>::iterator b = myvars.begin(),
                                               e = myvars.end();
             b != e; ++b) {
            builder.SetInsertPoint(destruct_pr.block);
            Variable *var = (*b);
            destruct_pr.type = var->type;
            destruct_pr.address_of_value = var->value;
            destruct_pr.set(destruct_pr.block, destruct_pr.type, NULL);

            Operation::Destruct(ctx, &destruct_pr, &destruct_pr);
        }

        block = destruct_pr.block;
        builder.SetInsertPoint(block);
        builder.CreateBr(label->block);
    }

    pr->set(block, ctx->tr->type_int, ctx->nt->getLLVMZero());
    if (!label) {
        pr->treat_as_terminator = true;
    }
    pr->do_not_copy_with_setf = true;

    return true;
}
}
