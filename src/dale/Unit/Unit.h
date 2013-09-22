#ifndef DALE_UNIT
#define DALE_UNIT

#include "../Parser/Parser.h"
#include "../Context/Context.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../NativeTypes/NativeTypes.h"
#include "../TypeRegister/TypeRegister.h"

namespace llvm {
    class Linker;
    class Module;
    class ExecutionEngine;
}

namespace dale
{
class Unit
{
public:
    Unit(const char *path,
         ErrorReporter *er,
         NativeTypes *nt,
         TypeRegister *tr);
    ~Unit(void);
    llvm::Module *module;
    llvm::Linker *linker;
    llvm::ExecutionEngine *ee;
    Context *ctx;
    Parser *parser;
    std::string once_tag;
    bool hasOnceTag(void);
    bool setOnceTag(std::string new_once_tag);
};
}

#endif
