#include "Config.h"
#if D_LLVM_VERSION_ORD == 32
#include "llvm/DataLayout.h"
#include "llvm/IRBuilder.h"
#include "llvm/TypeBuilder.h"
#elif D_LLVM_VERSION_ORD <= 70
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/TypeBuilder.h"
#else
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#endif
