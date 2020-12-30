#ifndef DALE_ELEMENT_LABEL
#define DALE_ELEMENT_LABEL

#include <cstdlib>

#include "llvm/IR/BasicBlock.h"
#include "../llvm_PassManager.h"

namespace dale {
class Namespace;

/*! Label

    A class for storing the details of a label.
*/
class Label {
    public:
    /*! The basic block at which this label begins. */
    llvm::BasicBlock *block;
    /*! The namespace active when the label was defined. */
    Namespace *ns;
    /*! The label's index.  See Namespace::lv_index. */
    int index;

    /*! Construct a new label using the given parameters.
     *  @param block The basic block.
     *  @param ns The namespace.
     *  @param index The index.
     *
     *  Neither block nor ns are copied.  This does not take ownership
     *  of them, either.
     */
    Label(llvm::BasicBlock *block, Namespace *ns, int index);
    ~Label();
};
}

#endif
