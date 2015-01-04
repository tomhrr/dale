#ifndef DALE_ELEMENT_TYPE
#define DALE_ELEMENT_TYPE

#include "../../Type/Type.h"
#include "../../Node/Node.h"
#include "../../Linkage/Linkage.h"
#include <cstddef>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <map>

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

#define IGNORE_ARG_CONSTNESS 1

namespace dale
{
extern std::map<std::string, std::string> dale_typemap;

int getSignedIntegerType(int size);
int getUnsignedIntegerType(int size);

int addTypeMapEntry(const char *from, const char *to);
int getTypeMapEntry(const char *from, std::string *to);

namespace Element
{
class Type
{
public:
    int            base_type;
    int            is_array;
    int64_t        array_size;
    Element::Type  *array_type;
    int            is_function;
    int            bitfield_size;
    int            is_const;
    int            is_reference;
    std::string    *struct_name;
    std::vector<std::string> *namespaces;
    int            is_retval;

    Element::Type     *points_to;

    Element::Type        *return_type;
    std::vector<Element::Type*> *parameter_types;

    Type();
    Type(int new_base_type,
         int new_is_array = 0,
         int new_array_size = 0);
    Type(Element::Type *new_points_to,
         int new_is_array = 0,
         int new_array_size = 0);

    ~Type();

    bool isEqualTo(Element::Type *other_type,
                  int ignore_arg_constness = 0);
    bool canBeSetFrom(Element::Type *other_type,
                      bool ignore_arg_constness = true);
    bool canBePassedFrom(Element::Type *other_type,
                         bool ignore_arg_constness = true);
    void toEncStr(std::string *newstr);

    Element::Type *makeCopy(void);

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
}

#endif
