/* vim: set sw=4 ts=4: */
#ifndef PP_CONTEXT
#define PP_CONTEXT

#include <string>

#include "llvm/Config/config.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Path.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Version.h"
#include "clang/Basic/FileManager.h"

#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/FrontendOptions.h"
#include "clang/Frontend/PreprocessorOptions.h"
#include "clang/Frontend/HeaderSearchOptions.h"
#include "clang/Frontend/Utils.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/MacroBuilder.h"

using namespace clang;
using namespace llvm;
struct PPContext {
    // Takes ownership of client.
    PPContext(
        clang::TargetOptions targetOpts,
        clang::DiagnosticClient* client,
        clang::LangOptions langOpts,
        std::string const &isysroot,
        std::vector<std::string> const &frameworkDirs,
        std::vector<std::string> const &headerDirs
    ) :
        diags(IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()), client),
        opts(langOpts),
        target(clang::TargetInfo::CreateTargetInfo(diags,targetOpts)),
        fm(FileSystemOptions()),
        headers(fm),
        sm(diags, fm),
        pp(diags, opts, *target, sm, headers)
    {
        diags.setSuppressSystemWarnings(true);

        HeaderSearchOptions hsOpts;
#if (defined(LLVM_LIBDIR) && defined (CLANG_VERSION_STRING))
        llvm::sys::Path resourceDir(LLVM_LIBDIR);
        resourceDir.appendComponent("clang");
        resourceDir.appendComponent(CLANG_VERSION_STRING);

        hsOpts.ResourceDir = resourceDir.c_str();
#endif
        hsOpts.UseBuiltinIncludes = 1;
        hsOpts.UseStandardIncludes = 1;

        for (std::vector<std::string>::const_iterator i = frameworkDirs.begin();
                i != frameworkDirs.end();
                ++i)
            hsOpts.AddPath(*i, clang::frontend::Angled, true, true, false);

        for (std::vector<std::string>::const_iterator i = headerDirs.begin();
                i != headerDirs.end();
                ++i)
            hsOpts.AddPath(*i, clang::frontend::Angled, true, false, false);

        hsOpts.Sysroot = isysroot;
        PreprocessorOptions ppo = PreprocessorOptions();
        ppo.addMacroDef(llvm::StringRef("__USE_MISC"));

        InitializePreprocessor(pp, ppo, hsOpts, FrontendOptions());
    }

    ~PPContext()
    {
        delete target;
    }


    clang::Diagnostic diags;
    clang::LangOptions opts;
    clang::TargetInfo* target;
    clang::FileManager fm;
    clang::HeaderSearch headers;
    clang::SourceManager sm;
    clang::Preprocessor pp;
};

#endif
