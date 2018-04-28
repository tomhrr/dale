#ifndef DALE_CONTEXT_SAVEPOINT
#define DALE_CONTEXT_SAVEPOINT

#include <map>
#include <string>

#include "../Context/Context.h"
#include "../NamespaceSavePoint/NamespaceSavePoint.h"

namespace dale {
struct SPNode {
    NamespaceSavePoint *nsp;
    std::map<std::string, SPNode *> children;
};
/*! ContextSavePoint

    A class for storing the state of a context at a given time, and
    for restoring the context to that state.  At the moment, this is
    only used when determining whether a given binding is a macro or a
    function, so that anything done in order to make that
    determination can be reversed.
*/
class ContextSavePoint {
    private:
    Context *src_ctx;
    SPNode *nsps;
    NSNode *active;
    int active_count;
    int used_count;

    public:
    /*! Construct a new savepoint using the given context.
     *  @param ctx The context.
     */
    explicit ContextSavePoint(Context *ctx);
    ~ContextSavePoint();
    /*! Restore the context so it is as it was prior to savepoint
     *  instantiation.
     */
    bool restore();
};
}

#endif
