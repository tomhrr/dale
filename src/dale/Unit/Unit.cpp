#include "Unit.h"
#include "../Lexer/Lexer.h"
#include "../llvm_Module.h"
#include "../llvm_Linker.h"

namespace dale
{
Unit::Unit(const char *path, ErrorReporter *er, NativeTypes *nt,
           TypeRegister *tr)
{
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("Unable to open file");
        exit(1);
    }

    er->current_filename = path;
    dnc = new DNodeConverter(er);

    ctx = new Context(er, nt, tr);

    Lexer *lxr = new Lexer(fp);
    parser = new Parser(lxr, er, path);

    module = new llvm::Module(path, llvm::getGlobalContext());

#if D_LLVM_VERSION_MINOR <= 2
    linker = new llvm::Linker(path, module, false);
#else
    linker = new llvm::Linker(module);
#endif

    ee = NULL;
}

Unit::~Unit(void)
{
    delete ctx;
    delete parser;
}

bool
Unit::hasOnceTag(void)
{
    return (once_tag.size() ? true : false);
}

bool
Unit::setOnceTag(std::string new_once_tag)
{
    if (hasOnceTag()) {
        return false;
    }

    once_tag = new_once_tag;
    return true;
}
}
