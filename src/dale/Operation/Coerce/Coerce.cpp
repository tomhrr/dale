#include "Coerce.h"

namespace dale
{
namespace Operation
{
bool
Coerce(Context *ctx, llvm::BasicBlock *block, llvm::Value *value,
       Type *from_type, Type *to_type, ParseResult *pr)
{
    bool fa = from_type->is_array;
    int fb = (fa) ? from_type->array_type->base_type : 0;
    Type *fp = from_type->points_to;
    Type *tp = to_type->points_to;

    std::vector<llvm::Value *> two_zero_indices;
    STL::push_back2(&two_zero_indices,
                    ctx->nt->getLLVMZero(), ctx->nt->getLLVMZero());

    if (fb == BaseType::Char && fa && !fp) {
        if (tp && tp->base_type == BaseType::Char && !tp->points_to) {
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
