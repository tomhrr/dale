#include "Config.h"
#if D_LLVM_VERSION_ORD <= 32
#include "llvm/Function.h"
#else
#include "llvm/IR/Function.h"
#endif
