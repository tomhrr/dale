#include "Enum.h"

namespace dale
{
namespace Element
{
Enum::Enum()
{
    last_index = -1;
    type = NULL;
    names_to_numbers = new std::map<std::string, int64_t>;
    linkage = 0;
}

Enum::~Enum()
{
    delete names_to_numbers;
}

int Enum::addElement(const char *name, int64_t number)
{
    if (nameToNumber(name) != ENUM_NOTFOUND) {
        return 0;
    }

    std::string temp_name(name);

    names_to_numbers->insert(
        std::pair<std::string, int64_t>(
            temp_name, number
        )
    );

    last_index = number;

    return 1;
}

int Enum::addElement(const char *name)
{
    return addElement(name, ++last_index);
}

int64_t Enum::nameToNumber(const char *name)
{
    std::map<std::string, int64_t>::iterator iter;
    std::string temp_name(name);

    iter = names_to_numbers->find(temp_name);

    if (iter == names_to_numbers->end()) {
        return ENUM_NOTFOUND;
    } else {
        return iter->second;
    }
}
}
}
