#include "Return.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/CloseScope/CloseScope.h"
#include "../../../Operation/Copy/Copy.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcReturnParse(Units *units, Function *fn,
                         llvm::BasicBlock *block, Node *node,
                         bool get_address, bool prefixed_with_core,
                         ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;

    if (!ctx->er->assertArgNums("return", node, 0, 1)) {
        return false;
    }

    pr->do_not_destruct = true;
    pr->do_not_copy_with_setf = true;
    pr->treat_as_terminator = true;

    if (lst->size() == 1) {
        llvm::IRBuilder<> builder(block);
        Operation::CloseScope(ctx, fn, block, NULL, true);
        builder.CreateRetVoid();
        pr->set(block, ctx->tr->type_void, NULL);
        return true;
    }

    ParseResult value_pr;
    bool res = FormProcInstParse(units, fn, block, (*lst)[1],
                                 get_address, false, NULL, &value_pr);
    if (!res) {
        return false;
    }

    Type *real_return_type =
        (fn->hasRetval() ? ctx->tr->type_void : fn->return_type);
    if (!ctx->er->assertTypeEquality("return", node, value_pr.type,
                                     real_return_type, false)) {
        return false;
    }

    res = Operation::Copy(units->top()->ctx, fn, node, &value_pr,
                          &value_pr);
    if (!res) {
        return false;
    }

    block = value_pr.block;
    Operation::CloseScope(ctx, fn, block, NULL, true);

    llvm::IRBuilder<> builder(block);
    builder.SetInsertPoint(block);

    if (value_pr.type->base_type == BaseType::Void) {
        builder.CreateRetVoid();
        pr->set(block, ctx->tr->type_void, NULL);
    } else {
        builder.CreateRet(value_pr.getValue(ctx));
        pr->set(block, fn->return_type, value_pr.getValue(ctx));
    }

    return true;
}
}
