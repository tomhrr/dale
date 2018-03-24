#include "Config.h"
#if D_LLVM_VERSION_ORD <= 32
#include "llvm/ValueSymbolTable.h"
#else
#include "llvm/IR/ValueSymbolTable.h"
#endif
