#ifndef DALE_NATIVETYPES
#define DALE_NATIVETYPES

#include <vector>

#include "../llvm_LinkAll.h"

namespace dale {
/*! NativeTypes

    Provides methods for getting the LLVM types that correspond to the
    native types of the current platform.  The get*Type methods are
    self-explanatory, and the get*Size methods return the number of
    bits required by the size.  The other methods are documented below.
*/
class NativeTypes {
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
    std::vector<llvm::Value *> zeros;

    public:
    NativeTypes();
    llvm::IntegerType *getNativeIntType();
    llvm::IntegerType *getNativeUIntType();
    llvm::IntegerType *getNativeCharType();
    llvm::IntegerType *getNativeIntptrType();
    llvm::IntegerType *getNativePtrDiffType();
    llvm::Type *getNativeFloatType();
    llvm::Type *getNativeDoubleType();
    llvm::Type *getNativeLongDoubleType();
    llvm::IntegerType *getNativeSizeType();
    int getNativeIntSize();
    int getNativePtrSize();
    int getNativeSizeSize();
    int getNativePtrDiffSize();
    /*! Convert an internal size to a real size.
     *  @param size An internal size.
     *
     *  Internal sizes are returned by Type::getIntegerSize and
     *  integerTypeToSize.
     */
    int internalSizeToRealSize(int size);
    /*! Get an LLVM true boolean value.
     */
    llvm::Value *getLLVMTrue();
    /*! Get an LLVM false boolean value.
     */
    llvm::Value *getLLVMFalse();
    /*! Get an LLVM native integer value of zero.
     */
    llvm::Value *getLLVMZero();
    /*! Get an array of two LLVM zeros.
     */
    llvm::ArrayRef<llvm::Value *> getTwoLLVMZeros();
    /*! Get an LLVM native integer value of one.
     */
    llvm::Value *getLLVMOne();
    /*! Get an LLVM native integer value for the given number.
     *  @param n The number.
     */
    llvm::ConstantInt *getNativeInt(int n);
    /*! Get an LLVM constant integer value for the given type and
     * number.
     *  @param type The type.
     *  @param nstr The number, as a string.
     *
     *  This handles hexadecimal numbers as well as normal decimal
     *  numbers.  Hexadecimal numbers must be prefixed with "0x".
     */
    llvm::ConstantInt *getConstantInt(llvm::IntegerType *type,
                                      const char *nstr);
};
}

#endif
