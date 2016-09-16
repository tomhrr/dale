#include "Sizeof.h"

#include "llvm/Support/Debug.h"

namespace dale
{
namespace Operation
{
size_t
SizeofGet(Unit *unit,
          Type *type)
{
    Context *ctx = unit->ctx;
    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    return unit->module->getDataLayout()->getTypeSizeInBits(llvm_type) / 8;
}
}
}
