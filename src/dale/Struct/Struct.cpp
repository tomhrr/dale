#include "Struct.h"

#include "../STLUtils/STLUtils.h"

namespace dale
{
Struct::Struct()
{
    is_opaque = 0;
    linkage   = 0;
    must_init = 0;
    serialise = true;
}

Struct::~Struct()
{
}

int Struct::addElement(const char *name, Type *type)
{
    /* Name already exists, return 0 and let caller deal with
     * it. */
    if (nameToType(name)) {
        return 0;
    }

    std::string temp_name(name);

    names_to_numbers.insert(
        std::pair<std::string, int>(
            temp_name, element_types.size()
        )
    );

    element_types.push_back(type);

    return 1;
}

Type *Struct::nameToType(const char *name)
{
    int index = nameToIndex(name);

    return
        (index == -1)
        ? NULL
        : element_types[index];
}

Type *Struct::indexToType(int index)
{
    return element_types[index];
}

int Struct::nameToIndex(const char *name)
{
    std::map<std::string, int>::iterator iter;
    std::string temp_name(name);

    iter = names_to_numbers.find(temp_name);

    if (iter == names_to_numbers.end()) {
        return -1;
    } else {
        return iter->second;
    }
}

const char *Struct::indexToName(int index)
{
    std::map<std::string, int>::iterator iter;

    iter = names_to_numbers.begin();
    while (iter != names_to_numbers.end()) {
        if (iter->second == index) {
            return iter->first.c_str();
        }
        ++iter;
    }

    return NULL;
}
}
