#include "Config.h"
#if D_LLVM_VERSION_ORD <= 34
#include "llvm/Analysis/Verifier.h"
#else
#include "llvm/IR/Verifier.h"
#endif
