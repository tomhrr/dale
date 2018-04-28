#include "Config.h"
#if D_LLVM_VERSION_ORD <= 32
#include "llvm/LLVMContext.h"
#else
#include "llvm/IR/LLVMContext.h"
#endif
