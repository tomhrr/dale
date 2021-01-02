#ifndef DALE_UNIT
#define DALE_UNIT

#include <string>
#include <vector>

#include "../Context/Context.h"
#include "../DNodeConverter/DNodeConverter.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../FunctionProcessor/FunctionProcessor.h"
#include "../MacroProcessor/MacroProcessor.h"
#include "../NativeTypes/NativeTypes.h"
#include "../Parser/Parser.h"
#include "../TypeRegister/TypeRegister.h"

namespace llvm {
class Linker;
class Module;
class ExecutionEngine;
}

namespace dale {
class Units;

/*! Unit

    A unit is created for each new file that is parsed.  It comprises
    the file's module, linker and execution engine (LLVM objects), as
    well as its context and parser (internal objects).  Each unit may
    also have a single once tag.
*/
class Unit {
    private:
    /*! The unit's global functions. */
    std::vector<Function *> global_functions;
    /*! The unit's global blocks. */
    std::vector<llvm::BasicBlock *> global_blocks;
    /*! The current global function. */
    Function *global_function;
    /*! The current global block. */
    llvm::BasicBlock *global_block;
    /*! The current variable index. */
    int var_count;
    /*! The current function index. */
    int fn_count;
    /*! The unused name prefix. */
    char unused_name_prefix[4];
    /*! Whether this unit has its own module. */
    bool has_own_module;

    public:
    /*! The unit's LLVM module. */
    llvm::Module *module;
    /*! The unit's linker. */
    llvm::Linker *linker;
    /*! The unit's execution engine. */
    llvm::ExecutionEngine *ee;
    /*! The unit's context. */
    Context *ctx;
    /*! The unit's parser. */
    Parser *parser;
    /*! The unit's DNode converter. */
    DNodeConverter *dnc;
    /*! The unit's macro processor. */
    MacroProcessor *mp;
    /*! The unit's function processor. */
    FunctionProcessor *fp;
    /*! The unit's once tag (optional). */
    std::string once_tag;
    /*! Whether this is an x86-64 platform. */
    bool is_x86_64;

    /*! Construct a new unit.
     *  @param path The path to the file being parsed.
     *  @param er The error reporter.
     *  @param nt The native types object.
     *  @param tr The type register.
     *  @param ee The execution engine.
     *  @param is_x86_64 Whether this is an x86-64 platform.
     *
     *  A new context and parser will be instantiated on construction,
     *  ownership of both being retained by the unit.
     */
    Unit(const char *path, Units *units, ErrorReporter *er,
         NativeTypes *nt, TypeRegister *tr, llvm::ExecutionEngine *ee,
         bool is_x86_64, Context *ctx, MacroProcessor *mp,
         FunctionProcessor *fp, llvm::Module *module,
         llvm::Linker *linker, bool line_buffered = false);
    /*! Construct a new unit.
     *
     *  The same as the default constructor, save that no path is
     *  provided.  See FormValueParse.
     */
    Unit(Units *units, ErrorReporter *er,
         NativeTypes *nt, TypeRegister *tr, llvm::ExecutionEngine *ee,
         bool is_x86_64, Context *ctx, MacroProcessor *mp,
         FunctionProcessor *fp, llvm::Module *module,
         llvm::Linker *linker, bool line_buffered = false);
    ~Unit();
    /*! Check whether this unit has a once tag.
     */
    bool hasOnceTag();
    /*! Set the once tag for this unit.
     */
    bool setOnceTag(std::string new_once_tag);
    /*! Get the current global function.
     */
    Function *getGlobalFunction();
    /*! Get the current global function.
     */
    llvm::BasicBlock *getGlobalBlock();
    /*! Push a new global function onto the stack.
     */
    void pushGlobalFunction(Function *fn);
    /*! Push a new global block onto the stack.
     */
    void pushGlobalBlock(llvm::BasicBlock *block);
    /*! Pop the top global function from the stack.
     */
    void popGlobalFunction();
    /*! Pop the top global block from the stack.
     */
    void popGlobalBlock();
    /*! Add a temporary global function.
     */
    void makeTemporaryGlobalFunction();
    /*! Remove a temporary global function.
     */
    void removeTemporaryGlobalFunction();
    /*! Add the necessary common declarations to this unit.
     */
    void addCommonDeclarations();
    /*! Get an unused LLVM variable name.
     */
    void getUnusedVarName(std::string *buf);
    /*! Get an unused LLVM function name.
     */
    void getUnusedFunctionName(std::string *buf);
    /*! Returns a boolean indicating whether this unit has its own
     *  module. */
    bool hasOwnModule();
};
}

#endif
