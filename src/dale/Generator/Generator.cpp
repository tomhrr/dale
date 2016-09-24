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
#include "../llvm_IRBuilder.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Signals.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include "../Units/Units.h"
#include "../Utils/Utils.h"
#include "../Context/Context.h"
#include "../Node/Node.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../TypeRegister/TypeRegister.h"
#include "../ErrorType/ErrorType.h"
#include "../STL/STL.h"
#include "../NativeTypes/NativeTypes.h"
#include "../Module/Writer/Writer.h"
#include "../Form/TopLevel/Inst/Inst.h"
#include "../Unit/Unit.h"
#include "../CoreForms/CoreForms.h"
#include "../CommonDecl/CommonDecl.h"

static const char *x86_64_layout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128";
static const char *x86_32_layout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32";

extern "C" {
    void init_introspection_functions();
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

    assert(false && "unable to find symbol in LFC");
    return NULL;
}

std::string
getTriple()
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
    assert(!result && "unable to link module");
    _unused(result);
}

void
addDataLayout(llvm::PassManager *pass_manager, llvm::Module *mod)
{
#if D_LLVM_VERSION_MINOR >= 5
    pass_manager->add(new llvm::DataLayoutPass(mod));
#elif D_LLVM_VERSION_MINOR >= 2
    pass_manager->add(new llvm::DataLayout(mod));
#else
    pass_manager->add(new llvm::TargetData(mod));
#endif
}

void
addPrintModulePass(llvm::PassManager *pass_manager,
                   llvm::raw_fd_ostream *ostream)
{
#if D_LLVM_VERSION_MINOR <= 4
    pass_manager->add(llvm::createPrintModulePass(ostream));
#else
    pass_manager->add(llvm::createPrintModulePass(*ostream));
#endif
}

#if D_LLVM_VERSION_MINOR <= 4
std::auto_ptr<llvm::TargetMachine> target_sp;
#else
std::shared_ptr<llvm::TargetMachine> target_sp;
#endif
llvm::TargetMachine *
getTargetMachine(llvm::Module *last_module)
{
    llvm::Triple triple(last_module->getTargetTriple());
    if (triple.getTriple().empty()) {
        triple.setTriple(getTriple());
    }

    std::string Err;
    const llvm::Target *target =
        llvm::TargetRegistry::lookupTarget(triple.getTriple(), Err);
    assert(target && "cannot auto-select target for module");

#if D_LLVM_VERSION_MINOR >= 2
    llvm::TargetOptions target_options;
#endif

    std::string Features;
    target_sp =
#if D_LLVM_VERSION_MINOR <= 4
        std::auto_ptr<llvm::TargetMachine>
#else
        std::shared_ptr<llvm::TargetMachine>
#endif
        (target->createTargetMachine(
            triple.getTriple(), llvm::sys::getHostCPUName(),
            Features
#if D_LLVM_VERSION_MINOR >= 2
            , target_options
#endif
        ));

    return target_sp.get();
}

int
Generator::run(std::vector<const char *> *file_paths,
               std::vector<const char *> *bc_file_paths,
               std::vector<const char *> *compile_lib_paths,
               std::vector<const char *> *include_paths,
               std::vector<const char *> *module_paths,
               std::vector<const char *> *static_module_names,
               const char *module_name,
               int debug,
               int produce,
               int optlevel,
               int remove_macros,
               int no_common,
               int no_dale_stdlib,
               int static_mods_all,
               int enable_cto,
               int print_expansions,
               std::vector<std::string> *shared_object_paths,
               FILE *output_file)
{
    if (!file_paths->size()) {
        return 0;
    }

    NativeTypes nt;
    TypeRegister tr;
    llvm::ExecutionEngine *ee = NULL;

    /* On OS X, SYSTEM_PROCESSOR is i386 even when the underlying
     * processor is x86-64, hence the extra check here. */
    bool is_x86_64 =
        ((!strcmp(SYSTEM_PROCESSOR, "x86_64"))
         || ((!strcmp(SYSTEM_PROCESSOR, "amd64")))
         || ((!strcmp(SYSTEM_NAME, "Darwin"))
             && (sizeof(char *) == 8)));

    init_introspection_functions();

    llvm::Module *last_module = NULL;

    Module::Reader mr(module_paths, shared_object_paths, include_paths,
                      static_module_names, static_mods_all,
                      remove_macros);
    for (std::vector<const char*>::iterator b = compile_lib_paths->begin(),
                                            e = compile_lib_paths->end();
            b != e;
            ++b) {
        mr.addDynamicLibrary((*b), false, false);
    }

    const char *libdrt_path = NULL;
    if (!no_dale_stdlib) {
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
    if (!module_name && libdrt_path) {
        shared_object_paths->push_back(libdrt_path);
    }

    Units units(&mr);
    units.cto              = enable_cto;
    units.no_common        = no_common;
    units.no_dale_stdlib   = no_dale_stdlib;
    units.print_expansions = print_expansions;
    units.debug            = debug;

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
                    ErrorInst::InvalidModuleName, NULL,
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

        Unit *unit = new Unit(filename, &units, &er, &nt, &tr, NULL,
                              is_x86_64, NULL, NULL, NULL, NULL, NULL);
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
        //eb.setEngineKind(llvm::EngineKind::Interpreter);
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
                mr.run(ctx, linker, mod, nullNode(), "drt", &import_forms);
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

    if (remove_macros) {
        ctx->eraseLLVMMacros();
    }

    if (er.getErrorTypeCount(ErrorType::Error)) {
        return 0;
    }

    if (bc_file_paths) {
        for (std::vector<const char*>::iterator b = bc_file_paths->begin(),
                                                e = bc_file_paths->end();
                b != e;
                ++b) {
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

    llvm::PassManager pass_manager;
    addDataLayout(&pass_manager, mod);
    pass_manager.add(llvm::createPostDomTree());

    llvm::PassManagerBuilder pass_manager_builder;
    pass_manager_builder.OptLevel = optlevel;
    pass_manager_builder.DisableUnitAtATime = true;

    if (optlevel > 0) {
        if (lto) {
            pass_manager_builder.DisableUnitAtATime = false;
        }
        pass_manager_builder.populateModulePassManager(pass_manager);
        if (lto) {
            pass_manager_builder.populateLTOPassManager(pass_manager, true, true);
        }
    }

    if (units.module_name.size() > 0) {
        Module::Writer mw(units.module_name, ctx, mod, &pass_manager,
                          &(mr.included_once_tags), &(mr.included_modules),
                          units.cto);
        mw.run();
        return 1;
    }

    ctx->regetPointers(mod);
    if (remove_macros) {
        ctx->eraseLLVMMacrosAndCTOFunctions();
    }

    llvm::formatted_raw_ostream *ostream_formatted =
        new llvm::formatted_raw_ostream(
            ostream,
            llvm::formatted_raw_ostream::DELETE_STREAM
        );

    if (produce == IR) {
        addPrintModulePass(&pass_manager, &ostream);
    } else if (produce == ASM) {
        target_machine->setAsmVerbosityDefault(true);
        llvm::CodeGenOpt::Level level = llvm::CodeGenOpt::Default;
        bool res = target_machine->addPassesToEmitFile(
            pass_manager, *ostream_formatted,
            llvm::TargetMachine::CGFT_AssemblyFile,
            level, NULL);
        assert(!res && "unable to add passes to emit file");
        _unused(res);
    }

    if (debug) {
        mod->dump();
#if D_LLVM_VERSION_MINOR >= 5
        if (llvm::verifyModule(*mod, &(llvm::errs()))) {
            abort();
        }
#endif
    }

    pass_manager.run(*mod);

    if (produce == BitCode) {
        llvm::WriteBitcodeToFile(mod, ostream);
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
