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

    std::vector<llvm::Value *> two_zero_indices;
    STL::push_back2(&two_zero_indices,
                    ctx->nt->getLLVMZero(), ctx->nt->getLLVMZero());

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
                    llvm::ArrayRef<llvm::Value*>(two_zero_indices));

            pr->set(block, to_type, charpointer); 
            return true;
        }
    }

    return false;
}
}
}
