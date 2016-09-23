#include "CloseScope.h"

#include "../Destruct/Destruct.h"

namespace dale
{
namespace Operation
{
bool
CloseScope(Context *ctx, Function *fn, llvm::BasicBlock *block,
           llvm::Value *skip_value, bool entire)
{
    std::vector<Variable *> stack_vars;
    if (entire && fn->index) {
        ctx->ns()->getVarsAfterIndex(fn->index, &stack_vars);
    } else {
        ctx->ns()->getVariables(&stack_vars);
    }

    ParseResult element;
    element.block = block;

    for (std::vector<Variable *>::iterator
            b = stack_vars.begin(),
            e = stack_vars.end();
            b != e;
            ++b) {
        if (skip_value && ((*b)->value == skip_value)) {
            continue;
        }
        element.set(element.block, (*b)->type, (*b)->value);
        element.do_not_destruct = false;
        Operation::Destruct(ctx, &element, &element, NULL, true);
    }

    return true;
}
}
}
