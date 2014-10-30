#ifndef DALE_ELEMENT_LABEL
#define DALE_ELEMENT_LABEL

#include <string>
#include <cstdlib>

#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "../../llvm_IRBuilder.h"
#include "../../llvm_Module.h"
#include "../../llvm_Function.h"
#include "../../llvm_CallingConv.h"

namespace dale
{
class Namespace;

namespace Element
{
class Label
{
public:
    llvm::BasicBlock *block;
    int              index;
    Namespace        *ns;

    Label();
    ~Label();
};
}
}

#endif
