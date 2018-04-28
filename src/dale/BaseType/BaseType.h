#ifndef DALE_TYPE
#define DALE_TYPE

#define BASIC_TYPE_COUNT 23

namespace dale {
/*! BaseType

    Contains the base type enums and a few functions for dealing with
    them.  Base types are generally primitive types, though there are
    definitions for Void and VarArgs here as well.
*/
namespace BaseType {
enum {
    /*! The null type. */
    Null,
    /*! The void type. */
    Void,
    /*! The boolean type. */
    Bool,
    /*! The character type. */
    Char,
    /*! The VarArgs type.  Internally, this is the last 'parameter' of
     *  a varargs function. */
    VarArgs,
    /*! The integer type. */
    Int,
    /*! The unsigned integer type. */
    UInt,
    /*! The 8-bit signed integer type. */
    Int8,
    /*! The 8-bit unsigned integer type. */
    UInt8,
    /*! The 16-bit signed integer type. */
    Int16,
    /*! The 16-bit unsigned integer type. */
    UInt16,
    /*! The 32-bit signed integer type. */
    Int32,
    /*! The 32-bit unsigned integer type. */
    UInt32,
    /*! The 64-bit signed integer type. */
    Int64,
    /*! The 64-bit unsigned integer type. */
    UInt64,
    /*! The 128-bit signed integer type. */
    Int128,
    /*! The 128-bit unsigned integer type. */
    UInt128,
    /*! The single-precision floating point type. */
    Float,
    /*! The double-precision floating point type. */
    Double,
    /*! The 'long double' floating point type (as per the system's C
     *  compiler. */
    LongDouble,
    /*! The size type (as per size_t). */
    Size,
    /*! The pointer difference type (as per ptrdiff_t). */
    PtrDiff,
    /*! The integer pointer type (as per intptr_t). */
    IntPtr
};
}

/*! Get the signed integer base type for the given size.
 *  @param size The size of the type in bits.
 *
 *  If the size is not one with an associated type, Int is returned.
 */
int getSignedIntegerType(int size);
/*! Get the unsigned integer base type for the given size.
 *  @param size The size of the type in bits.
 *
 *  If the size is not one with an associated type, UInt is returned.
 */
int getUnsignedIntegerType(int size);
/*! Get the base type as a string.
 *  @param base_type The base type.
 *
 *  If the base type is invalid, the string "[unknown]" will be
 *  returned.
 */
const char *baseTypeToString(int base_type);
/*! Get the base type from a string.
 *  @param str The base type as a string.
 *
 *  If the string does not represent a base type, the value -1 will be
 *  returned.
 */
int stringToBaseType(const char *str);
/*! Get the base type as a symbol character.
 *  @param base_type The base type.
 *
 *  If the base type is invalid, the character '?' will be returned.
 */
char baseTypeToSymbolChar(int base_type);
/*! Check whether the base type is an integer type.
 *  @param base_type The base type.
 *
 *  The core integer types (Int and UInt), their
 *  specifically-sized variations, Char, IntPtr, Size and PtrDiff
 *  are considered integer types.
 */
bool baseTypeIsIntegerType(int base_type);
/*! Check whether the base type is a signed integer type.
 *  @param base_type The base type.
 *
 *  Int, its specifically-sized variations and Char
 *  are considered signed integer types.
 */
bool baseTypeIsSignedIntegerType(int base_type);
/*! Check whether the base type is a floating point type.
 *  @param base_type The base type.
 */
bool baseTypeIsFloatingPointType(int base_type);
/*! Get the size (in bits) of a base integer type.
 *  @param base_type The base type.
 *
 *  If the result is 1, then the size is the platform's native integer
 *  size.  If the result is 2, then the size is the platform's native
 *  pointer size.  If the result is 3, then the size is that of
 *  size_t.  If the result is 4, then the size is that of ptrdiff_t.
 *  Otherwise, the result is the number of bits required by the type.
 */
int integerTypeToSize(int base_type);
/*! Get the relative size of a base floating point type.
 *  @param base_type The base type.
 *
 *  The possible results are 1, 2 and 3, which denote Float, Double
 *  and LongDouble respectively.
 */
int floatingPointTypeToRelativeSize(int base_type);
}

#endif
