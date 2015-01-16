#ifndef DALE_ELEMENT_ENUM
#define DALE_ELEMENT_ENUM

#include <string>
#include <map>

#include "../../llvm_Module.h"

#define ENUM_NOTFOUND -1000000

namespace dale
{
namespace Element
{
/*! Enum

    A class for storing the details of an enumerated type.  Element
    indices begin at zero.
*/
class Enum
{
public:
    /* The enumerated type's integral type. */
    llvm::Type *type;
    /* The index of the last enumerated element. */
    int last_index;
    /* A map from enumerated element name to index. */
    std::map<std::string, int64_t> *name_to_index;
    /* The once tag of this type. */
    std::string once_tag;
    /* The linkage of this type. */
    int linkage;
    /* Whether the type should be serialised. */
    bool serialise;

    Enum();
    ~Enum();

    /*! Add a new element to the enumerated type.
     *  @param name The name for the new element.
     */
    int addElement(const char *name);
    /*! Add a new element to the enumerated type at a specific index.
     *  @param name The name of the new element.
     *  @param index The index of the new element.
     */
    int addElement(const char *name, int64_t index);
    /*! Retrieve the index of the element with the given name.
     *  @param name The name of the element.
     */
    int64_t nameToIndex(const char *name);
};
}
}

#endif
