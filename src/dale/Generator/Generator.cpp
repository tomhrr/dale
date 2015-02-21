#include "Generator.h"
#include "Config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <unistd.h>
#include <setjmp.h>
#include <float.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include FFI_HEADER

#if D_LLVM_VERSION_MINOR <= 4
#include "llvm/Support/system_error.h"
#else
#include "llvm/Object/Error.h"
#endif
#if D_LLVM_VERSION_MINOR >= 3
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#endif

#include "../llvm_LLVMContext.h"
#include "../llvm_Module.h"
#include "../llvm_Linker.h"
#include "../llvm_Function.h"
#include "../llvm_CallingConv.h"
#include "../llvm_AssemblyPrintModulePass.h"
#include "../llvm_ValueSymbolTable.h"
#include "../llvm_AnalysisVerifier.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Signals.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include "../Units/Units.h"
#include "../Label/Label.h"
#include "../Context/Context.h"
#include "../Function/Function.h"
#include "../Variable/Variable.h"
#include "../Type/Type.h"
#include "../Struct/Struct.h"
#include "../ParseResult/ParseResult.h"
#include "../Parser/Parser.h"
#include "../Node/Node.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Enum/Enum.h"
#include "../TypeRegister/TypeRegister.h"
#include "../llvm_IRBuilder.h"
#include "../BasicTypes/BasicTypes.h"
#include "../ErrorType/ErrorType.h"
#include "../Utils/Utils.h"
#include "../Linkage/Linkage.h"
#include "../BaseType/BaseType.h"
#include "../Lexer/Lexer.h"
#include "../STL/STL.h"
#include "../Serialise/Serialise.h"
#include "../NativeTypes/NativeTypes.h"
#include "../ContextSavePoint/ContextSavePoint.h"
#include "../Module/Writer/Writer.h"
#include "../Form/Macro/ArrayDeref/ArrayDeref.h"
#include "../Form/Macro/StructDeref/StructDeref.h"
#include "../Form/Macro/DerefStructDeref/DerefStructDeref.h"
#include "../Form/Macro/DerefStruct/DerefStruct.h"
#include "../Form/Macro/Setv/Setv.h"
#include "../Form/Linkage/Enum/Enum.h"
#include "../Form/Linkage/Struct/Struct.h"
#include "../Form/Linkage/Linkage.h"
#include "../Form/Type/Type.h"
#include "../Form/ProcBody/ProcBody.h"
#include "../Form/Proc/Token/Token.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Form/TopLevel/Inst/Inst.h"
#include "../Unit/Unit.h"
#include "../CoreForms/CoreForms.h"
#include "../CommonDecl/CommonDecl.h"
#include "../Operation/Sizeof/Sizeof.h"
#include "../Operation/Offsetof/Offsetof.h"
#include "../MacroProcessor/MacroProcessor.h"
#include "../Operation/Coerce/Coerce.h"
#include "../Operation/Destruct/Destruct.h"
#include "../Operation/Copy/Copy.h"

static const char *x86_64_layout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128";
static const char *x86_32_layout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32";

extern "C" {
    void init_introspection_functions(void);
    void *find_introspection_function(const char *);
}

namespace dale
{
Generator::Generator()
{
    CoreForms::init();

    llvm::InitializeNativeTarget();
    llvm::InitializeAllAsmPrinters();
}

Generator::~Generator()
{
}

void *
lazyFunctionCreator(const std::string &name)
{
    void *fn_pointer = find_introspection_function(name.c_str());
    if (fn_pointer) {
        return fn_pointer;
    }

    if (name[0] != '_') {
        /* Try for one beginning with an underscore (OS X-specific). */
        std::string osx_name;
        osx_name.append("_");
        osx_name.append(name);

        fn_pointer =
            llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(osx_name);
        if (fn_pointer) {
            return fn_pointer;
        }
    }

    fprintf(stderr,
            "Internal warning: can't find symbol '%s' in installed "
            "lazy function creator.\n",
            name.c_str());

    return NULL;
}

std::string
getTriple(void)
{
#if D_LLVM_VERSION_MINOR >= 2
    return llvm::sys::getDefaultTargetTriple();
#else
    return llvm::sys::getHostTriple();
#endif
}

void
linkModule(llvm::Linker *linker, llvm::Module *mod)
{
    std::string error;
    bool result;
#if D_LLVM_VERSION_MINOR <= 2
    result = linker->LinkInModule(mod, &error);
#else
    result = linker->linkInModule(mod, &error);
#endif
    if (result) {
        fprintf(stderr,
                "Internal error: unable to link Dale module: %s\n",
                error.c_str());
        abort();
    }
}

int
Generator::run(std::vector<const char *> *file_paths,
               std::vector<const char *> *bc_file_paths,
               std::vector<const char *> *compile_lib_paths,
               std::vector<const char *> *include_paths,
               std::vector<const char *> *module_paths,
               std::vector<const char *> *static_module_names,
               std::vector<const char *> *cto_module_names,
               const char *module_name,
               int debug,
               int produce,
               int optlevel,
               int remove_macros,
               int no_common,
               int no_dale_stdlib,
               int static_mods_all,
               int enable_cto,
               std::vector<std::string> *shared_object_paths,
               FILE *output_file)
{
    if (!file_paths->size()) {
        return 0;
    }

    NativeTypes *nt = new NativeTypes();
    TypeRegister *tr = new TypeRegister();
    llvm::ExecutionEngine *ee = NULL;

    std::map<std::string, llvm::Module*> dtm_modules;
    std::map<std::string, std::string> dtm_nm_modules;

    std::set<std::string> cto_modules;
    for (std::vector<const char*>::iterator
            b = cto_module_names->begin(),
            e = cto_module_names->end();
            b != e;
            ++b) {
        cto_modules.insert(std::string(*b));
    }

    std::vector<llvm::Value *> two_zero_indices;
    two_zero_indices.clear();
    STL::push_back2(&two_zero_indices,
                    nt->getLLVMZero(), nt->getLLVMZero());

    /* On OS X, SYSTEM_PROCESSOR is i386 even when the underlying
     * processor is x86-64, hence the extra check here. */
    bool is_x86_64 =
        ((!strcmp(SYSTEM_PROCESSOR, "x86_64"))
         || ((!strcmp(SYSTEM_PROCESSOR, "amd64")))
         || ((!strcmp(SYSTEM_NAME, "Darwin"))
             && (sizeof(char *) == 8)));

    init_introspection_functions();

    llvm::Module *last_module = NULL;

    Module::Reader mr(module_paths, shared_object_paths, include_paths);
    for (std::vector<const char*>::iterator b = compile_lib_paths->begin(),
                                            e = compile_lib_paths->end();
            b != e;
            ++b) {
        mr.addLib((*b), 0, 0);
    }

    const char *libdrt_path = NULL;
    if (!no_dale_stdlib) {
        if (fopen(DALE_LIBRARY_PATH "/libdrt.so", "r")) {
            libdrt_path = DALE_LIBRARY_PATH "/libdrt.so";
        } else if (fopen("./libdrt.so", "r")) {
            libdrt_path = "./libdrt.so";
        } else {
            fprintf(stderr, "Unable to find libdrt.so.");
            abort();
        }
        mr.addLib(libdrt_path, 0, 0);
    }
    if (!module_name && libdrt_path) {
        shared_object_paths->push_back(libdrt_path);
    }

    Units units(&mr);
    units.cto            = enable_cto;
    units.no_common      = no_common;
    units.no_dale_stdlib = no_dale_stdlib;

    Context *ctx         = NULL;
    llvm::Module *mod    = NULL;
    llvm::Linker *linker = NULL;

    ErrorReporter er("");
    if (module_name) {
        const char *last_slash = strrchr(module_name, '/');
        std::string bare_module_name;
        if (!last_slash) {
            last_slash = module_name;
            bare_module_name = std::string(last_slash);
        } else {
            bare_module_name = std::string(last_slash + 1);
        }
        if (bare_module_name.length() > 0) {
            if (!isValidModuleName(&bare_module_name)) {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidModuleName, NULL,
                    bare_module_name.c_str()
                );
                er.addError(e);
                return 0;
            }
        }
        int diff = last_slash - module_name;
        units.module_name = std::string(module_name);
        units.module_name.replace(diff + 1, 0, "lib");
    }

    for (std::vector<const char*>::iterator b = file_paths->begin(),
                                            e = file_paths->end();
            b != e;
            ++b) {
        const char *filename = *b;
        assert(!units.size());

        Unit *unit = new Unit(filename, &units, &er, nt, tr, NULL, is_x86_64);
        units.push(unit);
        ctx    = unit->ctx;
        mod    = unit->module;
        linker = unit->linker;

        llvm::Triple triple(mod->getTargetTriple());
        if (triple.getTriple().empty()) {
            triple.setTriple(getTriple());
        }
        mod->setDataLayout((is_x86_64) ? x86_64_layout : x86_32_layout);

        llvm::EngineBuilder eb = llvm::EngineBuilder(mod);
        eb.setEngineKind(llvm::EngineKind::JIT);
        ee = eb.create();
        assert(ee);
        ee->InstallLazyFunctionCreator(&lazyFunctionCreator);

        unit->ee = ee;
        unit->mp->ee = ee;

        CommonDecl::addVarargsFunctions(unit);

        if (!no_common) {
            if (no_dale_stdlib) {
                unit->addCommonDeclarations();
            } else {
                std::vector<const char*> import_forms;
                mr.run(ctx, mod, nullNode(), "drt", &import_forms);
                units.top()->mp->setPoolfree();
            }
        }

        std::vector<Node*> nodes;
        for (;;) {
            int error_count = er.getErrorTypeCount(ErrorType::Error);
            Node *top = units.top()->parser->getNextList();
            if (top) {
                nodes.push_back(top);
            }

            if (er.getErrorTypeCount(ErrorType::Error) > error_count) {
                er.flush();
                continue;
            }
            if (!top) {
                er.flush();
                break;
            }

            if (!top->is_token && !top->is_list) {
                units.pop();
                if (!units.empty()) {
                    Unit *unit = units.top();
                    ctx    = unit->ctx;
                    mod    = unit->module;
                    linker = unit->linker;
                    continue;
                }
                break;
            }
            FormTopLevelInstParse(&units, top);
            er.flush();
        }

        if (remove_macros) {
            ctx->eraseLLVMMacros();
        }

        if (dale::pool_free_fptr) {
            ee->freeMachineCodeForFunction(dale::pool_free_fn);
        }

        if (last_module) {
            linkModule(linker, last_module);
        }

        last_module = mod;

        for (std::vector<Node *>::iterator b = nodes.begin(),
                                           e = nodes.end();
                b != e;
                ++b) {
            delete (*b);
        }
    }

    llvm::Triple global_triple(last_module->getTargetTriple());
    if (global_triple.getTriple().empty()) {
        global_triple.setTriple(getTriple());
    }

    std::string Err;
    const llvm::Target *TheTarget = llvm::TargetRegistry::lookupTarget(
                                        global_triple.getTriple(), Err);
    if (TheTarget == 0) {
        fprintf(stderr,
                "Internal error: cannot auto-select target "
                "for module: %s\n", Err.c_str());
        abort();
    }

#if D_LLVM_VERSION_MINOR >= 2
    llvm::TargetOptions target_options;
#endif

    std::string Features;
#if D_LLVM_VERSION_MINOR <= 4
    std::auto_ptr<llvm::TargetMachine> target =
        std::auto_ptr<llvm::TargetMachine>
#else
    std::shared_ptr<llvm::TargetMachine> target =
        std::shared_ptr<llvm::TargetMachine>
#endif
        (TheTarget->createTargetMachine(
            global_triple.getTriple(), llvm::sys::getHostCPUName(),
            Features
#if D_LLVM_VERSION_MINOR >= 2
            , target_options
#endif
         ));

    llvm::TargetMachine &Target = *target.get();

    if (er.getErrorTypeCount(ErrorType::Error)) {
        return 0;
    }

    if (bc_file_paths) {
        for (std::vector<const char*>::iterator
                b = bc_file_paths->begin(),
                e = bc_file_paths->end();
                b != e;
                ++b) {
#if D_LLVM_VERSION_MINOR <= 2
            const llvm::sys::Path bb(*b);
            bool is_native = false;
            if (linker->LinkInFile(bb, is_native)) {
                fprintf(stderr, "Internal error: unable to link "
                        "bitcode file.\n");
                abort();
            }
#else
            llvm::SMDiagnostic err;
            llvm::Module *mymod = llvm::ParseIRFile(*b, err,
                                                    llvm::getGlobalContext());
            std::string errstr;
            if (linker->linkInModule(mymod, &errstr)) {
                fprintf(stderr, "Internal error: unable to link "
                        "bitcode file module: %s.\n", errstr.c_str());
                abort();
            }
#endif
        }
    }

    if (remove_macros) {
        ctx->eraseLLVMMacros();
    }

    llvm::raw_fd_ostream temp(fileno(output_file), false);
    llvm::CodeGenOpt::Level OLvl = llvm::CodeGenOpt::Default;

    /* At optlevel 3, things go quite awry when making libraries,
     * due to the argumentPromotionPass. So set it to 2, unless
     * LTO has also been requested (optlevel == 4). */
    if (optlevel == 3) {
        optlevel = 2;
    }
    int lto = 0;
    if (optlevel == 4) {
        optlevel = 3;
        lto = 1;
    }

    llvm::PassManager PM;
    llvm::PassManagerBuilder PMB;
    PMB.OptLevel = optlevel;

#if D_LLVM_VERSION_MINOR >= 5
    PM.add(new llvm::DataLayoutPass(mod));
#elif D_LLVM_VERSION_MINOR >= 2
    PM.add(new llvm::DataLayout(mod));
#else
    PM.add(new llvm::TargetData(mod));
#endif
    PM.add(llvm::createPostDomTree());
    PMB.DisableUnitAtATime = true;
    if (optlevel > 0) {
        if (lto) {
            PMB.DisableUnitAtATime = false;
        }
        PMB.populateModulePassManager(PM);
        if (lto) {
            PMB.populateLTOPassManager(PM, true, true);
        }
    }

    if (units.module_name.size() > 0) {
        Module::Writer mw(units.module_name, ctx, mod, &PM,
                          &(mr.included_once_tags),
                          &(mr.included_modules),
                          units.cto);
        mw.run();
    } else {
        int rgp = 1;
        std::string err;

        std::map<std::string, llvm::Module *> mdtm_modules;
        if (static_mods_all || (static_module_names->size() > 0)) {
            if (remove_macros) {
                for (std::map<std::string, std::string>::iterator
                        b = dtm_nm_modules.begin(),
                        e = dtm_nm_modules.end();
                        b != e; ++b) {
                    mdtm_modules.insert(
                        std::pair<std::string, llvm::Module*>(
                            b->first,
                            mr.loadModule(&(b->second), true)
                        )
                    );
                }
            } else {
                mdtm_modules = dtm_modules;
            }
            rgp = 0;
        }

        if (static_mods_all) {
            for (std::map<std::string, llvm::Module *>::iterator b =
                        mdtm_modules.begin(), e = mdtm_modules.end();
                    b != e; ++b) {
                if (cto_modules.find(b->first) == cto_modules.end()) {
                    linkModule(linker, b->second);
                }
            }
        } else if (static_module_names->size() > 0) {
            for (std::vector<const char *>::iterator b =
                        static_module_names->begin(), e =
                        static_module_names->end();
                    b != e; ++b) {
                std::map<std::string, llvm::Module *>::iterator
                found = mdtm_modules.find(std::string(*b));
                if (found != mdtm_modules.end()) {
                    linkModule(linker, found->second);
                }
            }
            rgp = 0;
        }

        /* Previously, eraseLLVMMacrosAndCTOFunctions was only run
         * when a module was created, because that was the only time a
         * function could be CTO. It can be CTO at any time now. (The
         * removeMacros part of the call is unnecessary, but shouldn't
         * cause any problems.) */
        if (rgp) {
            ctx->regetPointers(mod);
        }
        if (remove_macros) {
            ctx->eraseLLVMMacrosAndCTOFunctions();
        }

        llvm::formatted_raw_ostream *temp_fro
        = new llvm::formatted_raw_ostream(temp,
            llvm::formatted_raw_ostream::DELETE_STREAM);

        if (produce == IR) {
#if D_LLVM_VERSION_MINOR <= 4
            PM.add(llvm::createPrintModulePass(&temp));
#else
            PM.add(llvm::createPrintModulePass(temp));
#endif
        } else if (produce == ASM) {
            Target.setAsmVerbosityDefault(true);
            bool res = Target.addPassesToEmitFile(
                PM, *temp_fro, llvm::TargetMachine::CGFT_AssemblyFile,
                OLvl, NULL);
            if (res) {
                fprintf(stderr,
                        "Internal error: unable to add passes "
                        "to emit file.\n");
                abort();
            }
        }

        if (debug) {
            mod->dump();
        }
        if (debug) {
            llvm::verifyModule(*mod);
        }
        PM.run(*mod);

        if (produce == BitCode) {
            llvm::WriteBitcodeToFile(mod, temp);
        }

        temp_fro->flush();
        temp.flush();
    }

    if (debug) {
        tr->print();
    }

    return 1;
}
}
