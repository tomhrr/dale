#ifndef DALE_NATIVETYPES
#define DALE_NATIVETYPES

#include "llvm/LinkAllVMCore.h"

namespace dale
{
class NativeTypes
{
private:
    llvm::IntegerType *native_char_type;
    llvm::IntegerType *native_int_type;
    llvm::IntegerType *native_uint_type;
    llvm::IntegerType *native_intptr_type;
    llvm::IntegerType *native_size_type;
    llvm::IntegerType *native_ptrdiff_type;
    llvm::Type *native_float_type;
    llvm::Type *native_double_type;
    llvm::Type *native_longdouble_type;
    int native_ptr_size;
    int native_int_size;
    int native_size_size;
    int native_ptrdiff_size;
public:
    NativeTypes();
    llvm::IntegerType *getNativeIntType(void);
    llvm::IntegerType *getNativeUIntType(void);
    llvm::IntegerType *getNativeCharType(void);
    llvm::IntegerType *getNativeIntptrType(void);
    llvm::IntegerType *getNativePtrDiffType(void);
    llvm::Type        *getNativeFloatType(void);
    llvm::Type        *getNativeDoubleType(void);
    llvm::Type        *getNativeLongDoubleType(void);
    llvm::IntegerType *getNativeSizeType(void);
    int                      getNativeIntSize(void);
    int                      getNativePtrSize(void);
    int                      getNativeSizeSize(void);
    int                      getNativePtrDiffSize(void);
};
}

#endif
