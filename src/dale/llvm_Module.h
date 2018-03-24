#include "Config.h"
#if D_LLVM_VERSION_ORD <= 32
#include "llvm/Module.h"
#else
#include "llvm/IR/Module.h"
#endif
