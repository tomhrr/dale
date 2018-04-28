#ifndef DALE_DEFERREDGOTO
#define DALE_DEFERREDGOTO

#include <string>

#include "../llvm_Function.h"
#include "../llvm_LLVMContext.h"

namespace dale {
class Node;
class Namespace;

/*! DeferredGoto

    A class for containing the details of a 'deferred goto', being a
    goto for a label that is not yet in scope. The generator will
    resolve deferred gotos once the function has been processed.
*/
class DeferredGoto {
    public:
    /*! The instruction after which the goto instruction should be
     *  added. This may be NULL, when e.g. the prospective goto will
     *  be the first instruction within the relevant block. */
    llvm::Instruction *marker;
    /*! The name of the label to which the goto should branch. */
    std::string label_name;
    /*! The block to which the goto should be added. */
    llvm::BasicBlock *block_marker;
    /*! The namespace to which the label's function belongs. */
    Namespace *ns;
    /*! The label's index. Used to determine the variables for which
     *  destructor calls should be added. */
    int index;
    /*! The node for the original goto form. Used for error reporting,
     *  in the event that the goto cannot be resolved. */
    Node *node;
};
}

#endif
