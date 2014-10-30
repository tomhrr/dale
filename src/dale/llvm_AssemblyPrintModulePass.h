#include "Config.h"
#if D_LLVM_VERSION_MINOR <= 4
#include "llvm/Assembly/PrintModulePass.h"
#else
#include "llvm/IR/IRPrintingPasses.h"
#endif
