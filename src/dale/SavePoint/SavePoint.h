#ifndef DALE_SAVEPOINT
#define DALE_SAVEPOINT

#include <map>
#include <string>
#include <vector>

#include "../Context/Context.h"
#include "../ContextSavePoint/ContextSavePoint.h"
#include "../NamespaceSavePoint/NamespaceSavePoint.h"

namespace dale {
/*! SavePoint

    A wrapper around ContextSavePoint that also stores the state of a
    given LLVM function at a particular time, and allows for restoring
    that state.
*/
class SavePoint {
    private:
    int block_count;
    int instruction_index;
    int dg_count;
    std::map<std::string, Label *> labels;
    Function *fn;
    llvm::BasicBlock *block;
    ContextSavePoint *csp;

    public:
    /*! Construct a new savepoint.
     *  @param ctx The context.
     *  @param fn The current function.
     *  @param block The current LLVM block.
     */
    SavePoint(Context *ctx, Function *fn, llvm::BasicBlock *block);
    ~SavePoint();
    /*! Restore the context and function state so it is as it was
     *  prior to savepoint instantiation.
     */
    bool restore();
};
}

#endif
