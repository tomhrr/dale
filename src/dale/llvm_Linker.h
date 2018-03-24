#include "Config.h"
#if D_LLVM_VERSION_ORD <= 34
#include "llvm/Linker.h"
#else
#include "llvm/Linker/Linker.h"
#endif
