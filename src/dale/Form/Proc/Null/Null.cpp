#include "Null.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvmUtils/llvmUtils.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcNullParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("null", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *node_value = (*lst)[1];

    /* Previously, this called parsePotentialMacroCall on the
     * expression, and if it evaluated to a form that was guaranteed
     * to return a non-null value (i.e. : or $), it would return an
     * error.  However, due to procedure macro evaluation in some cases
     * depending on the availability of functions (e.g. with
     * operator-macros), this is not done anymore. */

    ParseResult value_pr;
    bool res = FormProcInstParse(units, fn, block, node_value, false,
                                 false, NULL, &value_pr);

    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("null", node_value, value_pr.type,
                                      "1")) {
        return false;
    }

    llvm::IRBuilder<> builder(value_pr.block);
    llvm::Value *int_res = builder.CreatePtrToInt(
        value_pr.getValue(ctx), ctx->nt->getNativeIntType());

    llvm::Value *null_res =
        builder.CreateZExt(llvm::cast<llvm::Value>(builder.CreateICmpEQ(
                               int_res, ctx->nt->getLLVMZero())),
                           llvm::Type::getInt8Ty(*getContext()));

    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &value_pr, &destruct_pr);
    if (!res) {
        return false;
    }

    pr->set(destruct_pr.block, ctx->tr->type_bool, null_res);

    return true;
}
}
