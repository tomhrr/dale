#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "llvm/Config/config.h"
#include "llvm/Support/raw_ostream.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclGroup.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Parse/ParseAST.h"
#include "llvm/ADT/Triple.h"

#include "src/dale/PPContext.h"

#include "src/dale/BindingsConverter.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Please provide path.\n");
        return EXIT_FAILURE;
    }

    std::string target = LLVM_HOSTTRIPLE;
    std::string isysroot;
    std::vector<std::string> frameworkDirs;
    std::vector<std::string> headerDirs;
    std::string language = "c";
    std::vector<std::string> matchNames;
    std::vector<std::string> importNames;

    std::ostream &outstream = std::cout;
    llvm::Triple triple(target);

    clang::TargetOptions targetOpts;
    targetOpts.Triple = triple.str();

    clang::DiagnosticOptions diagOpts;

    BindingsConverter *converter = new BindingsConverter(
        outstream,
        diagOpts,
        language,
        matchNames,
        importNames
    );

    clang::LangOptions langOpts;
    langOpts.C99 = 1;
    langOpts.GNUMode = 1;
    if (language == "objective-c") {
        langOpts.ObjC1 = 1;
        langOpts.ObjC2 = 1;
    }

    PPContext context(
        targetOpts, converter, langOpts,
        isysroot, frameworkDirs, headerDirs
    );

    const FileEntry *file = context.fm.getFile(argv[1]);
    if (!file) {
        fprintf(stderr, "Unable to open file.\n");
        return EXIT_FAILURE;
    }
    context.sm.createMainFileID(file);

    clang::Builtin::Context builtins(*context.target);
    builtins.InitializeBuiltins(context.pp.getIdentifierTable(),
                                langOpts);

    clang::ASTContext astContext(context.opts,
                                 context.sm,
                                 *context.target,
                                 context.pp.getIdentifierTable(),
                                 context.pp.getSelectorTable(),
                                 builtins,
                                 0);

    converter->BeginSourceFile(langOpts, &context.pp);
    ParseAST(context.pp, converter, astContext);
    converter->EndSourceFile();

    converter->generate();

    return 0;
}
