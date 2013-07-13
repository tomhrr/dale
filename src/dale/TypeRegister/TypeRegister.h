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
};
}

#endif
