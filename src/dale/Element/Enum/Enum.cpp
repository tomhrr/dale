#include "Enum.h"

namespace dale
{
namespace Element
{
Enum::Enum()
{
    last_index = -1;
    type = NULL;
    name_to_index = new std::map<std::string, int64_t>;
    linkage = 0;
    serialise = true;
}

Enum::~Enum()
{
    delete name_to_index;
}

int
Enum::addElement(const char *name, int64_t number)
{
    if (nameToIndex(name) != ENUM_NOTFOUND) {
        return 0;
    }

    name_to_index->insert(
        std::pair<std::string, int64_t>(
            name, number
        )
    );

    last_index = number;

    return 1;
}

int
Enum::addElement(const char *name)
{
    return addElement(name, ++last_index);
}

int64_t
Enum::nameToIndex(const char *name)
{
    std::map<std::string, int64_t>::iterator iter;
    iter = name_to_index->find(name);

    if (iter == name_to_index->end()) {
        return ENUM_NOTFOUND;
    } else {
        return iter->second;
    }
}
}
}
