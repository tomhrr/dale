#include "NativeTypes.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

#define BITS(T) (CHAR_BIT * sizeof(T))

namespace dale
{
NativeTypes::NativeTypes()
{
    llvm::LLVMContext &lc = llvm::getGlobalContext();

    llvm::Type *native_bool_type =
        llvm::IntegerType::get(lc, 1);
    native_char_type    = llvm::IntegerType::get(lc, BITS(char));
    native_int_type     = 
    native_uint_type    = llvm::IntegerType::get(lc, BITS(int));
    native_intptr_type  = llvm::IntegerType::get(lc, BITS(char *));
    native_ptrdiff_type = llvm::IntegerType::get(lc, BITS(ptrdiff_t));
    native_size_type    = llvm::IntegerType::get(lc, BITS(size_t)),
    
    native_float_type      = llvm::Type::getFloatTy(lc);
    native_double_type     = llvm::Type::getDoubleTy(lc);
    native_longdouble_type = llvm::Type::getX86_FP80Ty(lc);

    native_ptr_size     = CHAR_BIT * sizeof(char *);
    native_int_size     = CHAR_BIT * sizeof(int);
    native_size_size    = CHAR_BIT * sizeof(size_t);
    native_ptrdiff_size = CHAR_BIT * sizeof(ptrdiff_t);

    llvm_false = llvm::ConstantInt::get(native_bool_type, 0);
    llvm_true  = llvm::ConstantInt::get(native_bool_type, 1);
    llvm_zero  = llvm::ConstantInt::get(native_int_type,  0);
    llvm_one   = llvm::ConstantInt::get(native_int_type,  1);
}

llvm::IntegerType *NativeTypes::getNativeIntType(void)
{
    return native_int_type;
}

llvm::IntegerType *NativeTypes::getNativeUIntType(void)
{
    return native_uint_type;
}

llvm::IntegerType *NativeTypes::getNativeCharType(void)
{
    return native_char_type;
}

llvm::IntegerType *NativeTypes::getNativeIntptrType(void)
{
    return native_intptr_type;
}

llvm::IntegerType *NativeTypes::getNativePtrDiffType(void)
{
    return native_ptrdiff_type;
}

llvm::Type *NativeTypes::getNativeFloatType(void)
{
    return native_float_type;
}

llvm::Type *NativeTypes::getNativeDoubleType(void)
{
    return native_double_type;
}

llvm::Type *NativeTypes::getNativeLongDoubleType(void)
{
    return native_longdouble_type;
}

llvm::IntegerType *NativeTypes::getNativeSizeType(void)
{
    return native_size_type;
}

int NativeTypes::getNativePtrSize(void)
{
    return native_ptr_size;
}

int NativeTypes::getNativeIntSize(void)
{
    return native_int_size;
}

int NativeTypes::getNativePtrDiffSize(void)
{
    return native_ptrdiff_size;
}

int NativeTypes::getNativeSizeSize(void)
{
    return native_size_size;
}

llvm::Value *NativeTypes::getLLVMTrue(void)
{
    return llvm_true;
}

llvm::Value *NativeTypes::getLLVMFalse(void)
{
    return llvm_false;
}

llvm::Value *NativeTypes::getLLVMZero(void)
{
    return llvm_zero;
}

llvm::Value *NativeTypes::getLLVMOne(void)
{
    return llvm_one;
}

llvm::ConstantInt *NativeTypes::getNativeInt(int n)
{
    return llvm::ConstantInt::get(native_int_type, n);
}

llvm::ConstantInt *NativeTypes::getConstantInt(llvm::IntegerType *type,
                                               const char *numstr) 
{
    int len = strlen(numstr);
    int radix = 10;
    if ((len >= 3) && (numstr[0] == '0') && (numstr[1] == 'x')) {
        numstr += 2;
        radix = 16;
    }

    return llvm::ConstantInt::get(type,
                                  llvm::StringRef(numstr),
                                  radix);
}

int NativeTypes::internalSizeToRealSize(int n)
{
    if (n > 1000) {
        return n - 1000;
    }

    switch (n) {
    case 1:
        return getNativeIntSize();
    case 2:
        return getNativePtrSize();
    case 3:
        return getNativeSizeSize();
    case 4:
        return getNativePtrDiffSize();
    default:
        return n;
    }
}
}
