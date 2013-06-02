#ifndef DALE_DEFERREDGOTO
#define DALE_DEFERREDGOTO

#include "llvm/LLVMContext.h"
#include "llvm/Function.h"

namespace dale
{
class Context;
class Node;

/*! DeferredGoto
    
    A class for containing the details of a 'deferred goto', being a
    goto for a label that is not yet in scope.

*/
class DeferredGoto
{
public:
    llvm::Instruction *marker;
    std::string *label_name;
    int no_instruction;
    llvm::BasicBlock *block_marker;
    Context *ctx;
    int index;
    Node *node;
};
}

#endif
