#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../../../Operation/Cast/Cast.h"
#include "../Inst/Inst.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace VaStart
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    if (!ctx->er->assertArgNums("va-start", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;

    ParseResult pr_valist;
    bool res =
        Form::Proc::Inst::parse(gen, fn, block, (*lst)[1], false,
                                false, NULL,
                               &pr_valist);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_valist.block);
    llvm::Function *va_start =
        gen->mod->getFunction(llvm::StringRef("llvm.va_start"));
    if (!va_start) {
        fprintf(stderr, "Unable to load va_start.");
        abort();
    }

    std::vector<llvm::Value*> call_args;
    ParseResult to_pchar;
    res = Operation::Cast::execute(ctx, pr_valist.block,
                pr_valist.value,
                pr_valist.type,
                ctx->tr->getPointerType(ctx->tr->getBasicType(Type::Char)),
                node, 0, &to_pchar);
    if (!res) {
        return false;
    }
    call_args.push_back(to_pchar.value);
    builder.CreateCall(va_start,
                       llvm::ArrayRef<llvm::Value*>(call_args));

    pr->set(to_pchar.block, ctx->tr->getBasicType(Type::Void), NULL);

    return true;
}
}
}
}
}
