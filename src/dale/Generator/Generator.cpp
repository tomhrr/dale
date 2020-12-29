#include "Generator.h"
#include "Config.h"

#include <float.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#if D_LLVM_VERSION_ORD <= 34
#include "llvm/Support/system_error.h"
#else
#include "llvm/Object/Error.h"
#endif
#if D_LLVM_VERSION_ORD >= 33
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#endif

#include "../llvmUtils/llvmUtils.h"
#include "../llvm_AnalysisVerifier.h"
#include "../llvm_AssemblyPrintModulePass.h"
#include "../llvm_CallingConv.h"
#include "../llvm_Function.h"
#include "../llvm_IRBuilder.h"
#include "../llvm_LLVMContext.h"
#include "../llvm_Linker.h"
#include "../llvm_Module.h"
#include "../llvm_PassManager.h"
#include "../llvm_ValueSymbolTable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#if D_LLVM_VERSION_ORD <= 35
#include "llvm/ExecutionEngine/JIT.h"
#else
#include "llvm/ExecutionEngine/MCJIT.h"
#endif
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include "../CommonDecl/CommonDecl.h"
#include "../Context/Context.h"
#include "../CoreForms/CoreForms.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../ErrorType/ErrorType.h"
#include "../Form/TopLevel/Inst/Inst.h"
#include "../Module/Writer/Writer.h"
#include "../NativeTypes/NativeTypes.h"
#include "../Node/Node.h"
#include "../STL/STL.h"
#include "../TypeRegister/TypeRegister.h"
#include "../Unit/Unit.h"
#include "../Units/Units.h"
#include "../Utils/Utils.h"
#include "../BasicTypes/BasicTypes.h"

extern "C" {
void init_introspection_functions();
void *find_introspection_function(const char *);
}

namespace dale {
Generator::Generator() {
    CoreForms::init();

    llvm::InitializeNativeTarget();
    llvm::InitializeAllAsmPrinters();
}

Generator::~Generator() {}

int Generator::run(std::vector<const char *> *file_paths,
                   std::vector<const char *> *bc_file_paths,
                   std::vector<const char *> *compile_lib_paths,
                   std::vector<const char *> *include_paths,
                   std::vector<const char *> *module_paths,
                   std::vector<const char *> *static_module_names,
                   const char *module_name, int debug, int produce,
                   int optlevel, int remove_macros, int no_common,
                   int no_drt, int no_arithmetic,
                   int static_mods_all,
                   int enable_cto, int print_expansions,
                   std::vector<std::string> *shared_object_paths,
                   FILE *output_file) {
    if (!file_paths->size()) {
        return 0;
    }

    NativeTypes nt;
    TypeRegister tr;
    llvm::ExecutionEngine *ee = NULL;

    /* On OS X, SYSTEM_PROCESSOR is i386 even when the underlying
     * processor is x86-64, hence the extra check here. */
    bool is_x86_64 =
        ((!strcmp(SYSTEM_PROCESSOR, "x86_64")) ||
         ((!strcmp(SYSTEM_PROCESSOR, "amd64"))) ||
         ((!strcmp(SYSTEM_NAME, "Darwin")) && (sizeof(char *) == 8)));

    init_introspection_functions();

    llvm::Module *last_module = NULL;

    if (!no_arithmetic) {
        static_module_names->push_back("arithmetic");
    }

    Module::Reader mr(module_paths, shared_object_paths, include_paths,
                      static_module_names, static_mods_all,
                      remove_macros);
    for (std::vector<const char *>::iterator
             b = compile_lib_paths->begin(),
             e = compile_lib_paths->end();
         b != e; ++b) {
        mr.addDynamicLibrary((*b), false, false);
    }

    const char *libarithmetic_path = NULL;
    const char *libdrt_path = NULL;
    if (!no_arithmetic) {
        FILE *arithmetic_file = NULL;
        if ((arithmetic_file = fopen(DALE_LIBRARY_PATH "/libarithmetic.so", "r"))) {
            libarithmetic_path = DALE_LIBRARY_PATH "/libarithmetic.so";
        } else if ((arithmetic_file = fopen("./libarithmetic.so", "r"))) {
            libarithmetic_path = "./libarithmetic.so";
        } else {
            error("unable to find libarithmetic.so");
        }
        mr.addDynamicLibrary(libarithmetic_path, false, false);
        int res = fclose(arithmetic_file);
        if (res != 0) {
            error("unable to close %s", libarithmetic_path, true);
        }
    }
    if (!no_drt) {
        FILE *drt_file = NULL;
        if ((drt_file = fopen(DALE_LIBRARY_PATH "/libdrt.so", "r"))) {
            libdrt_path = DALE_LIBRARY_PATH "/libdrt.so";
        } else if ((drt_file = fopen("./libdrt.so", "r"))) {
            libdrt_path = "./libdrt.so";
        } else {
            error("unable to find libdrt.so");
        }
        mr.addDynamicLibrary(libdrt_path, false, false);
        int res = fclose(drt_file);
        if (res != 0) {
            error("unable to close %s", libdrt_path, true);
        }
    }
    if (!module_name && libarithmetic_path) {
        shared_object_paths->push_back(libarithmetic_path);
    }
    if (!module_name && libdrt_path) {
        shared_object_paths->push_back(libdrt_path);
    }

    Units units(&mr);
    units.cto = enable_cto;
    units.no_common = no_common;
    units.no_drt = no_drt;
    units.print_expansions = print_expansions;
    units.debug = debug;

    Context *ctx = NULL;
    llvm::Module *mod = NULL;
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
                Error *e = new Error(ErrorInst::InvalidModuleName, NULL,
                                     bare_module_name.c_str());
                er.addError(e);
                return 0;
            }
        }
        int diff = last_slash - module_name;
        units.module_name = std::string(module_name);
        units.module_name.replace(diff + 1, 0, "lib");
    }

    for (std::vector<const char *>::iterator b = file_paths->begin(),
                                             e = file_paths->end();
         b != e; ++b) {
        const char *filename = *b;
        assert(!units.size());

        Unit *unit = new Unit(filename, &units, &er, &nt, &tr, NULL,
                              is_x86_64, NULL, NULL, NULL, NULL, NULL);
        units.push(unit);
        ctx = unit->ctx;
        mod = unit->module;
        linker = unit->linker;

        llvm::Triple triple(mod->getTargetTriple());
        if (triple.getTriple().empty()) {
            triple.setTriple(getTriple());
        }

        setDataLayout(mod, is_x86_64);
        DECLARE_ENGINE_BUILDER(mod, eb);

        eb.setEngineKind(llvm::EngineKind::JIT);
        std::string error;
        eb.setErrorStr(&error);
        ee = eb.create();
        if (!ee) {
            fprintf(stderr, "Unable to create execution engine: %s\n",
                    error.c_str());
            abort();
        }
        ee->InstallLazyFunctionCreator(&lazyFunctionCreator);

        unit->ee = ee;
        unit->mp->ee = ee;

        CommonDecl::addVarargsFunctions(unit);

        if (!no_common) {
            if (no_arithmetic) {
		CommonDecl::addBasicTypes(unit, is_x86_64);
            } else if (no_drt) {
                unit->addCommonDeclarations();
            } else {
                std::vector<const char *> import_forms;
                mr.run(ctx, linker, mod, nullNode(), "arithmetic",
                       &import_forms);
                mr.run(ctx, linker, mod, nullNode(), "drt",
                       &import_forms);
                units.top()->mp->setPoolfree();
            }
        }

        std::vector<Node *> nodes;
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
                    ctx = unit->ctx;
                    mod = unit->module;
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

        if (last_module) {
            linkModule(linker, last_module);
        }

        last_module = mod;

        for (std::vector<Node *>::iterator b = nodes.begin(),
                                           e = nodes.end();
             b != e; ++b) {
            delete (*b);
        }
    }

    if (remove_macros) {
        ctx->eraseLLVMMacros();
    }

    if (er.getErrorTypeCount(ErrorType::Error)) {
        return 0;
    }

    if (bc_file_paths) {
        for (std::vector<const char *>::iterator
                 b = bc_file_paths->begin(),
                 e = bc_file_paths->end();
             b != e; ++b) {
            linkFile(linker, *b);
        }
    }

    /* At optlevel 3, things go quite awry when making libraries, due
     * to the argumentPromotionPass. So set it to 2, unless LTO has
     * also been requested (optlevel == 4). */
    bool lto = false;
    if (optlevel == 3) {
        optlevel = 2;
    } else if (optlevel == 4) {
        optlevel = 3;
        lto = true;
    }

    llvm::TargetMachine *target_machine = getTargetMachine(last_module);
    llvm::raw_fd_ostream ostream(fileno(output_file), false);

    PassManager pass_manager;
    addDataLayout(&pass_manager, mod);
    pass_manager.add(llvm::createPostDomTree());

    llvm::PassManagerBuilder pass_manager_builder;
    pass_manager_builder.OptLevel = optlevel;
#if D_LLVM_VERSION_ORD <= 80
    pass_manager_builder.DisableUnitAtATime = true;
#endif
    pass_manager_builder.Inliner = llvm::createFunctionInliningPass();
    pass_manager.add(llvm::createFunctionInliningPass());

    if (optlevel > 0) {
        if (lto) {
#if D_LLVM_VERSION_ORD <= 80
            pass_manager_builder.DisableUnitAtATime = false;
#endif
        }
        pass_manager_builder.populateModulePassManager(pass_manager);
        if (lto) {
            populateLTOPassManager(&pass_manager_builder,
                                   &pass_manager);
        }
    }

    ctx->regetPointers(mod);
    if (!no_arithmetic) {
        dale::BasicTypes::initBasicTypeFormStrings();
        for (std::vector<std::string>::iterator
                b = dale::BasicTypes::basic_type_form_strings.begin(),
                e = dale::BasicTypes::basic_type_form_strings.end();
                b != e;
                ++b) {
            llvm::Function *fn = mod->getFunction(*b);
            if (fn->size()) {
                fn->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));
            }
        }
    }

    if (units.module_name.size() > 0) {
        Module::Writer mw(units.module_name, ctx, mod, &pass_manager,
                          &(mr.included_once_tags),
                          &(mr.included_modules), units.cto);
        mw.run();
        return 1;
    }

    if (remove_macros) {
        ctx->eraseLLVMMacrosAndCTOFunctions();
    }
    if (lto) {
        for (llvm::Module::iterator b = mod->begin(),
                                    e = mod->end();
                b != e;
                ++b) {
            if (b->size()) {
                b->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));
            }
        }
        for (llvm::Module::global_iterator b = mod->global_begin(),
                                           e = mod->global_end();
                b != e;
                ++b) {
            if (b->hasInitializer()) {
                b->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));
            }
        }
        llvm::Function *main = mod->getFunction("main");
        main->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_C));
    }

    llvm_formatted_ostream *ostream_formatted =
        getFormattedOstream(&ostream);

    if (produce == IR) {
        addPrintModulePass(&pass_manager, &ostream);
    } else if (produce == ASM) {
        llvm::CodeGenOpt::Level level = llvm::CodeGenOpt::Default;
#if D_LLVM_VERSION_ORD <= 60
        bool res = target_machine->addPassesToEmitFile(
            pass_manager, *ostream_formatted,
            llvm::TargetMachine::CGFT_AssemblyFile, level, NULL);
#elif D_LLVM_VERSION_ORD <= 90
        bool res = target_machine->addPassesToEmitFile(
            pass_manager, *ostream_formatted, NULL,
            llvm::TargetMachine::CGFT_AssemblyFile, level, NULL);
#else
        bool res = target_machine->addPassesToEmitFile(
            pass_manager, *ostream_formatted, NULL,
            llvm::CGFT_AssemblyFile, level, NULL);
#endif
        assert(!res && "unable to add passes to emit file");
        _unused(res);
    }

    if (debug) {
        moduleDebugPass(mod);
    }

    pass_manager.run(*mod);

    if (produce == BitCode) {
#if D_LLVM_VERSION_ORD <= 60
        llvm::WriteBitcodeToFile(mod, ostream);
#else
        llvm::WriteBitcodeToFile(*mod, ostream);
#endif
    }

    ostream_formatted->flush();
    ostream.flush();

    module_paths->clear();
    std::copy(mr.module_directory_paths.begin(),
              mr.module_directory_paths.end(),
              back_inserter(*module_paths));

    return 1;
}
}
