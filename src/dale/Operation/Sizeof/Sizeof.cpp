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
#if D_LLVM_VERSION_MINOR <= 4
    llvm::DataLayout data_layout(unit->module->getDataLayout());
    return data_layout.getTypeSizeInBits(llvm_type) / 8;
#else
    return unit->module->getDataLayout()->getTypeSizeInBits(llvm_type) / 8;
#endif
}
}
}
