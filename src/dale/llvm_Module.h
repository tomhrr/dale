#include "Config.h"
#if D_LLVM_VERSION_MINOR <= 2
#include "llvm/Module.h"
#else
#include "llvm/IR/Module.h"
#endif
