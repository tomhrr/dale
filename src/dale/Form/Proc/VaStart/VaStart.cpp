#include "VaStart.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcVaStartParse(Units *units, Function *fn,
                          llvm::BasicBlock *block, Node *node,
                          bool get_address, bool prefixed_with_core,
                          ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("va-start", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;

    ParseResult valist_pr;
    bool res = FormProcInstParse(units, fn, block, (*lst)[1], false,
                                 false, NULL, &valist_pr);
    if (!res) {
        return false;
    }

    llvm::Function *va_start = units->top()->module->getFunction(
        llvm::StringRef("llvm.va_start"));

    ParseResult pchar_pr;
    res = Operation::Cast(ctx, valist_pr.block, valist_pr.getValue(ctx),
                          valist_pr.type, ctx->tr->type_pchar, node, 0,
                          &pchar_pr);
    if (!res) {
        return false;
    }

    std::vector<llvm::Value *> call_args;
    call_args.push_back(pchar_pr.getValue(ctx));
    llvm::IRBuilder<> builder(valist_pr.block);
    builder.CreateCall(va_start,
                       llvm::ArrayRef<llvm::Value *>(call_args));

    pr->set(pchar_pr.block, ctx->tr->type_void, NULL);

    return true;
}
}
