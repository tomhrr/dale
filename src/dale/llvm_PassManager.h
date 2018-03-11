#include "Config.h"
#if D_LLVM_VERSION_MINOR <= 5
#include "llvm/PassManager.h"
#else
#include "llvm/IR/LegacyPassManager.h"
#endif
