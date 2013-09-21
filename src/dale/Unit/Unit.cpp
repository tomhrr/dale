#include "Unit.h"
#include "../Lexer/Lexer.h"
#include "llvm/Module.h"
#include "llvm/Linker.h"

namespace dale
{
Unit::Unit(const char *path,
           ErrorReporter *er,
           NativeTypes *nt,
           TypeRegister *tr)
{
    er->current_filename = path;

    ctx = new Context(er, nt, tr);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("Unable to open file");
        exit(1);
    }

    std::vector<Token *> *ungot_tokens = new std::vector<Token *>;
    Lexer *lxr = new Lexer(fp, ungot_tokens, 1, 1);
    parser = new Parser(lxr, er, path);

    module = new llvm::Module(path, llvm::getGlobalContext());
    linker = new llvm::Linker(path, module, false);
}

Unit::~Unit(void)
{
    delete ctx;
    delete parser;
}

bool Unit::hasOnceTag(void)
{
    return (once_tag.size() ? true : false);
}

bool Unit::setOnceTag(std::string new_once_tag)
{
    if (hasOnceTag()) {
        return false;
    }

    once_tag = new_once_tag;
    return true;
}
}
