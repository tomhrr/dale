#include "Struct.h"

#include "../STLUtils/STLUtils.h"

namespace dale
{
Struct::Struct()
{
    linkage   = 0;
    is_opaque = false;
    must_init = false;
    serialise = true;
}

Struct::~Struct()
{
}

bool
Struct::addMember(const char *name, Type *type)
{
    if (memberToType(name)) {
        return false;
    }

    member_to_index.insert(
        std::pair<std::string, int>(name, member_types.size())
    );

    member_types.push_back(type);

    return true;
}

Type *
Struct::memberToType(const char *name)
{
    int index = memberToIndex(name);

    return (index == -1) ? NULL : member_types[index];
}

Type *
Struct::indexToType(int index)
{
    return member_types[index];
}

int
Struct::memberToIndex(const char *name)
{
    std::map<std::string, int>::iterator iter =
        member_to_index.find(name);

    if (iter == member_to_index.end()) {
        return -1;
    } else {
        return iter->second;
    }
}

const char *
Struct::indexToMember(int index)
{
    std::map<std::string, int>::iterator iter;

    iter = member_to_index.begin();
    while (iter != member_to_index.end()) {
        if (iter->second == index) {
            return iter->first.c_str();
        }
        ++iter;
    }

    return NULL;
}
}
