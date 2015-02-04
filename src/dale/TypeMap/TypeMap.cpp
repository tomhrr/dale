#include "TypeMap.h"

#include "../STL/STL.h"
#include "../Utils/Utils.h"

#include <cstdio>
#include <cstring>

namespace dale
{
std::map<std::string, std::string> dale_typemap;

bool
addTypeMapEntry(const char *from, const char *to)
{
    dale_typemap.insert(std::pair<std::string, std::string>(from, to));
    return true;
}

bool
getTypeMapEntry(const char *from, std::string *to)
{
    std::map<std::string, std::string>::iterator iter
        = dale_typemap.find(from);
    if (iter != dale_typemap.end()) {
        to->append(iter->second);
        return true;
    } else {
        return false;
    }
}
}
