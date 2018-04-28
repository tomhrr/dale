#include "Label.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvmUtils/llvmUtils.h"
#include "../../../llvm_Function.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcLabelParse(Units *units, Function *fn,
                        llvm::BasicBlock *block, Node *node,
                        bool get_address, bool prefixed_with_core,
                        ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("label", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *label_node = (*lst)[1];

    if (!ctx->er->assertArgIsAtom("label", label_node, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("label", label_node, "1")) {
        return false;
    }
    const char *label_name = label_node->token->str_value.c_str();

    if (fn->getLabel(label_name)) {
        Error *e = new Error(RedeclarationOfLabel, node, label_name);
        ctx->er->addError(e);
        return false;
    }

    llvm::BasicBlock *new_block = llvm::BasicBlock::Create(
        *getContext(), label_name, block->getParent());

    if (block->size() == 0 || !(block->back().isTerminator())) {
        llvm::IRBuilder<> builder(block);
        builder.CreateBr(new_block);
    }

    int index = ++(ctx->ns()->lv_index);
    Label *label = new Label(new_block, ctx->ns(), index);
    fn->addLabel(label_name, label);

    pr->set(new_block, ctx->tr->type_int, ctx->nt->getNativeInt(0));

    return true;
}
}
