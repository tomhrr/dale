#ifndef DALE_LLVMUTILS
#define DALE_LLVMUTILS

#include <climits>
#include <cerrno>
#include <sys/stat.h>
#include <vector>
#include <string>

#include "../llvm_Module.h"
#include "../llvm_Linker.h"
#include "../Type/Type.h"
#include "../Variable/Variable.h"

#if D_LLVM_VERSION_ORD >= 33
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#endif

#if D_LLVM_VERSION_ORD <= 33
#include "llvm/PassManager.h"
#else
#include "llvm/IR/LegacyPassManager.h"
#endif

#define _unused(x) ((void)x)

#define STRTOUL_FAILED(ret, str, end) \
    (((((ret) == ULONG_MAX || ((ret) == 0)) && (errno == ERANGE)) \
                || (((ret) == 0) && ((str) == (end)))))
#define DECIMAL_RADIX 10

namespace dale
{
/*! The PassManager type. */
#if D_LLVM_VERSION_ORD <= 33
typedef llvm::PassManager PassManager;
#else
typedef llvm::legacy::PassManager PassManager;
#endif

std::string getTriple();

/*! Get an LLVM function type.
 *  @param t The return type.
 *  @param v The parameter types.
 *  @param b Whether the function is a varargs function.
 */
llvm::FunctionType *getFunctionType(llvm::Type *t,
                                    std::vector<llvm::Type*> &v, bool b);
/*! Construct an LLVM string constant data array.
 *  @param data The data for the array.
 */
llvm::Constant *getStringConstantArray(const char *data);
/*! Construct an LLVM null pointer for the type.
 *  @param type The type.
 */
llvm::ConstantPointerNull *getNullPointer(llvm::Type *type);
/*! Link a file into the given linker.
 *  @param linker The linker.
 *  @param path The path to the file to be linked.
 */
void linkFile(llvm::Linker *linker, const char *path);
/*! Get a new context.
 */
llvm::LLVMContext* getContext();
}

#endif
