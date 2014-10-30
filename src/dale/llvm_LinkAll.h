#include "Config.h"
#if D_LLVM_VERSION_MINOR <= 2
#include "llvm/LinkAllVMCore.h"
#else
#include "llvm/LinkAllIR.h"
#endif
#include "llvm/LinkAllPasses.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
