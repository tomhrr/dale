#include "Enum.h"

namespace dale
{
Enum::Enum()
{
    last_index = -1;
    type = NULL;
    member_to_index = new std::map<std::string, int64_t>;
    linkage = 0;
    serialise = true;
}

Enum::~Enum()
{
    delete member_to_index;
}

bool
Enum::existsMember(const char *name)
{
    std::map<std::string, int64_t>::iterator iter;
    iter = member_to_index->find(name);
    return (iter != member_to_index->end());
}

int64_t
Enum::memberToIndex(const char *name)
{
    std::map<std::string, int64_t>::iterator iter;
    iter = member_to_index->find(name);
    return iter->second;
}

int
Enum::addMember(const char *name, int64_t number)
{
    if (existsMember(name)) {
        return 0;
    }

    member_to_index->insert(std::pair<std::string, int64_t>(name, number));
    last_index = number;

    return 1;
}

int
Enum::addMember(const char *name)
{
    return addMember(name, ++last_index);
}
}
