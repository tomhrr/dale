#ifndef DALE_UNIT
#define DALE_UNIT

#include "../Parser/Parser.h"
#include "../Context/Context.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../NativeTypes/NativeTypes.h"
#include "../TypeRegister/TypeRegister.h"
#include "../DNodeConverter/DNodeConverter.h"
#include "../MacroProcessor/MacroProcessor.h"
#include "../FunctionProcessor/FunctionProcessor.h"

namespace llvm {
    class Linker;
    class Module;
    class ExecutionEngine;
}

namespace dale
{
class Generator;

/*! Unit

    A unit is created for each new file that is parsed.  It comprises
    the file's module, linker and execution engine (LLVM objects), as
    well as its context and parser (internal objects).  Each unit may
    also have a single once tag.
*/
class Unit
{
private:
    /*! The unit's global functions. */
    std::vector<Function*> global_functions;
    /*! The unit's global blocks. */
    std::vector<llvm::BasicBlock*> global_blocks;
    /*! The current global function. */
    Function *global_function;
    /*! The current global block. */
    llvm::BasicBlock *global_block;

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
    Unit(const char *path, Generator *gen, ErrorReporter *er,
         NativeTypes *nt, TypeRegister *tr, llvm::ExecutionEngine *ee,
         bool is_x86_64);
    ~Unit(void);
    /*! Check whether this unit has a once tag.
     */
    bool hasOnceTag(void);
    /*! Set the once tag for this unit.
     */
    bool setOnceTag(std::string new_once_tag);
    /*! Get the current global function.
     */
    Function *getGlobalFunction(void);
    /*! Get the current global function.
     */
    llvm::BasicBlock *getGlobalBlock(void);
    /*! Push a new global function onto the stack.
     */
    void pushGlobalFunction(Function *fn);
    /*! Push a new global block onto the stack.
     */
    void pushGlobalBlock(llvm::BasicBlock *block);
    /*! Pop the top global function from the stack.
     */
    void popGlobalFunction(void);
    /*! Pop the top global block from the stack.
     */
    void popGlobalBlock(void);
    /*! Add the necessary common declarations to this unit.
     */
    void addCommonDeclarations(void);
};
}

#endif
