#include "Sizeof.h"

#include "llvm/Support/Debug.h"

namespace dale {
namespace Operation {
size_t SizeofGet(Unit *unit, Type *type) {
    Context *ctx = unit->ctx;
    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        std::string type_str;
        type->toString(&type_str);
        fprintf(stderr, "Unable to get size of type %s\n",
                type_str.c_str());
        abort();
    }
#if D_LLVM_VERSION_ORD <= 34
    llvm::DataLayout data_layout(unit->module->getDataLayout());
    return data_layout.getTypeSizeInBits(llvm_type) / 8;
#elif D_LLVM_VERSION_ORD <= 36
    return unit->module->getDataLayout()->getTypeSizeInBits(llvm_type) /
           8;
#else
    return unit->module->getDataLayout().getTypeSizeInBits(llvm_type) /
           8;
#endif
}
}
}
