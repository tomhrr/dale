#ifndef DALE_CONTEXT_SAVEPOINT
#define DALE_CONTEXT_SAVEPOINT

#include "../Context/Context.h"

#include <vector>
#include <string>
#include <map>

namespace dale
{
class ContextSavePoint
{
public:
    ContextSavePoint(Context *ctx);
    ~ContextSavePoint();
    bool restore(void);

private:
    NativeTypes *nt;
    std::map<std::string, int> *function_count;
    std::set<std::string> *variables;
    std::set<std::string> *labels;
    std::set<std::string> *structs;
    std::set<std::string> *enums;
    int active_namespace_count;
    int used_namespace_count;
    std::map<std::string, ContextSavePoint *> *namespaces;
    int current_namespace_count;
    Context *my_context;

    /* todo: sub-contexts. */
};
}

#endif
