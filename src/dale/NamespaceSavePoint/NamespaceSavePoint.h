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
    int variable_count;
    int struct_count;
    int enum_count;
    Namespace *src_ns;
};
}

#endif
