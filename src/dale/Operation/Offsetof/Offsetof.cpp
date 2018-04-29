#include "Offsetof.h"

#include <vector>

#include <cstdio>

namespace dale {
namespace Operation {
static int nameToIndex(Context *ctx, Type *type,
                       const char *member_name) {
    Struct *st = ctx->getStruct(type);
    assert(st);

    int index = st->nameToIndex(member_name);
    assert(index != -1);

    return index;
}

uint64_t OffsetofGet(Unit *unit, Type *type, int index) {
    Context *ctx = unit->ctx;
    llvm::Type *llvm_type =
        ctx->toLLVMType(ctx->tr->getPointerType(type), NULL, false);
    if (!llvm_type) {
        return false;
    }
    std::vector<llvm::Value *> indices;
    indices.push_back(ctx->nt->getLLVMZero());
    indices.push_back(ctx->nt->getNativeInt(index));
    llvm::ArrayRef<llvm::Value *> indices_aref(indices);

#if D_LLVM_VERSION_ORD <= 34
    llvm::DataLayout data_layout(unit->module->getDataLayout());
    return data_layout.getIndexedOffset(llvm_type, indices_aref);
#elif D_LLVM_VERSION_ORD <= 36
    return unit->module->getDataLayout()->getIndexedOffset(
        llvm_type, indices_aref);
#elif D_LLVM_VERSION_ORD <= 38
    return unit->module->getDataLayout().getIndexedOffset(llvm_type,
                                                          indices_aref);
#else
    llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return false;
    }
    return unit->module->getDataLayout().getIndexedOffsetInType(
        llvm_type, indices_aref);

#endif
}

bool Offsetof(Unit *unit, llvm::BasicBlock *block, Type *type,
              const char *member_name, ParseResult *pr) {
    size_t size = OffsetofGet(
        unit, type, nameToIndex(unit->ctx, type, member_name));
    pr->set(block, unit->ctx->tr->type_size,
            llvm::ConstantInt::get(
                unit->ctx->toLLVMType(unit->ctx->tr->type_size, NULL,
                                      false),
                size));
    return true;
}

size_t OffsetofGetByIndex(Unit *unit, Type *type, int index) {
    return OffsetofGet(unit, type, index);
}
}
}
