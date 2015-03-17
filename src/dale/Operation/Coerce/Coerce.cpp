#include "Coerce.h"

namespace dale
{
namespace Operation
{
bool
Coerce(Context *ctx, llvm::BasicBlock *block, llvm::Value *value,
       Type *from_type, Type *to_type, ParseResult *pr)
{
    bool from_is_array = from_type->is_array;
    int from_array_base_type =
        (from_is_array) ? from_type->array_type->base_type : 0;
    Type *from_points = from_type->points_to;
    Type *to_points = to_type->points_to;

    if (from_array_base_type == BaseType::Char
            && from_is_array
            && !from_points) {
        if (to_points
                && to_points->base_type == BaseType::Char
                && !to_points->points_to) {
            llvm::IRBuilder<> builder(block);

            llvm::Value *charpointer =
                builder.CreateGEP(
                    llvm::cast<llvm::Value>(value),
                    ctx->nt->getTwoLLVMZeros()
                );

            pr->set(block, to_type, charpointer);
            return true;
        }
    }

    return false;
}
}
}
