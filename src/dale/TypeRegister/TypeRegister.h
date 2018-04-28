#ifndef DALE_TYPEREGISTER
#define DALE_TYPEREGISTER

#include <map>
#include <string>

#include "../BaseType/BaseType.h"
#include "../Type/Type.h"

namespace dale {
/*! TypeRegister

    A class for generating and caching types.

    All returned Types should be treated as const. This will
    be fixed properly later.
*/
class TypeRegister {
    private:
    /*! An array containing instances of all the basic types. */
    Type *basic_types[BASIC_TYPE_COUNT];
    /*! A map from type to pointer type. */
    std::map<Type *, Type *> pointer_types;
    /*! A map from type to const type. */
    std::map<Type *, Type *> const_types;
    /*! A map from type to reference type. */
    std::map<Type *, Type *> reference_types;
    /*! A map from type to rvalue reference type. */
    std::map<Type *, Type *> rvalue_reference_types;
    /*! A map from type to retval type. */
    std::map<Type *, Type *> retval_types;
    /*! A map from type, to size, to array type. */
    std::map<Type *, std::map<size_t, Type *> > array_types;
    /*! A map from type, to size, to bitfield type. */
    std::map<Type *, std::map<size_t, Type *> > bitfield_types;
    /*! A map from fully-qualified struct name to struct type. */
    std::map<std::string, Type *> struct_types;

    public:
    /*! The standard constructor. Initialises the basic types. */
    TypeRegister();
    ~TypeRegister();

    /*! Return an instance of a basic type.
     *  @param type The basic type enum (see BaseType). */
    Type *getBasicType(int type);
    /*! Return an instance of a pointer to a type.
     *  @param type The pointee type object. */
    Type *getPointerType(Type *type);
    /*! Return an instance of a const type.
     *  @param type The type to 'make const'. */
    Type *getConstType(Type *type);
    /*! Return an instance of a reference type.
     *  @param type The type to make into a reference type. */
    Type *getReferenceType(Type *type);
    /*! Return an instance of an rvalue reference type.
     *  @param type The type to make into an rvalue reference type. */
    Type *getRvalueReferenceType(Type *type);
    /*! Return an instance of a retval type.
     *  @param type The type to make into a retval type. */
    Type *getRetvalType(Type *type);
    /*! Return an instance of an array type.
     *  @param type The array element type.
     *  @param size The size of the array. */
    Type *getArrayType(Type *type, size_t size);
    /*! Return an instance of a bitfield type.
     *  @param type The underlying integer type.
     *  @param size The size of the bitfield. */
    Type *getBitfieldType(Type *type, size_t size);
    /*! Return an instance of a struct type.
     *  @param name The fully-qualified name of the struct. */
    Type *getStructType(const char *name);

    /*! Takes a type, and returns a previously-generated type object,
     *  if possible. Otherwise, stores the type in the appropriate
     *  place and returns it. The argument type continues to be owned
     *  by the caller after this function has been executed.
     *  @param type The type. */
    Type *getType(Type *type);

    /*! Variables for common types. */
    Type *type_bool;
    Type *type_void;
    Type *type_varargs;
    Type *type_int;
    Type *type_intptr;
    Type *type_size;
    Type *type_ptrdiff;
    Type *type_uint;
    Type *type_char;
    Type *type_float;
    Type *type_double;
    Type *type_longdouble;
    Type *type_int8;
    Type *type_uint8;
    Type *type_int16;
    Type *type_uint16;
    Type *type_int32;
    Type *type_uint32;
    Type *type_int64;
    Type *type_uint64;
    Type *type_int128;
    Type *type_uint128;
    Type *type_pchar;
    Type *type_pvoid;
    Type *type_dnode;
    Type *type_pdnode;

    /*! Dump debugging information to stderr. */
    void print();
};
}

#endif
