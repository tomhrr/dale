#include "Config.h"
#if D_LLVM_VERSION_MINOR <= 4
#include "llvm/Linker.h"
#else
#include "llvm/Linker/Linker.h"
#endif
