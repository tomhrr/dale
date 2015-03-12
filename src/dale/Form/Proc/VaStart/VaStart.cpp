#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Cast/Cast.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcVaStartParse(Units *units, Function *fn, llvm::BasicBlock *block,
                     Node *node, bool get_address, bool prefixed_with_core,
                     ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("va-start", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;

    ParseResult pr_valist;
    bool res = FormProcInstParse(units, fn, block, (*lst)[1], false,
                                 false, NULL, &pr_valist);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_valist.block);
    llvm::Function *va_start =
        units->top()->module->getFunction(llvm::StringRef("llvm.va_start"));

    ParseResult pr_pchar;
    res = Operation::Cast(ctx, pr_valist.block, pr_valist.value,
                          pr_valist.type, ctx->tr->type_pchar,
                          node, 0, &pr_pchar);
    if (!res) {
        return false;
    }

    std::vector<llvm::Value*> call_args;
    call_args.push_back(pr_pchar.value);
    builder.CreateCall(va_start, llvm::ArrayRef<llvm::Value*>(call_args));

    pr->set(pr_pchar.block, ctx->tr->type_void, NULL);

    return true;
}
}
