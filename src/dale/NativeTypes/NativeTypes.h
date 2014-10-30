#ifndef DALE_NATIVETYPES
#define DALE_NATIVETYPES

#include "../llvm_LinkAll.h"

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
    llvm::Value *llvm_true;
    llvm::Value *llvm_false;
    llvm::Value *llvm_zero;
    llvm::Value *llvm_one;
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
    int getNativeIntSize(void);
    int getNativePtrSize(void);
    int getNativeSizeSize(void);
    int getNativePtrDiffSize(void);
    int internalSizeToRealSize(int size);
    llvm::Value *getLLVMTrue(void);
    llvm::Value *getLLVMFalse(void);
    llvm::Value *getLLVMZero(void);
    llvm::Value *getLLVMOne(void);
    llvm::ConstantInt *getNativeInt(int n);
    llvm::ConstantInt *getConstantInt(llvm::IntegerType *type,
                                      const char *numstr);
};
}

#endif
