#include "Copy.h"

namespace dale
{
namespace Operation
{
bool
Copy(Context *ctx, Function *fn, ParseResult *pr, ParseResult *ret_pr)
{
    pr->copyTo(ret_pr);

    /* If this is a setf function, then don't copy, even if it can be
     * done.  This is because, if the setf function is (e.g.) the same
     * as the current function, there will be non-terminating
     * recursion.  It would be possible to limit this to the same
     * function only, but you could have mutual recursion; it would be
     * better for the author to do all of this manually, rather than
     * complicating things.  (It's very possible that this will change
     * in the future.) */
    if (fn->is_setf_fn) {
        return true;
    }
    if (pr->do_not_copy_with_setf) {
        return true;
    }
    /* If the parse result has already been copied, then don't copy
     * it again, since there's no point. */
    if (pr->freshly_copied) {
        return true;
    }

    Type *copy_type = ctx->tr->getPointerType(pr->type);
    std::vector<Type *> types;
    types.push_back(copy_type);
    types.push_back(copy_type);
    Function *over_setf = ctx->getFunction("setf-copy", &types, NULL, 0);
    if (!over_setf) {
        return true;
    }

    llvm::IRBuilder<> builder(pr->block);
    llvm::Type *llvm_type_pr = ctx->toLLVMType(pr->type, NULL, false);
    llvm::Value *result_ptr =
        llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_type_pr));
    llvm::Value *value_ptr =
        llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_type_pr));
    builder.CreateStore(pr->value, value_ptr);

    std::vector<llvm::Value *> call_args;
    call_args.push_back(result_ptr);
    call_args.push_back(value_ptr);
    builder.CreateCall(
        over_setf->llvm_function,
        llvm::ArrayRef<llvm::Value*>(call_args)
    );
    llvm::Value *result = builder.CreateLoad(result_ptr);

    ret_pr->set(pr->block, pr->type, result);
    ret_pr->freshly_copied = 1;

    return true;
}
}
}
