#include "Config.h"
#if D_LLVM_VERSION_ORD <= 32
#include "llvm/CallingConv.h"
#else
#include "llvm/IR/CallingConv.h"
#endif
