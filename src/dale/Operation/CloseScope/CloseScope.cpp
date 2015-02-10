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
    if (entire) {
        ctx->ns()->getVarsAfterIndex(fn->index, &stack_vars);
    } else {
        ctx->ns()->getVariables(&stack_vars);
    }

    ParseResult mnew;
    mnew.block = block;

    for (std::vector<Variable *>::iterator
            b = stack_vars.begin(),
            e = stack_vars.end();
            b != e;
            ++b) {

        if (skip_value && ((*b)->value == skip_value)) {
            continue;
        }
        mnew.type = (*b)->type;
        mnew.value = (*b)->value;
        Operation::Destruct(ctx, &mnew, &mnew, NULL, true);
    }

    return true; 
}
}
}
