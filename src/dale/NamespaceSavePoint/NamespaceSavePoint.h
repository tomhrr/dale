#ifndef DALE_NAMESPACE_SAVEPOINT
#define DALE_NAMESPACE_SAVEPOINT

#include "../Namespace/Namespace.h"

#include <map>
#include <string>
#include <vector>

namespace dale {
/*! NamespaceSavePoint

    A class for storing the state of a namespace at a given time, and
    for restoring the namespace to that state.  At the moment, this is
    only used by ContextSavePoint.
*/
class NamespaceSavePoint {
   public:
    /*! Construct a new savepoint using the given namespace.
     *  @param ns The namespace.
     */
    NamespaceSavePoint(Namespace *ns);
    ~NamespaceSavePoint();
    /*! Restore the context so it is as it was prior to savepoint
     *  instantiation.
     */
    bool restore();

   private:
    std::map<std::string, int> function_count;
    int variable_count;
    int struct_count;
    Namespace *src_ns;
};
}

#endif
