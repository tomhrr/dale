#include "Config.h"
#if D_LLVM_VERSION_ORD <= 35
#include "llvm/PassManager.h"
#else
#include "llvm/IR/LegacyPassManager.h"
#endif
