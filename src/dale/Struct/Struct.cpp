#include "Struct.h"

#include <map>
#include <string>
#include <utility>

#include "../STL/STL.h"

namespace dale {
Struct::Struct() {
    linkage = 0;
    is_opaque = false;
    serialise = true;
}

Struct::~Struct() {}

bool Struct::addMember(const char *name, Type *type) {
    if (nameToType(name)) {
        return false;
    }

    name_to_index.insert(
        std::pair<std::string, int>(name, member_types.size()));

    member_types.push_back(type);

    return true;
}

Type *Struct::nameToType(const char *name) {
    int index = nameToIndex(name);

    return (index == -1) ? NULL : member_types[index];
}

Type *Struct::indexToType(int index) { return member_types[index]; }

int Struct::nameToIndex(const char *name) {
    std::map<std::string, int>::iterator iter =
        name_to_index.find(name);

    if (iter == name_to_index.end()) {
        return -1;
    } else {
        return iter->second;
    }
}

const char *Struct::indexToName(int index) {
    std::map<std::string, int>::iterator iter;

    iter = name_to_index.begin();
    while (iter != name_to_index.end()) {
        if (iter->second == index) {
            return iter->first.c_str();
        }
        ++iter;
    }

    return NULL;
}
}
