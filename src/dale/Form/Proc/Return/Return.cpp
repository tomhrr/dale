#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/CloseScope/CloseScope.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcReturnParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "parseReturn must receive a list!");

    symlist *lst = node->list;

    if (!ctx->er->assertArgNums("return", node, 0, 1)) {
        return false;
    }
    if (lst->size() == 1) {
        llvm::IRBuilder<> builder(block);
        Operation::CloseScope(ctx, fn, block, NULL, true);
        builder.CreateRetVoid();
        pr->set(block, ctx->tr->type_void, NULL);
        pr->do_not_destruct       = 1;
        pr->do_not_copy_with_setf = 1;
        pr->treat_as_terminator   = 1;
        return true;
    }

    ParseResult p;
    bool res =
        FormProcInstParse(gen, 
            fn, block, (*lst)[1], get_address, false, NULL, &p
        );
    if (!res) {
        return false;
    }
    Type *real_return_type =
        (fn->hasRetval() ? ctx->tr->getBasicType(BaseType::Void)
                         : fn->return_type);
    if (!ctx->er->assertTypeEquality("return", node, p.type,
                                     real_return_type, false)) {
        return false;
    }
    block = p.block;
    llvm::IRBuilder<> builder(block);
    /* Both branches here create a ParseResult with an integer
     * value but a type that is the same as the return type of the
     * function. This is purposeful - the type is so that if other
     * instructions occur between here and the conclusion of the
     * function (for whatever reason), the last value continues to
     * have the correct type, and in any event the value from a
     * parseReturn parse result should never be used. (Woops, this
     * last part isn't correct - see e.g. the call to CreateCondBr
     * in parseIf. So, return the proper value in the second
     * branch.) */
    if (p.type->base_type == BaseType::Void) {
        Operation::CloseScope(ctx, fn, block, NULL, true);
        builder.SetInsertPoint(block);
        builder.CreateRetVoid();
        pr->set(block, ctx->tr->type_void, NULL);
        pr->do_not_destruct       = 1;
        pr->do_not_copy_with_setf = 1;
        pr->treat_as_terminator   = 1;
        return true;
    } else {
        Operation::CloseScope(ctx, fn, block, NULL, true);
        builder.SetInsertPoint(block);
        builder.CreateRet(p.value);
        pr->set(block, fn->return_type, p.value);
        pr->do_not_destruct       = 1;
        pr->do_not_copy_with_setf = 1;
        pr->treat_as_terminator   = 1;
        return true;
    }
}
}
