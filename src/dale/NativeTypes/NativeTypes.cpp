#include "NativeTypes.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace dale
{
NativeTypes::NativeTypes()
{
    llvm::LLVMContext &lc = llvm::getGlobalContext();

    native_char_type =
        llvm::IntegerType::get(lc, CHAR_BIT);
    native_int_type = native_uint_type =
                          llvm::IntegerType::get(lc, CHAR_BIT * sizeof(int));
    native_intptr_type =
        llvm::IntegerType::get(lc, CHAR_BIT * sizeof(char *));
    native_ptrdiff_type =
        llvm::IntegerType::get(lc, CHAR_BIT * sizeof(ptrdiff_t));
    native_size_type =
        llvm::IntegerType::get(lc, CHAR_BIT * sizeof(size_t)),
        native_float_type =
            llvm::Type::getFloatTy(lc);
    native_double_type =
        llvm::Type::getDoubleTy(lc);
    native_longdouble_type =
        llvm::Type::getX86_FP80Ty(lc);

    native_ptr_size     = CHAR_BIT * sizeof(char *);
    native_int_size     = CHAR_BIT * sizeof(int);
    native_size_size    = CHAR_BIT * sizeof(size_t);
    native_ptrdiff_size = CHAR_BIT * sizeof(ptrdiff_t);
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
}
