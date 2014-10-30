#include "Config.h"
#if D_LLVM_VERSION_MINOR <= 2
#include "llvm/ValueSymbolTable.h"
#else
#include "llvm/IR/ValueSymbolTable.h"
#endif
