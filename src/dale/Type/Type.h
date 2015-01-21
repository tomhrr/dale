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
/*

Will have a series of constructors later, or some other more
high-level way of making types.

The 'core' type of the variable.  The array size of the variable (if
it is an array).  The reason for having is_array and array_size is
that an array size of 0 indicates an array with no predefined size (as
in LLVM), so it can't be used to indicate 'not an array'.  Linkage
applies only to 'chief' types.  If this is a pointer, contains the
type to which it points.

*/
int getSignedIntegerType(int size);
int getUnsignedIntegerType(int size);

class Type
{
public:
    int            base_type;
    int            is_array;
    int64_t        array_size;
    Type  *array_type;
    int            is_function;
    int            bitfield_size;
    int            is_const;
    int            is_reference;
    std::string    *struct_name;
    std::vector<std::string> *namespaces;
    int            is_retval;

    Type     *points_to;

    Type        *return_type;
    std::vector<Type*> *parameter_types;

    Type();
    Type(int new_base_type,
         int new_is_array = 0,
         int new_array_size = 0);
    Type(Type *new_points_to,
         int new_is_array = 0,
         int new_array_size = 0);

    ~Type();

    bool isEqualTo(Type *other_type,
                  int ignore_arg_constness = 0);
    bool canBeSetFrom(Type *other_type,
                      bool ignore_arg_constness = true);
    bool canBePassedFrom(Type *other_type,
                         bool ignore_arg_constness = true);
    void toEncStr(std::string *newstr);

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

const char *baseTypeToString(int base_type);
}

#endif
