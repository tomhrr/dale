#ifndef DALE_ELEMENT_LABEL
#define DALE_ELEMENT_LABEL

#include <string>
#include <cstdlib>

#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

namespace dale
{
class Context;

namespace Element
{
class Label
{
public:
    llvm::BasicBlock *block;
    int              index;
    Context *ctx;

    Label();
    ~Label();
};
}
}

#endif
