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
#include "../Unit/Unit.h"

#if D_LLVM_VERSION_ORD >= 33
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#endif

#if D_LLVM_VERSION_ORD <= 33
#include "llvm/PassManager.h"
#else
#include "llvm/IR/LegacyPassManager.h"
#endif

#if D_LLVM_VERSION_ORD >= 36
#include "llvm/Transforms/Utils/Cloning.h"
#endif

#include "llvm/Support/TargetRegistry.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"

#define _unused(x) ((void)x)

#define STRTOUL_FAILED(ret, str, end) \
    (((((ret) == ULONG_MAX || ((ret) == 0)) && (errno == ERANGE)) \
                || (((ret) == 0) && ((str) == (end)))))
#define DECIMAL_RADIX 10

#if D_LLVM_VERSION_ORD <= 35
#define DECLARE_ENGINE_BUILDER(mod, name) llvm::EngineBuilder name = llvm::EngineBuilder(mod);
#else
#define DECLARE_ENGINE_BUILDER(mod, name) std::unique_ptr<llvm::Module> module_ptr(llvm::CloneModule(mod)); llvm::EngineBuilder name(move(module_ptr));
#endif

namespace dale
{
/*! The PassManager type. */
#if D_LLVM_VERSION_ORD <= 33
typedef llvm::PassManager PassManager;
#else
typedef llvm::legacy::PassManager PassManager;
#endif

#if D_LLVM_VERSION_ORD <= 36
typedef llvm::formatted_raw_ostream llvm_formatted_ostream;
#else
typedef llvm::raw_fd_ostream llvm_formatted_ostream;
#endif

std::string getTriple();
void linkModule(llvm::Linker *linker, llvm::Module *mod);
void addDataLayout(PassManager *pass_manager, llvm::Module *mod);
void addPrintModulePass(PassManager *pass_manager,
                        llvm::raw_fd_ostream *ostream);
llvm::TargetMachine* getTargetMachine(llvm::Module *last_module);
void setDataLayout(llvm::Module *module, bool is_x86_64);
void populateLTOPassManager(llvm::PassManagerBuilder *pass_manager_builder,
                            PassManager *pass_manager);
llvm_formatted_ostream* getFormattedOstream(llvm::raw_fd_ostream *ostream);
void moduleDebugPass(llvm::Module *mod);
void functionDebugPass(llvm::Function *fn);
void addInlineAttribute(llvm::Function *fn);
llvm::BasicBlock::iterator instructionToIterator(llvm::Instruction *inst);
void setInsertPoint(llvm::IRBuilder<> *builder, llvm::BasicBlock::iterator iter);
uint64_t variableToAddress(llvm::ExecutionEngine *ee, Variable *var);
uint64_t functionToAddress(Unit *unit, Function *fn);
void cloneModuleIfRequired(Unit *unit);

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
