#ifndef DALE_ELEMENT_TYPE
#define DALE_ELEMENT_TYPE

#include "../BaseType/BaseType.h"
#include "../Node/Node.h"
#include "../Linkage/Linkage.h"
#include "../TypeMap/TypeMap.h"

#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#define IGNORE_ARG_CONSTNESS 1

namespace dale
{
/*! Type

    A class for storing the details of a type.  (This should probably
    be an abstract base class with subclasses for each 'type' of
    type.)

    Putting aside function types, there should generally only be one
    instance for a given type.  See TypeRegister.
*/
class Type
{
public:
    /*! The base type (see BaseType) of the type. */
    int base_type;

    /*! Whether the type is an array. */
    bool is_array;
    /*! The array element type, for an array type. */
    Type *array_type;
    /*! The size of the array. */
    int64_t array_size;

    /*! The size of the bitfield.  If non-zero, implies that the type
     *  is a bitfield type. */
    int bitfield_size;

    /*! Whether the type is a constant type. */
    bool is_const;
    /*! Whether the type is a reference type. */
    bool is_reference;
    /*! Whether the type is a retval type. */
    bool is_retval;

    /*! For a struct type, the name (unqualified, unmangled) of the
     *  struct. */
    std::string struct_name;
    /*! For a struct type, the namespaces of the struct. */
    std::vector<std::string> namespaces;

    /*! For a pointer type, the type to which this points. */
    Type *points_to;

    /*! Whether the type is a function type. */
    bool is_function;
    /*! For a function type, the return type. */
    Type *return_type;
    /*! For a function type, the parameter types. */
    std::vector<Type*> parameter_types;

    Type();
    /*! Construct a new type with the given base type.
     *  @param base_type The new base type.
     */
    Type(int base_type);
    /*! Construct a new pointer type with the given type.
     *  @param points_to The type to which the new type will point.
     */
    Type(Type *points_to);
    ~Type();

    /*! Reset all fields of the type.
     */
    void reset(void);
    /*! Check whether a type is equal to another type.
     *  @param other type The other type.
     *  @param ignore_arg_constness
     *
     *  ignore_arg_constness defaults to false.  If it is true, then
     *  the fact that the current type is not const while the
     *  other_type is will not cause the types to be considered
     *  unequal.
     */
    bool isEqualTo(Type *other_type,
                   bool ignore_arg_constness = false);
    /*! Check whether a variable with this type can be set based on a
     *  value of another type.
     *  @param other type The other type.
     *  @param ignore_arg_constness
     *
     *  ignore_arg_constness has the same semantics as in isEqualTo,
     *  except that it defaults to true.
     */
    bool canBeSetFrom(Type *other_type,
                      bool ignore_arg_constness = true);
    /*! Check whether a parameter with this type can be set based on a
     *  value of another type.
     *  @param other type The other type.
     *  @param ignore_arg_constness
     *
     *  ignore_arg_constness has the same semantics as in
     *  canBeSetFrom.
     */
    bool canBePassedFrom(Type *other_type,
                         bool ignore_arg_constness = true);
    /*! Convert a type into a symbol string.
     *  @param to The buffer for the symbol string.
     *
     *  Type symbol strings are used mainly for function name
     *  mangling.  See Namespace.
     */
    void toSymbolString(std::string *to);

    /*! Make a deep copy of the type.
     *
     *  TypeRegister should be the only client for this method.
     */
    Type *makeCopy(void);

    bool isIntegerType(void);
    bool isSignedIntegerType(void);
    int getIntegerSize(void);
    int getFPRelativeSize(void);
    bool isFloatingPointType(void);
    void toStringProper(std::string *str);
    bool isVarArgs(void);
    unsigned int numberOfRequiredArgs(void);
    Node *toNode(void);
};
}

#endif
