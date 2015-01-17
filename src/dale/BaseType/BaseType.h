#ifndef DALE_TYPE
#define DALE_TYPE

#define BASIC_TYPE_COUNT 23

namespace dale
{
/*! BaseType

    Contains the base type enums.  Base types are generally primitive
    types, though there are definitions for Void and VarArgs here as
    well.
*/
namespace BaseType
{
enum
{
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
}

#endif
