#ifndef DALE_ELEMENT_STRUCT
#define DALE_ELEMENT_STRUCT

#include <map>
#include <string>
#include <vector>

#include "../Linkage/Linkage.h"
#include "../Type/Type.h"
#include "../llvm_Module.h"

namespace dale {
/*! Struct

    A class for storing the details of a struct definition.  Member
    indices begin at zero.
*/
class Struct {
    public:
    /* The struct's LLVM struct type. */
    llvm::StructType *type;
    /* The struct's internal name (mangled, qualified). */
    std::string symbol;
    /* The types of the struct's members. */
    std::vector<Type *> member_types;
    /* A map from member name to index. */
    std::map<std::string, int> name_to_index;
    /* The struct's once tag. */
    std::string once_tag;
    /* The struct's linkage. */
    int linkage;
    /* Whether the struct is opaque.  An opaque struct is one whose
     * members are defined elsewhere. */
    bool is_opaque;
    /* Whether the struct should be serialised. */
    bool serialise;

    Struct();
    ~Struct();

    /*! Add a member to the struct.
     *  @param name The member's name.
     *  @param type The member's type.
     *
     *  Returns false if a member with the given name already exists.
     *  Otherwise, returns true.  name is copied, while type is not.
     */
    bool addMember(const char *name, Type *type);
    /*! Get the type of a given member.
     *  @param name The member's name.
     *
     *  Returns null if no member with the given name exists.
     */
    Type *nameToType(const char *name);
    /*! Get the index of a given member.
     *  @param name The member's name.
     *
     *  Returns -1 if no member with the given name exists.
     */
    int nameToIndex(const char *name);
    /*! Get the type at a given index.
     *  @param index The index.
     *
     *  Returns null if the index is invalid.
     */
    Type *indexToType(int index);
    /*! Get the member at a given index.
     *  @param index The index.
     *
     *  Returns null if no member with the given name exists.
     */
    const char *indexToName(int index);
};
}

#endif
