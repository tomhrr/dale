#ifndef DALE_LLVMUTILS
#define DALE_LLVMUTILS

#include <sys/stat.h>
#include <cerrno>
#include <climits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../Type/Type.h"
#include "../Unit/Unit.h"
#include "../Variable/Variable.h"
#include "../llvm_Linker.h"
#include "../llvm_Module.h"

#if D_LLVM_VERSION_ORD >= 33
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#endif

#if D_LLVM_VERSION_ORD <= 33
#include "llvm/PassManager.h"
#else
#include "llvm/IR/LegacyPassManager.h"
#endif

#if D_LLVM_VERSION_ORD >= 36
#include "llvm/Transforms/Utils/Cloning.h"
#endif

#if D_LLVM_VERSION_ORD <= 39
#include "llvm/Bitcode/ReaderWriter.h"
#else
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#endif

#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#define _unused(x) ((void)x)

#define STRTOUL_FAILED(ret, str, end)                                \
    (((((ret) == ULONG_MAX || ((ret) == 0)) && (errno == ERANGE)) || \
      (((ret) == 0) && ((str) == (end)))))
#define DECIMAL_RADIX 10

#if D_LLVM_VERSION_ORD <= 35
#define DECLARE_ENGINE_BUILDER(mod, name) \
    llvm::EngineBuilder name = llvm::EngineBuilder(mod);
#elif D_LLVM_VERSION_ORD <= 60
#define DECLARE_ENGINE_BUILDER(mod, name)                             \
    std::unique_ptr<llvm::Module> module_ptr(llvm::CloneModule(mod)); \
    llvm::EngineBuilder name(move(module_ptr));
#else
#define DECLARE_ENGINE_BUILDER(mod, name)                             \
    std::unique_ptr<llvm::Module> module_ptr(llvm::CloneModule(*mod)); \
    llvm::EngineBuilder name(move(module_ptr));
#endif

/* Utility functions that depend on LLVM classes.  Most of the ifdefs
 * required for backwards compatibility with older versions of LLVM
 * should be contained within the corresponding implementation file. */
namespace dale {
/*! The PassManager type. */
#if D_LLVM_VERSION_ORD <= 33
typedef llvm::PassManager PassManager;
#else
typedef llvm::legacy::PassManager PassManager;
#endif

/*! The formatted output stream type. */
#if D_LLVM_VERSION_ORD <= 36
typedef llvm::formatted_raw_ostream llvm_formatted_ostream;
#else
typedef llvm::raw_fd_ostream llvm_formatted_ostream;
#endif

/*! Get the current target triple. */
std::string getTriple();
/*! Link the given module using the linker.
 *  @param linker The linker.
 *  @param mod The module.
 */
void linkModule(llvm::Linker *linker, llvm::Module *mod);
/*! Add a data layout pass to the pass manager, based on the module's
 *  data layout, if required.
 *  @param pass_manager The pass manager.
 *  @param mod The module.
 */
void addDataLayout(PassManager *pass_manager, llvm::Module *mod);
/*! Add a print module pass to the pass manager.
 *  @param pass_manager The pass manager.
 *  @param ostream The output stream for the pass.
 */
void addPrintModulePass(PassManager *pass_manager,
                        llvm::raw_fd_ostream *ostream);
/*! Construct and return a new target machine.
 *  @param mod The module (for its reference to the triple).
 */
llvm::TargetMachine *getTargetMachine(llvm::Module *mod);
/*! Set the data layout for the module.
 *  @param mod The module.
 *  @param is_x86_64 Whether compilation is for x86-64.
 */
void setDataLayout(llvm::Module *mod, bool is_x86_64);
/*! Add the LTO passes to the pass manager.
 *  @param pass_manager_builder The pass manager builder.
 *  @param pass_manager The pass manager.
 */
void populateLTOPassManager(
    llvm::PassManagerBuilder *pass_manager_builder,
    PassManager *pass_manager);
/*! Get a formatted output stream from a raw output stream.
 *  @param ostream The raw output stream.
 */
llvm_formatted_ostream *getFormattedOstream(
    llvm::raw_fd_ostream *ostream);
/*! Print the module to stderr, verify it if possible, and abort if
 *  verification fails.
 *  @param mod The module.
 */
void moduleDebugPass(llvm::Module *mod);
/*! Print the function to stderr, verify it if possible, and abort if
 *  verification fails.
 *  @param fn The function.
 */
void functionDebugPass(llvm::Function *fn);
/*! Add the 'always inline' attribute to the function.
 *  @param fn The function.
 */
void addInlineAttribute(llvm::Function *fn);
/*! Convert an instruction into an instruction iterator.
 *  @param inst The instruction.
 */
llvm::BasicBlock::iterator instructionToIterator(
    llvm::Instruction *inst);
/*! Set the insertion point of the builder to be before the specified
 *  instruction.
 *  @param builder The builder.
 *  @param iter The instruction iterator.
 */
void setInsertPoint(llvm::IRBuilder<> *builder,
                    llvm::BasicBlock::iterator iter);
/*! Get the current EE address for the given variable.
 *  @param ee The execution engine.
 *  @param var The variable.
 */
uint64_t variableToAddress(llvm::ExecutionEngine *ee, Variable *var);
/*! Get the current EE address for the given function.
 *  @param ee The execution engine.
 *  @param fn The function.
 */
uint64_t functionToAddress(Unit *unit, Function *fn);

/*! Clone the current module and add it to the EE, if required.  This
 *  should only be used before executing a function via the JIT.
 *  Cloning is only required in LLVM versions 3.6 and following, because
 *  of how the EE takes ownership of the module in those versions.
 *  @param unit The current unit.
 */
void cloneModuleIfRequired(Unit *unit);
/*! Set the standard attributes for the 'basic' functions (i.e. the
 *  core arithmetical and relational functions).
 *  @param fn The function.
 */
void setStandardAttributes(llvm::Function *fn);
/*! Load the module.
 *  @param path The path to the module.
 */
llvm::Module *loadModule(std::string *path);
/*! Construct a new linker.
 *  @param path The path to the current executable ('.../dalec[i]').
 *  @param mod The initial module for linking.
 */
llvm::Linker *newLinker(const char *path, llvm::Module *mod);
/*! Get an LLVM function type.
 *  @param t The return type.
 *  @param v The parameter types.
 *  @param b Whether the function is a varargs function.
 */
llvm::FunctionType *getFunctionType(llvm::Type *t,
                                    std::vector<llvm::Type *> const &v,
                                    bool b);
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
llvm::LLVMContext *getContext();

/*! Create a new empty function returning the specified type.
 *  @param units The units context.
 *  @param type The return type.
 *  @param top A reference node for errors.
 */
Function *createFunction(Units *units, Type *type, Node *top);

/*! Link the specified functions/variables into the current module,
 *  inlining anything not already available.
 *  @param mod The top-level module.
 *  @param top A reference node for errors.
 *  @param functions The functions to link into the module.
 *  @param variables The variables to link into the module.
 */
void linkRetrievedObjects(llvm::Module *mod, Node *top,
                          std::vector<Function *> *functions,
                          std::vector<Variable *> *variables);
}

#endif
