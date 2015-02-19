#include "Generator.h"
#include "Config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <sys/stat.h>

#if D_LLVM_VERSION_MAJOR < 3
#error "LLVM >= 3.0 is required."
#endif

#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Signals.h"
#if D_LLVM_VERSION_MINOR <= 4
#include "llvm/Support/system_error.h"
#else
#include "llvm/Object/Error.h"
#endif
#include "../llvm_LLVMContext.h"
#include "../llvm_Module.h"
#include "llvm/LinkAllPasses.h"
#include "../llvm_Linker.h"
#include "../llvm_Function.h"
#include "../llvm_CallingConv.h"
#include "../llvm_AssemblyPrintModulePass.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "../llvm_ValueSymbolTable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Passes.h"
#include "../llvm_AnalysisVerifier.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/Cloning.h"

#if D_LLVM_VERSION_MINOR >= 3
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#endif

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

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <float.h>
#include FFI_HEADER

#define DALE_DEBUG 0

#define eq(str) !strcmp(t->str_value.c_str(), str)

extern "C" {
    void init_introspection_functions(void);
    void *find_introspection_function(const char *);
}

namespace dale
{

int nesting = 0;

std::vector<llvm::Value *> two_zero_indices;

llvm::Module *mod;

Generator::Generator()
{
    CoreForms::init();

    llvm::InitializeNativeTarget();
    llvm::InitializeAllAsmPrinters();

    nt = new NativeTypes();
    tr = new TypeRegister();

    dtm_modules     = new std::map<std::string, llvm::Module*>;
    dtm_nm_modules  = new std::map<std::string, std::string>;

    two_zero_indices.clear();
    STL::push_back2(&two_zero_indices,
                    nt->getLLVMZero(), nt->getLLVMZero());

    cto_modules = new std::set<std::string>;
}

Generator::~Generator()
{
    delete tr;

    delete cto_modules;
    delete dtm_modules;
    delete dtm_nm_modules;
}

void *myLFC(const std::string &name)
{
    void *fn_pointer = find_introspection_function(name.c_str());
    if (fn_pointer) {
        return fn_pointer;
    }

    if (name[0] != '_') {
        /* Try for the underscored version. */
        std::string temp;
        temp.append("_");
        temp.append(name);

        void *ptr =
            llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(temp);
        if (ptr) {
            return ptr;
        }
    }

    if (DALE_DEBUG) {
        fprintf(stderr,
                "Internal warning: can't find symbol '%s' "
                "in installed lazy function creator.\n",
                name.c_str());
    }

    return NULL;
}

int Generator::run(std::vector<const char *> *filenames,
                   std::vector<const char *> *bc_files,
                   FILE *outfile,
                   int produce,
                   int optlevel,
                   int remove_macros,
                   const char *my_module_name,
                   int no_common,
                   std::vector<std::string> *so_paths,
                   int nostrip,
                   int static_mods_all,
                   std::vector<const char *> *static_modules,
                   std::vector<const char *> *mycto_modules,
                   int enable_cto,
                   int mydebug,
                   int no_dale_stdlib,
                   std::vector<const char *> *compile_libs_sv,
                   std::vector<const char *> *include_paths_sv,
                   std::vector<const char *> *module_paths_sv)
{
    /* On OS X, SYSTEM_PROCESSOR is i386 even when the underlying
     * processor is x86-64, hence the extra check here. */
    bool is_x86_64 =
        ((!strcmp(SYSTEM_PROCESSOR, "x86_64"))
         || ((!strcmp(SYSTEM_PROCESSOR, "amd64")))
         || ((!strcmp(SYSTEM_NAME, "Darwin"))
             && (sizeof(char *) == 8)));

    llvm::ExecutionEngine *ee = NULL;

    init_introspection_functions();

    debug = mydebug;

    cto_modules->clear();
    for (std::vector<const char*>::iterator
            b = mycto_modules->begin(),
            e = mycto_modules->end();
            b != e;
            ++b) {
        cto_modules->insert(std::string(*b));
    }

    llvm::Module *last_module = NULL;

    std::vector<const char *>::iterator iter =
        filenames->begin();

    erep = new ErrorReporter("");

    Module::Reader mr(module_paths_sv, so_paths, include_paths_sv);
    for (std::vector<const char*>::iterator b = compile_libs_sv->begin(),
                                            e = compile_libs_sv->end();
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
    if (!my_module_name && libdrt_path) {
        so_paths->push_back(libdrt_path);
    }

    units = new Units(&mr);
    units->cto = enable_cto;
    units->no_common = no_common;
    units->no_dale_stdlib = no_dale_stdlib;
    Context *ctx    = NULL;
    mod    = NULL;
    llvm::Linker *linker = NULL;

    std::string under_module_name;
    if (my_module_name) {
        const char *last = strrchr(my_module_name, '/');
        if (!last) {
            last = my_module_name;
            under_module_name = std::string(last);
        } else {
            under_module_name = std::string(last + 1);
        }
        int diff = last - my_module_name;
        units->module_name = std::string(my_module_name);
        units->module_name.replace(diff + 1, 0, "lib");
    }

    if (filenames->size() == 0) {
        return 0;
    }

    if (under_module_name.length() > 0) {
        if (!isValidModuleName(&under_module_name)) {
            Error *e = new Error(
                ErrorInst::Generator::InvalidModuleName,
                NULL,
                under_module_name.c_str()
            );
            erep->addError(e);
            return 0;
        }
    }

    while (iter != filenames->end()) {
        const char *filename = (*iter);
        if (units->size()) {
            fprintf(stderr, "units size\n");
            abort();
        }

        Unit *unit = new Unit(filename, units, erep, nt, tr, NULL,
                              is_x86_64);
        units->push(unit);
        ctx    = unit->ctx;
        mod    = unit->module;
        linker = unit->linker;

        llvm::Triple TheTriple(mod->getTargetTriple());
        if (TheTriple.getTriple().empty()) {
#if D_LLVM_VERSION_MINOR >= 2
            TheTriple.setTriple(llvm::sys::getDefaultTargetTriple());
#else
            TheTriple.setTriple(llvm::sys::getHostTriple());
#endif
        }

        /* todo: need some sort of platform table/class. */
        if (is_x86_64) {
            mod->setDataLayout("e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128");
        } else {
            mod->setDataLayout("e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32");
        }

        llvm::EngineBuilder eb = llvm::EngineBuilder(mod);
        eb.setEngineKind(llvm::EngineKind::JIT);
        ee = eb.create();
        assert(ee);

        unit->ee = ee;
        unit->mp->ee = ee;

        ee->InstallLazyFunctionCreator(myLFC);
        CommonDecl::addVarargsFunctions(unit);

        if (!units->no_common) {
            if (units->no_dale_stdlib) {
                unit->addCommonDeclarations();
            } else {
                std::vector<const char*> import_forms;
                mr.run(ctx, mod, nullNode(), "drt", &import_forms);
                units->top()->mp->setPoolfree();
            }
        }
        int error_count = 0;

        std::vector<Node*> nodes;

        do {
            error_count =
                erep->getErrorTypeCount(ErrorType::Error);

            Node *top = units->top()->parser->getNextList();
            if (top) {
                nodes.push_back(top);
            }

            if (erep->getErrorTypeCount(ErrorType::Error) > error_count) {
                erep->flush();
                continue;
            }
            if (!top) {
                erep->flush();
                break;
            }

            /* EOF. */
            if (!top->is_token && !top->is_list) {
                units->pop();
                if (!units->empty()) {
                    Unit *unit = units->top();
                    ctx    = unit->ctx;
                    mod    = unit->module;
                    linker = unit->linker;
                    continue;
                }
                break;
            }
            FormTopLevelInstParse(units, top);
            erep->flush();
        } while (1);

        if (remove_macros) {
            ctx->eraseLLVMMacros();
        }

        if (dale::pool_free_fptr) {
            ee->freeMachineCodeForFunction(dale::pool_free_fn);
        }

        if (last_module) {
            std::string link_error;
#if D_LLVM_VERSION_MINOR <= 2
            if (linker->LinkInModule(last_module, &link_error)) {
#else
            if (linker->linkInModule(last_module, &link_error)) {
#endif
                Error *e = new Error(
                    ErrorInst::Generator::CannotLinkModules,
                    new Node()
                );
                e->addArgString(link_error.c_str());
                erep->addError(e);
                erep->flush();
                break;
            }
        }

        last_module = mod;

        ++iter;

        for (std::vector<Node *>::iterator b = nodes.begin(),
                                           e = nodes.end();
                b != e;
                ++b) {
            delete (*b);
        }
    }

    llvm::Triple GTheTriple(last_module->getTargetTriple());
    if (GTheTriple.getTriple().empty()) {
#if D_LLVM_VERSION_MINOR >= 2
        GTheTriple.setTriple(llvm::sys::getDefaultTargetTriple());
#else
        GTheTriple.setTriple(llvm::sys::getHostTriple());
#endif
    }

    std::string Err;
    const llvm::Target *TheTarget = llvm::TargetRegistry::lookupTarget(
                                        GTheTriple.getTriple(), Err);
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
            GTheTriple.getTriple(), llvm::sys::getHostCPUName(),
            Features
#if D_LLVM_VERSION_MINOR >= 2
            , target_options
#endif
         ));

    llvm::TargetMachine &Target = *target.get();

    if (erep->getErrorTypeCount(ErrorType::Error)) {
        return 0;
    }

    if (bc_files) {
        for (std::vector<const char*>::iterator
                b = bc_files->begin(),
                e = bc_files->end();
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

    llvm::raw_fd_ostream temp(fileno(outfile), false);
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

    if (units->module_name.size() > 0) {
        Module::Writer mw(units->module_name, ctx, mod, &PM,
                          &(mr.included_once_tags),
                          &(mr.included_modules),
                          units->cto);
        mw.run();
    } else {
        int rgp = 1;
        std::string err;

        std::map<std::string, llvm::Module *> mdtm_modules;
        if (static_mods_all || (static_modules->size() > 0)) {
            if (remove_macros) {
                for (std::map<std::string, std::string>::iterator
                        b = dtm_nm_modules->begin(),
                        e = dtm_nm_modules->end();
                        b != e; ++b) {
                    mdtm_modules.insert(
                        std::pair<std::string, llvm::Module*>(
                            b->first,
                            mr.loadModule(&(b->second), true)
                        )
                    );
                }
            } else {
                mdtm_modules = *dtm_modules;
            }
            rgp = 0;
        }

        if (static_mods_all) {
            for (std::map<std::string, llvm::Module *>::iterator b =
                        mdtm_modules.begin(), e = mdtm_modules.end();
                    b != e; ++b) {
                if (cto_modules->find(b->first) == cto_modules->end()) {
#if D_LLVM_VERSION_MINOR <= 2
                    if (linker->LinkInModule(b->second, &err)) {
#else
                    if (linker->linkInModule(b->second, &err)) {
#endif
                        fprintf(stderr,
                                "Internal error: unable to link "
                                "dale module: %s\n",
                                err.c_str());
                        return 0;
                    }
                }
            }
        } else if (static_modules->size() > 0) {
            for (std::vector<const char *>::iterator b =
                        static_modules->begin(), e =
                        static_modules->end();
                    b != e; ++b) {
                std::map<std::string, llvm::Module *>::iterator
                found = mdtm_modules.find(std::string(*b));
                if (found != mdtm_modules.end()) {
#if D_LLVM_VERSION_MINOR <= 2
                    if (linker->LinkInModule(found->second, &err)) {
#else
                    if (linker->linkInModule(found->second, &err)) {
#endif
                        fprintf(stderr,
                                "Internal error: unable to link "
                                "dale module: %s\n",
                                err.c_str());
                        return 0;
                    }
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

        if (DALE_DEBUG) {
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

    if (DALE_DEBUG) {
        tr->print();
    }

    return 1;
}
}
