#ifndef DALE_TYPEREGISTER
#define DALE_TYPEREGISTER

#include "../Type/Type.h"
#include "../Element/Type/Type.h"

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

public:
    /*! The standard constructor. Initialises the basic types. */
    TypeRegister();
    ~TypeRegister();
    
    /*! Return an instance of a basic type.
     *  @param type The basic type enum (see dale::Type). */
    Element::Type *getBasicType(int type);
};
}

#endif
