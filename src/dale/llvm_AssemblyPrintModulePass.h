#include "Config.h"
#if D_LLVM_VERSION_ORD <= 34
#include "llvm/Assembly/PrintModulePass.h"
#else
#include "llvm/IR/IRPrintingPasses.h"
#endif
