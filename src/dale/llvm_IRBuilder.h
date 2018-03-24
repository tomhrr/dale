#include "Config.h"
#if D_LLVM_VERSION_ORD == 32
#include "llvm/IRBuilder.h"
#include "llvm/TypeBuilder.h"
#include "llvm/DataLayout.h"
#elif D_LLVM_VERSION_ORD >= 33
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/DataLayout.h"
#else
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/TypeBuilder.h"
#endif
