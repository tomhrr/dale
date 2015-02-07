#ifndef DALE_UNIT
#define DALE_UNIT

#include "../Parser/Parser.h"
#include "../Context/Context.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../NativeTypes/NativeTypes.h"
#include "../TypeRegister/TypeRegister.h"
#include "../DNodeConverter/DNodeConverter.h"

namespace llvm {
    class Linker;
    class Module;
    class ExecutionEngine;
}

namespace dale
{
/*! Unit

    A unit is created for each new file that is parsed.  It comprises
    the file's module, linker and execution engine (LLVM objects), as
    well as its context and parser (internal objects).  Each unit may
    also have a single once tag.
*/
class Unit
{
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
    /*! The unit's once tag (optional). */
    std::string once_tag;

    /*! Construct a new unit.
     *  @param path The path to the file being parsed.
     *  @param er The error reporter.
     *  @param nt The native types object.
     *  @param tr The type register.
     *
     *  A new context and parser will be instantiated on construction,
     *  ownership of both being retained by the unit.
     */
    Unit(const char *path, ErrorReporter *er, NativeTypes *nt,
         TypeRegister *tr);
    ~Unit(void);
    /*! Check whether this unit has a once tag.
     */
    bool hasOnceTag(void);
    /*! Set the once tag for this unit.
     */
    bool setOnceTag(std::string new_once_tag);
};
}

#endif
