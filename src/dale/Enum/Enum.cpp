#include "Enum.h"

namespace dale
{
Enum::Enum()
{
    last_index = -1;
    type = NULL;
    linkage = 0;
    serialise = true;
}

Enum::~Enum()
{
}

bool
Enum::existsName(const char *name)
{
    std::map<std::string, int64_t>::iterator iter;
    iter = name_to_index.find(name);
    return (iter != name_to_index.end());
}

int64_t
Enum::nameToIndex(const char *name)
{
    std::map<std::string, int64_t>::iterator iter;
    iter = name_to_index.find(name);
    return iter->second;
}

int
Enum::addMember(const char *name, int64_t number)
{
    if (existsName(name)) {
        return 0;
    }

    name_to_index.insert(std::pair<std::string, int64_t>(name, number));
    last_index = number;

    return 1;
}

int
Enum::addMember(const char *name)
{
    return addMember(name, ++last_index);
}
}
