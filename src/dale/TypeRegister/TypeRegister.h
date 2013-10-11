#ifndef DALE_TYPEREGISTER
#define DALE_TYPEREGISTER

#include "../Type/Type.h"
#include "../Element/Type/Type.h"

#include <map>

namespace dale
{

/*! TypeRegister

    A class for generating and caching types.

    All returned Element::Types should be treated as const. This will
    be fixed properly later.
*/

class TypeRegister
{
private:
    /*! An array containing instances of all the basic types. */
    Element::Type *basic_types[BASIC_TYPE_COUNT];
    /*! A map from type to pointer type. */
    std::map<Element::Type*, Element::Type*> pointer_types;
    /*! A map from type to const type. */
    std::map<Element::Type*, Element::Type*> const_types;
    /*! A map from type, to size, to array type. */
    std::map<Element::Type*, std::map<size_t, Element::Type*> > array_types;
    /*! A map from type, to size, to bitfield type. */
    std::map<Element::Type*, std::map<size_t, Element::Type*> > bitfield_types;
    /*! A map from fully-qualified struct name to struct type. */
    std::map<std::string, Element::Type*> struct_types;

public:
    /*! The standard constructor. Initialises the basic types. */
    TypeRegister();
    ~TypeRegister();
    
    /*! Return an instance of a basic type.
     *  @param type The basic type enum (see dale::Type). */
    Element::Type *getBasicType(int type);
    /*! Return an instance of a pointer to a type.
     *  @param type The pointee type object. */
    Element::Type *getPointerType(Element::Type *type);
    /*! Return an instance of a const type.
     *  @param type The type to 'make const'. */
    Element::Type *getConstType(Element::Type *type);
    /*! Return an instance of an array type.
     *  @param type The array element type.
     *  @param size The size of the array. */
    Element::Type *getArrayType(Element::Type *type, size_t size);
    /*! Return an instance of a bitfield type.
     *  @param type The underlying integer type.
     *  @param size The size of the bitfield. */
    Element::Type *getBitfieldType(Element::Type *type, size_t size);
    /*! Return an instance of a struct type.
     *  @param name The fully-qualified name of the struct. */
    Element::Type *getStructType(std::string name);

    /*! Takes a type, and returns a previously-generated type object,
     *  if possible. Otherwise, stores the type in the appropriate
     *  place and returns it. The argument type continues to be owned
     *  by the caller after this function has been executed.
     *  @param type The type. */
    Element::Type *getType(Element::Type *type);
    /*! Variables for common types. */
    Element::Type *type_bool;
    Element::Type *type_void;
    Element::Type *type_varargs;
    Element::Type *type_int;
    Element::Type *type_intptr;
    Element::Type *type_size;
    Element::Type *type_ptrdiff;
    Element::Type *type_uint;
    Element::Type *type_char;
    Element::Type *type_float;
    Element::Type *type_double;
    Element::Type *type_longdouble;
    Element::Type *type_int8;
    Element::Type *type_uint8;
    Element::Type *type_int16;
    Element::Type *type_uint16;
    Element::Type *type_int32;
    Element::Type *type_uint32;
    Element::Type *type_int64;
    Element::Type *type_uint64;
    Element::Type *type_int128;
    Element::Type *type_uint128;
    Element::Type *type_pchar;

    /*! Dump debugging information to stderr. */
    void dump(void);
};
}

#endif
