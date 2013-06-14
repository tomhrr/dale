#ifndef DALE_NAMESPACE_SAVEPOINT
#define DALE_NAMESPACE_SAVEPOINT

#include "../Namespace/Namespace.h"

#include <vector>
#include <string>
#include <map>

namespace dale
{
class NamespaceSavePoint
{
public:
    NamespaceSavePoint(Namespace *ns);
    ~NamespaceSavePoint(void);
    bool restore(void);

private:
    std::map<std::string, int> function_count;
    std::set<std::string> variables;
    std::set<std::string> structs;
    std::set<std::string> enums;
    Namespace *src_ns;
};
}

#endif
