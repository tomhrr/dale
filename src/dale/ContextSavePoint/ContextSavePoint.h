#ifndef DALE_CONTEXT_SAVEPOINT
#define DALE_CONTEXT_SAVEPOINT

#include "../Context/Context.h"
#include "../NamespaceSavePoint/NamespaceSavePoint.h"

#include <vector>
#include <string>
#include <map>

namespace dale
{
struct SPNode
{
    NamespaceSavePoint *nsp;
    std::map<std::string, SPNode *> children;
};
class ContextSavePoint
{
public:
    ContextSavePoint(Context *ctx);
    ~ContextSavePoint();
    bool restore(void);

private:
    SPNode *nsps;
    int active_count;
    int used_count;
    Context *src_ctx;
};
}

#endif
