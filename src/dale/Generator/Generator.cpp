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

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <setjmp.h>
#include <float.h>
#include FFI_HEADER

#define DALE_DEBUG 0

#define IMPLICIT 1

#define eq(str) !strcmp(t->str_value.c_str(), str)

extern "C" {
    void init_introspection_functions(void);
    void *find_introspection_function(const char *);
}

namespace dale
{

int nesting = 0;

std::vector<llvm::Value *> two_zero_indices;

char *mod_paths[100];
int mod_path_count = 0;

static int var_count = 0;
int Generator::getUnusedVarname(std::string *mystr)
{
    char buf[256];
    do {
        mystr->clear();
        sprintf(buf, "%d", var_count++);
        mystr->append("_dv");
        mystr->append(buf);
    } while (mod->getGlobalVariable(llvm::StringRef(*mystr)));

    return 1;
}

Generator::Generator()
{
    CoreForms::init();

    prefunction_ns = NULL;
    inc_path_count = 0;

    llvm::InitializeNativeTarget();
    llvm::InitializeAllAsmPrinters();

    nt = new NativeTypes();
    tr = new TypeRegister();

    included_inodes    = new std::multiset<ino_t>;
    included_once_tags = new std::set<std::string>;
    included_modules   = new std::set<std::string>;
    set_module_name    = 0;

    dtm_modules     = new std::map<std::string, llvm::Module*>;
    dtm_nm_modules  = new std::map<std::string, std::string>;

    two_zero_indices.clear();
    STL::push_back2(&two_zero_indices,
                    nt->getLLVMZero(), nt->getLLVMZero());

    /* On OS X, SYSTEM_PROCESSOR is i386 even when the underlying
     * processor is x86-64, hence the extra check here. */
    is_x86_64 =
        ((!strcmp(SYSTEM_PROCESSOR, "x86_64"))
         || ((!strcmp(SYSTEM_PROCESSOR, "amd64")))
         || ((!strcmp(SYSTEM_NAME, "Darwin"))
             && (sizeof(char *) == 8)));

    has_defined_extern_macro = 0;

    cto_modules = new std::set<std::string>;
}

Generator::~Generator()
{
    delete ctx;
    delete prsr;
    delete tr;
    delete included_inodes;
    delete included_once_tags;
    delete included_modules;

    delete cto_modules;
    delete dtm_modules;
    delete dtm_nm_modules;
}

llvm::Module *loadModule(std::string *path, bool materialize)
{
#if D_LLVM_VERSION_MINOR <= 4
    llvm::OwningPtr<llvm::MemoryBuffer> buffer;
#endif

#if D_LLVM_VERSION_MINOR <= 3
    const llvm::sys::Path sys_path(*path);
    llvm::MemoryBuffer::getFileOrSTDIN(sys_path.c_str(), buffer);
#elif D_LLVM_VERSION_MINOR <= 4
    llvm::MemoryBuffer::getFileOrSTDIN(*path, buffer);
#else
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> eo =
        llvm::MemoryBuffer::getFileOrSTDIN(*path);
    if (eo.getError()) {
        fprintf(stderr, 
                "Internal error: cannot load module: %s\n", 
                eo.getError().message().c_str());
        abort();
    }
    std::unique_ptr<llvm::MemoryBuffer> buffer = std::move(eo.get());
#endif

#if D_LLVM_VERSION_MINOR <= 4
    std::string errmsg;
    llvm::Module *module = 
        llvm::getLazyBitcodeModule(buffer.get(),
                                   llvm::getGlobalContext(),
                                   &errmsg);
#else
    std::string errmsg;
    llvm::ErrorOr<llvm::Module *> err =
        llvm::getLazyBitcodeModule(buffer.get(),
                                   llvm::getGlobalContext());
    llvm::Module *module = err.get();
    if (!module) {
        errmsg = err.getError().message();
    }
    buffer.release();
#endif

    if (!module) {
        fprintf(stderr,
                "Internal error: cannot load module: %s\n",
                errmsg.c_str());
        abort();
    }

    std::string ma_error;
#if D_LLVM_VERSION_MINOR <= 4
    bool ma = module->MaterializeAll(&ma_error);
#else
    std::error_code ec = module->materializeAllPermanently();
    bool ma = (bool) ec;
    if (ec) {
        ma_error = ec.message();
    }
#endif
    if (ma) {
        fprintf(stderr, "Internal error: failed to materialize "
                        "module (%s): %s\n",
                        path->c_str(), ma_error.c_str());
        abort();
    }

    return module;
}

int Generator::addIncludePath(char *filename)
{
    inc_paths[inc_path_count++] = filename;
    return 1;
}

int Generator::addModulePath(char *filename)
{
    mod_paths[mod_path_count++] = filename;
    return 1;
}

static int added_common_declarations = 0;

void Generator::addCommonDeclarations(void)
{
    CommonDecl::addBasicTypes(unit_stack->top(), is_x86_64);

    /* The basic math functions and the varargs functions are
     * added to every module, but the structs are not, because
     * they can merge backwards and forwards (the other stuff has
     * internal linkage). */

    if (added_common_declarations) {
        return;
    }
    added_common_declarations = 1;

    CommonDecl::addVarargsTypes(unit_stack->top(), is_x86_64);
    CommonDecl::addStandardVariables(unit_stack->top());

    return;
}

static int mc_count   = 0;
static int mcpn_count = 0;
static int globmarker = 0;

void Generator::removeMacroTemporaries(void)
{
    /* Remove macro calls using mc_count. */

    std::string name;
    char buf[20];

    while (mc_count > 0) {
        mc_count--;

        name.clear();
        name.append("_dale_TempMacroExecution");
        sprintf(buf, "%d", mc_count);
        name.append(buf);

        llvm::Function *fn =
            mod->getFunction((const char *) name.c_str());

        if (fn) {
            fn->eraseFromParent();
        } else {
            fprintf(stderr,
                    "Internal error: tried to remove function '%s', "
                    "but it could not be found.\n",
                    name.c_str());
            abort();
        }
    }

    while (mcpn_count > 0) {
        mcpn_count--;

        name.clear();
        name.append("_dale_TempMacroPVar");
        sprintf(buf, "%d", mcpn_count);
        name.append(buf);

        llvm::GlobalVariable *g =
            mod->getGlobalVariable((const char *) name.c_str(), true);

        if (g) {
            g->eraseFromParent();
        } else {
            fprintf(stderr,
                    "Internal error: tried to remove variable '%s', "
                    "but it could not be found.\n",
                    name.c_str());
            abort();
        }
    }

    /* Remove strings used for macro calls. */

    while (globmarker > 0) {
        globmarker--;

        name.clear();
        name.append("_dale_");
        sprintf(buf, "%d", globmarker);
        name.append(buf);

        llvm::GlobalVariable *g =
            mod->getGlobalVariable((const char *) name.c_str());

        if (g) {
            g->eraseFromParent();
        } else {
            fprintf(stderr,
                    "Internal error: tried to remove variable '%s', "
                    "but it could not be found.\n",
                    name.c_str());
            abort();
        }
    }
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
                   char *my_module_name,
                   int no_acd,
                   std::vector<std::string> *so_paths,
                   int nostrip,
                   int static_mods_all,
                   std::vector<const char *> *static_modules,
                   std::vector<const char *> *mycto_modules,
                   int enable_cto,
                   int mydebug,
                   int nodrt)
{
    init_introspection_functions();

    has_defined_extern_macro = 0;
    no_add_common_declarations = no_acd;
    no_drt = nodrt;

    debug = mydebug;

    const char *libdrt_path = NULL;
    if (!nodrt) {
        if (fopen(DALE_LIBRARY_PATH "/libdrt.so", "r")) {
            libdrt_path = DALE_LIBRARY_PATH "/libdrt.so";
        } else if (fopen("./libdrt.so", "r")) {
            libdrt_path = "./libdrt.so";
        } else {
            fprintf(stderr, "Unable to find libdrt.so.");
            abort();
        }
        addLib(libdrt_path, 0, 0);
    }

    cto = enable_cto;

    cto_modules->clear();
    for (std::vector<const char*>::iterator
            b = mycto_modules->begin(),
            e = mycto_modules->end();
            b != e;
            ++b) {
        cto_modules->insert(std::string(*b));
    }

    so_paths_g = so_paths;
    if (!my_module_name && libdrt_path) {
        so_paths_g->push_back(libdrt_path);
    }

    std::string under_module_name;
    if (my_module_name) {
        char *last = strrchr(my_module_name, '/');
        if (!last) {
            last = my_module_name;
            under_module_name = std::string(last);
        } else {
            under_module_name = std::string(last + 1);
        }
        int diff = last - my_module_name;
        module_name = std::string(my_module_name);
        module_name.replace(diff + 1, 0, "lib");
    }

    if (filenames->size() == 0) {
        return 0;
    }

    llvm::Module *last_module = NULL;

    std::vector<const char *>::iterator iter =
        filenames->begin();

    erep = new ErrorReporter("");

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

        Unit *unit = new Unit(filename, this, erep, nt, tr, NULL);
        unit_stack = new UnitStack(unit);

        ctx    = unit->ctx;
        mod    = unit->module;
        linker = unit->linker;
        prsr   = unit->parser;
        current_once_tag.clear();

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

        if (!no_acd) {
            if (nodrt) {
                addCommonDeclarations();
            } else {
                std::vector<const char*> import_forms;
                addDaleModule(nullNode(), "drt", &import_forms);
                getUnit()->mp->setPoolfree();
            }
        }
        int error_count = 0;

        std::vector<Node*> nodes;

        do {
            error_count =
                erep->getErrorTypeCount(ErrorType::Error);

            Node *top = prsr->getNextList();
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
                unit_stack->pop();
                if (!unit_stack->empty()) {
                    Unit *unit = unit_stack->top();
                    ctx    = unit->ctx;
                    mod    = unit->module;
                    linker = unit->linker;
                    prsr   = unit->parser;
                    current_once_tag = unit->once_tag;
                    continue;
                }
                break;
            }
            FormTopLevelInstParse(this, top);
            erep->flush();
        } while (1);

        removeMacroTemporaries();

        if (remove_macros) {
            ctx->eraseLLVMMacros();
            has_defined_extern_macro = 0;
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
        has_defined_extern_macro = 0;
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

    if (module_name.size() > 0) {
        Module::Writer mw(module_name, ctx, mod, &PM,
                          included_once_tags, included_modules,
                          cto);
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
                            loadModule(&(b->second), true)
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

bool Generator::addLib(const char *lib_path,
                       int add_to_so_paths,
                       int add_nm_to_so_paths)
{
    std::string temp;

    bool res =
        llvm::sys::DynamicLibrary::LoadLibraryPermanently(
            lib_path,
            &temp
        );
    if (res) {
        /* If this is Darwin, and .so is at the end of lib_path, try
         * replacing it with dylib. */
        if (!strcmp(SYSTEM_NAME, "Darwin")) {
            int len = strlen(lib_path);
            if ((len >= 3) && !strcmp((lib_path + len - 3), ".so")) {
                char lib_path_dylib[256];
                strcpy(lib_path_dylib, lib_path);
                strcpy((lib_path_dylib + len - 2),
                       "dylib");
                bool res =
                    llvm::sys::DynamicLibrary::LoadLibraryPermanently(
                        lib_path_dylib,
                        &temp
                    );
                if (!res) {
                    /* This only generates SOs - the dylib stuff
                     * is only for internal Mac libraries, not for
                     * dale modules. */
                    goto done;
                }
            }
        }
        fprintf(stderr,
                "Internal error: unable to load library %s\n",
                temp.c_str());
        return false;
    } else {
        if (add_nm_to_so_paths) {
            std::string temp;
            temp.append(lib_path);
            temp.erase((temp.size() - 3), 3);
            temp.append("-nomacros.so");
            so_paths_g->push_back(temp);
        } else if (add_to_so_paths) {
            std::string temp;
            temp.append(lib_path);
            so_paths_g->push_back(temp);
        }
    }
done:
    if (DALE_DEBUG) {
        fprintf(stderr,
                "Loaded library %s (%s)\n",
                lib_path, temp.c_str());
    }

    return !res;
}

int Generator::addDaleModule(Node *n,
                             const char *my_module_name,
                             std::vector<const char*> *import_forms)
{
    std::vector<const char*> temp;
    if (import_forms == NULL) {
        import_forms = &temp;
    }

    std::string real_module_name;
    if (!(strstr(my_module_name, "lib") == my_module_name)) {
        real_module_name.append("lib");
    }
    real_module_name.append(my_module_name);

    /* If the module has already been added, then skip it. */

    if (included_modules->find(real_module_name)
            != included_modules->end()) {
        return 1;
    }

    /* Look for the module in the current directory, then the
     * specified directories, then the modules directory. */

    std::string tmn(real_module_name);
    std::string tmn2(real_module_name);
    std::string tmn4;
    const char *bc_suffix    = ".bc";
    const char *bc_nm_suffix = "-nomacros.bc";
    const char *so_suffix    = ".so";
    tmn.append(".dtm");
    FILE *test = fopen(tmn.c_str(), "r");
    if (!test) {
        int mi;
        for (mi = 0; mi < mod_path_count; ++mi) {
            std::string whole_name(mod_paths[mi]);
            whole_name.append("/")
            .append(real_module_name)
            .append(".dtm");
            test = fopen(whole_name.c_str(), "r");
            if (test) {
                tmn2 = std::string(mod_paths[mi]);
                tmn2.append("/")
                .append(real_module_name)
                .append(bc_suffix);

                tmn4 = std::string(mod_paths[mi]);
                tmn4.append("/")
                .append(real_module_name)
                .append(so_suffix);
                break;
            }
        }

        if (!test) {
            std::string whole_name(DALE_MODULE_PATH);
            whole_name.append("/")
            .append(real_module_name)
            .append(".dtm");
            test = fopen(whole_name.c_str(), "r");
            if (!test) {
                Error *e = new Error(
                    ErrorInst::Generator::FileError,
                    n,
                    whole_name.c_str(),
                    strerror(errno)
                );
                erep->addError(e);
                return 0;
            }
            tmn2 = std::string(DALE_MODULE_PATH);
            tmn2.append("/")
            .append(real_module_name)
            .append(bc_suffix);

            tmn4 = std::string(DALE_MODULE_PATH);
            tmn4.append("/")
            .append(real_module_name)
            .append(so_suffix);
        }
    } else {
        tmn2.append(bc_suffix);

        char *cwd = getcwd(NULL, 0);
        tmn4 = std::string(cwd);
        free(cwd);

        tmn4.append("/")
        .append(real_module_name)
        .append(so_suffix);
    }

    Context *mynewcontext = new Context(erep, nt, tr);

    int fd = fileno(test);
    struct stat buf;
    if (fstat(fd, &buf)) {
        fprintf(stderr, "Unable to fstat file\n");
        abort();
    }
    int size = buf.st_size;
    char *data = (char*) malloc(size);
    char *original_data = data;
    if (fread(data, 1, size, test) != (size_t) size) {
        fprintf(stderr, "Unable to read module file.\n");
        abort();
    }

    data = deserialise(tr, data, mynewcontext);
    std::set<std::string> temponcetags;
    data = deserialise(tr, data, &temponcetags);
    std::set<std::string> tempmodules;
    data = deserialise(tr, data, &tempmodules);
    int my_cto;
    data = deserialise(tr, data, &my_cto);
    std::map<std::string, std::string> new_typemap;
    data = deserialise(tr, data, &new_typemap);
    for (std::map<std::string, std::string>::iterator
            b = new_typemap.begin(),
            e = new_typemap.end();
            b != e;
            ++b) {
        std::string x = (*b).first;
        std::string y = (*b).second;
        addTypeMapEntry(x.c_str(), y.c_str());
    }
    free(original_data);

    std::string module_path(tmn2);
    std::string module_path_nomacros(tmn2);

    module_path_nomacros.replace(module_path_nomacros.find(".bc"), 
                                 3, bc_nm_suffix);

    llvm::Module *new_module = loadModule(&module_path, false);

    included_modules->insert(real_module_name);

    /* Load each dependent module, before loading this one. */

    for (std::set<std::string>::iterator b = tempmodules.begin(),
            e = tempmodules.end();
            b != e;
            ++b) {
        int res = addDaleModule(n, (*b).c_str(), NULL);
        if (!res) {
            return 0;
        }
    }

    if (my_cto) {
        cto_modules->insert(real_module_name);
    }

    int add_to_so_paths =
        (cto_modules->find(std::string(real_module_name)) ==
         cto_modules->end());

    /* Never add to so_paths if you are making a module (it's
     * pointless - it only matters when you are linking an
     * executable). */
    bool res = addLib(tmn4.c_str(), 0,
                      ((module_name.size() == 0) && add_to_so_paths));
    if (!res) {
        fprintf(stderr, "Cannot addlib\n");
        abort();
    }

    /* Get the union of temponcetags and included_once_tags.
     * Remove from the module any structs/enums that have a once
     * tag from this set, remove the bodies of any
     * functions/variables that have a once tag from this set, and
     * remove from the context any structs/enums that have a once
     * tag from this set (the functions and variables can stay,
     * they won't cause any trouble.) todo: comment doesn't make
     * any sense given what's below. */

    std::set<std::string> common;
    std::set_union(included_once_tags->begin(),
                   included_once_tags->end(),
                   temponcetags.begin(),
                   temponcetags.end(),
                   std::insert_iterator<std::set<std::string> >(
                       common,
                       common.end()));

    mynewcontext->eraseOnceForms(&common, new_module);

    std::set<std::string> current;
    std::merge(included_once_tags->begin(),
               included_once_tags->end(),
               temponcetags.begin(),
               temponcetags.end(),
               std::insert_iterator<std::set<std::string> >(
                   current,
                   current.end()));
    included_once_tags->erase(included_once_tags->begin(),
                              included_once_tags->end());
    included_once_tags->insert(current.begin(), current.end());

    /* Add the module name to the set of included modules. */
    included_modules->insert(real_module_name);

    dtm_modules->insert(std::pair<std::string, llvm::Module *>(
                            std::string(real_module_name),
                            new_module
                        ));

    dtm_nm_modules->insert(std::pair<std::string, std::string>(
                               std::string(real_module_name), 
                               module_path_nomacros
                           ));

    /* Remove from mynewctx things not mentioned in import_forms,
     * but only if at least one import form has been specified.
     * */

    if (import_forms->size() > 0) {
        std::set<std::string> forms_set;
        for (std::vector<const char*>::iterator
                b = import_forms->begin(),
                e = import_forms->end();
                b != e;
                ++b) {
            forms_set.insert(std::string(*b));
        }

        std::set<std::string> found;
        mynewcontext->removeUnneeded(&forms_set, &found);

        std::set<std::string> not_found;
        set_difference(forms_set.begin(), forms_set.end(),
                       found.begin(),     found.end(),
                       std::insert_iterator<std::set<std::string> >(
                           not_found,
                           not_found.end()));
        if (not_found.size() > 0) {
            std::string all;
            for (std::set<std::string>::iterator b = not_found.begin(),
                    e = not_found.end();
                    b != e;
                    ++b) {
                all.append(*b).append(", ");
            }
            all.erase(all.size() - 2, all.size() - 1);
            std::string temp_mod_name(real_module_name);
            // Get rid of "^lib".
            temp_mod_name.replace(0, 3, "");
            Error *e = new Error(
                ErrorInst::Generator::ModuleDoesNotProvideForms,
                n,
                temp_mod_name.c_str(),
                all.c_str()
            );
            erep->addError(e);
            return 0;
        }

    }

    ctx->merge(mynewcontext);
    ctx->regetPointersForNewModule(mod);
    ctx->relink();

    return 1;
}

bool Generator::destructIfApplicable(ParseResult *pr,
        llvm::IRBuilder<> *builder,
        ParseResult *pr_ret,
        bool value_is_ptr)
{
    pr->copyTo(pr_ret);

    if (pr->do_not_destruct) {
        return true;
    }

    if (DALE_DEBUG) {
        std::string mytype;
        pr->type->toString(&mytype);
        if (mytype.size() == 0) {
            fprintf(stderr, "Internal error: trying to destroy "
                    "ParseResult, but the type is empty.");
            abort();
        } else {
            fprintf(stderr, "%s\n", mytype.c_str());
        }
    }

    if (!pr->type) {
        fprintf(stderr, "No type in destruct call.\n");
        abort();
    }

    /* If it's an array with a known size, call this function for
     * each element in the array in order from last to first. */
    if (pr->type->is_array && pr->type->array_size) {
        Type *mine =
            pr->type->array_type;
        llvm::BasicBlock   *mbl  = pr->block;
        int i = pr->type->array_size;
        llvm::Value *actual_value = pr->value;
        if (DALE_DEBUG) {
            fprintf(stderr, "Destroying array type\n");
        }

        if (!(pr->value)) {
            if (DALE_DEBUG) {
                fprintf(stderr, "Parseresult has no value (in "
                        "destructIfApplicable).");
            }
            return true;
        }
        if (!(pr->value->getType())) {
            if (DALE_DEBUG) {
                fprintf(stderr, "Parseresult value has no type (in "
                        "destructIfApplicable).");
            }
            return true;
        }

        std::vector<Type *> types;
        if (!mine->is_array) {
            types.push_back(tr->getPointerType(mine));
            Function *fn = ctx->getFunction("destroy", &types,
                                    NULL, 0);
            if (!fn) {
                return true;
            }
        }

        /* Hmph: array literals are stored in the variable table as
         * actual arrays, rather than pointers to arrays. This should
         * be fixed at some point, but for now, if this value is not a
         * pointer, then store it in a temporary location. */

        if (!(pr->value->getType()->isPointerTy())) {
            if (builder) {
                llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                        builder->CreateAlloca(
                            ctx->toLLVMType(pr->type, NULL, false))
                        );
                builder->CreateStore(pr->value, new_ptr2);
                actual_value = new_ptr2;
            } else {
                llvm::IRBuilder<> builder(mbl);
                llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                        builder.CreateAlloca(
                            ctx->toLLVMType(pr->type, NULL, false))
                        );
                builder.CreateStore(pr->value, new_ptr2);
                actual_value = new_ptr2;
            }
        }

        for (i = (pr->type->array_size - 1); i >= 0; i--) {
            ParseResult temp;
            temp.type  = mine;
            temp.block = mbl;
            std::vector<llvm::Value *> indices;
            STL::push_back2(&indices,
                            nt->getLLVMZero(),
                            llvm::cast<llvm::Value>(
                                llvm::ConstantInt::get(
                                    nt->getNativeIntType(),
                                    i
                                )
                            ));
            ParseResult mnew;

            if (builder) {
                llvm::Value *res = builder->Insert(
                                       llvm::GetElementPtrInst::Create(
                                           actual_value,
                                           llvm::ArrayRef<llvm::Value*>(indices)
                                       ),
                                       "asdf"
                                   );
                if (!mine->is_array) {
                    temp.value = builder->CreateLoad(res);
                } else {
                    temp.value = res;
                }
                destructIfApplicable(&temp, builder, &mnew);
            }
            else {
                llvm::IRBuilder<> builder(mbl);
                llvm::Value *res = builder.Insert(
                                       llvm::GetElementPtrInst::Create(
                                           actual_value,
                                           llvm::ArrayRef<llvm::Value*>(indices)
                                       ),
                                       "asdf"
                                   );
                if (!mine->is_array) {
                    temp.value = builder.CreateLoad(res);
                } else {
                    temp.value = res;
                }
                destructIfApplicable(&temp, &builder, &mnew);
            }
            mbl = mnew.block;
        }
        pr_ret->block = mbl;
        return true;
    }

    std::vector<Type *> types;
    types.push_back(tr->getPointerType(pr->type));
    Function *fn = ctx->getFunction("destroy", &types,
                            NULL, 0);
    if (!fn) {
        /* If this is a struct, call destructIfApplicable on each of
         * the elements, in the absence of a destructor for the struct
         * as a whole. */
        Type *type = pr->type;
        if (type->struct_name.size()) {
            Struct *st = ctx->getStruct(type->struct_name.c_str(),
                                                 &(type->namespaces));
            std::vector<Type*> *st_types = &(st->member_types);
            int i = 0;
            llvm::Value *actual_value = pr->value;

            if (!value_is_ptr) {
                if (builder) {
                    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                            builder->CreateAlloca(
                                ctx->toLLVMType(pr->type, NULL, false))
                            );
                    builder->CreateStore(pr->value, new_ptr2);
                    actual_value = new_ptr2;
                } else {
                    llvm::IRBuilder<> builder(pr->block);
                    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                            builder.CreateAlloca(
                                ctx->toLLVMType(pr->type, NULL, false))
                            );
                    builder.CreateStore(pr->value, new_ptr2);
                    actual_value = new_ptr2;
                }
            }

            for (std::vector<Type*>::iterator
                    b = st_types->begin(),
                    e = st_types->end();
                    b != e;
                    ++b) {
                ParseResult element;
                ParseResult mnew;
                std::string ts;
                (*b)->toString(&ts);
                element.set(pr->block, *b, actual_value);
                std::vector<llvm::Value *> indices;
                STL::push_back2(&indices,
                                nt->getLLVMZero(),
                                llvm::cast<llvm::Value>(
                                    llvm::ConstantInt::get(
                                        nt->getNativeIntType(),
                                        i++
                                    )
                                ));
                if (builder) {
                    llvm::Value *res = 
                        builder->Insert(
                            llvm::GetElementPtrInst::Create(
                                actual_value,
                                llvm::ArrayRef<llvm::Value*>(indices)
                            ),
                            "asdf"
                        );
                    element.value = res;
                    destructIfApplicable(&element, builder, &mnew, true);
                } else {
                    llvm::IRBuilder<> builder(pr->block);
                    llvm::Value *res = 
                        builder.Insert(
                            llvm::GetElementPtrInst::Create(
                                actual_value,
                                llvm::ArrayRef<llvm::Value*>(indices)
                            ),
                            "asdf"
                        );
                    element.value = res;
                    destructIfApplicable(&element, &builder, &mnew, true);
                }
            }
        }
        return true;
    }
    int destroy_builder = 0;
    if (!builder) {
        destroy_builder = 1;
        builder = new llvm::IRBuilder<>(pr->block);
    }
    std::vector<llvm::Value *> call_args;
    llvm::Value *new_ptr2;
    if (value_is_ptr) {
        new_ptr2 = pr->value;
    } else {
        new_ptr2 = llvm::cast<llvm::Value>(
            builder->CreateAlloca(ctx->toLLVMType(pr->type, NULL, false))
        );
        builder->CreateStore(pr->value, new_ptr2);
    }

    call_args.push_back(new_ptr2);
    builder->CreateCall(
        fn->llvm_function,
        llvm::ArrayRef<llvm::Value*>(call_args));
    if (destroy_builder) {
        delete builder;
    }
    return true;
}

bool Generator::copyWithSetfIfApplicable(
    Function *dfn,
    ParseResult *pr,
    ParseResult *pr_res
) {
    /* If this is a setf function, then don't copy, even if it can
     * be done. This is because, if the setf function is (e.g.)
     * the same as the current function, you'll end up with
     * non-terminating recursion. It would be possible to limit
     * this to the same function only, but you could have mutual
     * recursion - it would be better for the author to do all of
     * this manually, rather than complicating things. */
    pr->copyTo(pr_res);
    if (dfn->is_setf_fn) {
        return true;
    }
    if (pr->do_not_copy_with_setf) {
        return true;
    }
    /* If the parseresult has already been copied, then don't copy
     * it again (pointless). todo: if you are having copy etc.
     * problems, this is likely to be the issue. */
    if (pr->freshly_copied) {
        return true;
    }
    std::vector<Type *> types;
    Type *copy_type = tr->getPointerType(pr->type);
    types.push_back(copy_type);
    types.push_back(copy_type);
    Function *over_setf =
        ctx->getFunction("setf-copy", &types, NULL, 0);
    if (!over_setf) {
        return true;
    }
    llvm::IRBuilder<> builder(pr->block);
    llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                builder.CreateAlloca(ctx->toLLVMType(pr->type, NULL,
                                        false))
                            );
    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                builder.CreateAlloca(ctx->toLLVMType(pr->type, NULL,
                                        false))
                            );
    builder.CreateStore(pr->value, new_ptr2);

    std::vector<llvm::Value *> call_args;
    call_args.push_back(new_ptr1);
    call_args.push_back(new_ptr2);
    builder.CreateCall(
        over_setf->llvm_function,
        llvm::ArrayRef<llvm::Value*>(call_args));
    llvm::Value *result = builder.CreateLoad(new_ptr1);

    pr_res->set(pr->block, pr->type, result);
    pr_res->freshly_copied = 1;

    return true;
}

bool Generator::scopeClose(Function *dfn,
                           llvm::BasicBlock *block,
                           llvm::Value *no_destruct,
                           bool entire_function)
{
    std::vector<Variable *> stack_vars;
    if (entire_function) {
        ctx->ns()->getVarsAfterIndex(dfn->index, &stack_vars);
    } else {
        ctx->ns()->getVariables(&stack_vars);
    }
    
    ParseResult mnew;
    mnew.block = block;

    for (std::vector<Variable *>::iterator
            b = stack_vars.begin(),
            e = stack_vars.end();
            b != e;
            ++b) {

        if (no_destruct && ((*b)->value == no_destruct)) {
            continue;
        }
        mnew.type = (*b)->type;
        mnew.value = (*b)->value;
        destructIfApplicable(&mnew, NULL, &mnew, true);
    }

    return true;
}

void Generator::processRetval(Type *return_type,
                              llvm::BasicBlock *block, 
                              ParseResult *pr,
                              std::vector<llvm::Value*> *call_args)
{
    if (return_type->is_retval) {
        pr->do_not_destruct = 1;
        pr->do_not_copy_with_setf = 1;
        /* todo: may turn out to be unnecessary. */
        pr->retval_used = true;
        if (!pr->retval) {
            llvm::IRBuilder<> builder(block);
            llvm::Type *et = 
                ctx->toLLVMType(return_type, NULL, false,
                                false);
            if (!et) {
                return;
            }
            llvm::Value *new_ptr =
                llvm::cast<llvm::Value>(
                    builder.CreateAlloca(et)
                );
            call_args->push_back(new_ptr);
            pr->retval = new_ptr;
            pr->retval_type = ctx->tr->getPointerType(return_type);
        } else {
            call_args->push_back(pr->retval);
        }
    }

    return;
}

bool Generator::parseFuncallInternal(
    Function *dfn,
    Node *n,
    bool getAddress,
    ParseResult *fn_ptr,
    int skip,
    std::vector<llvm::Value*> *extra_call_args,
    ParseResult *pr
) {
    assert(n->list && "must receive a list!");

    llvm::BasicBlock *block = fn_ptr->block;
    llvm::Value *fn = fn_ptr->value;
    symlist *lst = n->list;

    std::vector<Node *>::iterator symlist_iter;
    symlist_iter = lst->begin();
    int count = lst->size() - skip;
    if (extra_call_args) {
        count += extra_call_args->size();
    }

    int num_required_args =
        fn_ptr->type->points_to->numberOfRequiredArgs();

    if (fn_ptr->type->points_to->isVarArgs()) {
        if (count < num_required_args) {
            char buf1[100];
            char buf2[100];
            sprintf(buf1, "%d", num_required_args);
            sprintf(buf2, "%d", count);

            Error *e = new Error(
                ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
                n,
                "function pointer call", buf1, buf2
            );
            erep->addError(e);
            return false;
        }
    } else {
        if (count != num_required_args) {
            char buf1[100];
            char buf2[100];
            sprintf(buf1, "%d", num_required_args);
            sprintf(buf2, "%d", count);

            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                n,
                "function pointer call", buf1, buf2
            );
            erep->addError(e);
            return false;
        }
    }

    std::vector<llvm::Value *> call_args;
    std::vector<ParseResult> call_arg_prs;
    std::vector<Node *> call_arg_nodes;
    if (extra_call_args) {
        for (std::vector<llvm::Value*>::iterator b =
                    extra_call_args->begin(), e = extra_call_args->end(); b !=
                e; ++b) {
            call_args.push_back((*b));
        }
    }
    std::vector<Type *>::iterator param_iter;

    while (skip--) {
        ++symlist_iter;
    }

    param_iter = fn_ptr->type->points_to->parameter_types.begin();
    bool args_cast = false;
    int arg_count = 1;
    int size = 0;
    if (extra_call_args) {
       size = (int) extra_call_args->size();
    }
    while (size--) {
        ++param_iter;
    }
    while (symlist_iter != lst->end()) {
        ParseResult p;
        bool res = FormProcInstParse(this, 
            dfn, block, (*symlist_iter), getAddress, false, NULL, &p,
            true
        );
        if (!res) {
            return false;
        }

        call_arg_prs.push_back(p);
        call_arg_nodes.push_back(*symlist_iter);
        block = p.block;

        if ((param_iter != fn_ptr->type->points_to->parameter_types.end())
                && (!(p.type->isEqualTo((*param_iter), 1)))
                && ((*param_iter)->base_type != BaseType::VarArgs)) {
            ParseResult coerce;
            bool coerce_result = Operation::Coerce(ctx, block,
                                               p.getValue(ctx),
                                               p.type,
                                               (*param_iter),
                                               &coerce);
            llvm::Value *new_val = coerce.value;

            if (!coerce_result) {
                std::string twant;
                std::string tgot;
                (*param_iter)->toString(&twant);
                p.type->toString(&tgot);
                char buf[100];
                sprintf(buf, "%d", arg_count);

                Error *e = new Error(
                    ErrorInst::Generator::IncorrectArgType,
                    (*symlist_iter),
                    "function pointer call",
                    twant.c_str(), buf, tgot.c_str()
                );
                erep->addError(e);
                return false;
            } else {
                args_cast = true;
                call_args.push_back(new_val);
            }
        } else {
            call_args.push_back(p.getValue(ctx));
        }

        ++symlist_iter;

        if (param_iter != fn_ptr->type->points_to->parameter_types.end()) {
            ++param_iter;
            // Skip the varargs type.
            if (param_iter !=
                    fn_ptr->type->points_to->parameter_types.end()) {
                if ((*param_iter)->base_type == BaseType::VarArgs) {
                    ++param_iter;
                }
            }
        }
    }

    llvm::IRBuilder<> builder(block);

    /* Iterate over the types of the found function. For the reference
     * types, replace the call argument with its address. todo: same
     * code as in parseFunctionCall, move into a separate function. */

    std::vector<llvm::Value *> call_args_final = call_args;
    int caps = call_arg_prs.size();
    int pts  = fn_ptr->type->points_to->parameter_types.size();
    int limit = (caps > pts ? pts : caps);
    ParseResult refpr;
    int start = (extra_call_args ? extra_call_args->size() : 0);
    for (int i = start; i < limit; i++) {
        Type *pt = 
            fn_ptr->type->points_to->parameter_types.at(i);
        ParseResult *arg_refpr = &(call_arg_prs.at(i));
        if (pt->is_reference) {
            if (!pt->is_const && !arg_refpr->value_is_lvalue) {
                Error *e = new Error(
                    ErrorInst::Generator::CannotTakeAddressOfNonLvalue,
                    call_arg_nodes.at(i)
                );
                erep->addError(e);
                return false;
            }
            bool res = arg_refpr->getAddressOfValue(ctx, &refpr);
            if (!res) {
                return false;
            }
            call_args_final[i] = refpr.getValue(ctx);
        } else {
            /* If arguments had to be cast, then skip the copies,
             * here. (todo: do the casting after this part, instead.)
             * */
            if (!args_cast) {
                bool res = copyWithSetfIfApplicable(dfn, arg_refpr, arg_refpr);
                if (!res) {
                    return false;
                }
                call_args_final[i] = arg_refpr->getValue(ctx);
            }
        }
    }

    processRetval(fn_ptr->type->points_to->return_type,
                  block, pr, &call_args_final);

    llvm::Value *call_res =
        builder.CreateCall(fn, llvm::ArrayRef<llvm::Value*>(call_args_final));

    pr->set(block, fn_ptr->type->points_to->return_type, call_res);

    fn_ptr->block = pr->block;
    ParseResult temp;
    bool res = destructIfApplicable(fn_ptr, NULL, &temp);
    if (!res) {
        return false;
    }
    pr->block = temp.block;

    return true;
}

bool Generator::parseFunctionCall(Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        const char *name,
        bool getAddress,
        bool prefixed_with_core,
        Function **macro_to_call,
        ParseResult *pr)
{
    if (DALE_DEBUG) {
        fprintf(stderr, "Calling '%s'\n", name);
    }

    assert(n->list && "must receive a list!");

    if (getAddress) {
        Error *e = new Error(
            ErrorInst::Generator::CannotTakeAddressOfNonLvalue,
            n
        );
        erep->addError(e);
        return false;
    }

    symlist *lst = n->list;

    Node *nfn_name = (*lst)[0];

    if (!nfn_name->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            nfn_name
        );
        erep->addError(e);
        return false;
    }

    Token *t = nfn_name->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            nfn_name
        );
        erep->addError(e);
        return false;
    }

    /* Put all of the arguments into a list. */

    std::vector<Node *>::iterator symlist_iter;

    std::vector<llvm::Value *> call_args;
    std::vector<Node *> call_arg_nodes;
    std::vector<ParseResult> call_arg_prs;
    std::vector<Type *> call_arg_types;

    std::vector<llvm::Value *> call_args_newer;
    std::vector<Type *> call_arg_types_newer;

    if (!strcmp(name, "setf")) {
        /* Add a bool argument and type to the front of the
         * function call. */
        call_arg_types.push_back(ctx->tr->type_bool);
        call_args.push_back(nt->getLLVMFalse());
    }

    symlist_iter = lst->begin();
    /* Skip the function name. */
    ++symlist_iter;

    /* The processing below is only required when the function/macro
     * name is overloaded. For now, short-circuit for macros that are
     * not overloaded, because that will give the greatest benefits.
     * */

    if (!ctx->isOverloadedFunction(t->str_value.c_str())) {
        std::map<std::string, std::vector<Function *> *>::iterator
            iter;
        Function *fn = NULL;
        for (std::vector<NSNode *>::reverse_iterator
                rb = ctx->used_ns_nodes.rbegin(),
                re = ctx->used_ns_nodes.rend();
                rb != re;
                ++rb) {
            iter = (*rb)->ns->functions.find(name);
            if (iter != (*rb)->ns->functions.end()) {
                fn = iter->second->at(0);
                break;
            }
        }
        if (fn && fn->is_macro) {
            /* If the third argument is either non-existent, or a (p
             * DNode) (because typed arguments must appear before the
             * first (p DNode) argument), then short-circuit, so long
             * as the argument count is ok. */
            std::vector<Variable*>::iterator
                b = (fn->parameter_types.begin() + 1);
            if ((b == fn->parameter_types.end())
                    || (*b)->type->isEqualTo(tr->type_pdnode)) {
                bool use = false;
                int size = lst->size();
                if (fn->isVarArgs()) {
                    use = ((fn->numberOfRequiredArgs() - 1)
                            <= (size - 1));
                } else {
                    use = ((fn->numberOfRequiredArgs() - 1)
                            == (size - 1));
                }
                if (use) {
                    *macro_to_call = fn;
                    return false;
                }
            }
        }
    }

    std::vector<Error*> errors;

    /* Record the number of blocks and the instruction index in the
     * current block. If the underlying Function to call
     * is a function, then there's no problem with using the
     * modifications caused by the repeated PFBI calls below. If
     * it's a macro, however, anything that occurred needs to be
     * 'rolled back'. Have to do the same thing for the context. */

    int current_block_count = dfn->llvm_function->size();
    int current_instr_index = block->size();
    int current_dgcount = dfn->deferred_gotos.size();
    std::map<std::string, Label *> labels = dfn->labels;
    llvm::BasicBlock *original_block = block;
    ContextSavePoint *csp = new ContextSavePoint(ctx);

    while (symlist_iter != lst->end()) {
        call_arg_nodes.push_back(*symlist_iter);
        int error_count =
            erep->getErrorTypeCount(ErrorType::Error);

        ParseResult p;
        bool res = 
            FormProcInstParse(this, dfn, block, (*symlist_iter),
                                    false, false, NULL,
                                    &p, true);

        int diff = erep->getErrorTypeCount(ErrorType::Error)
                   - error_count;

        if (!res || diff) {
            /* May be a macro call (could be an unparseable
             * argument). Pop and store errors for the time being
             * and treat this argument as a (p DNode). */

            if (diff) {
                errors.insert(errors.end(),
                              erep->errors.begin() + error_count,
                              erep->errors.end());
                erep->errors.erase(erep->errors.begin() + error_count,
                                    erep->errors.end());
            }

            call_args.push_back(NULL);
            call_arg_types.push_back(tr->type_pdnode);
            ++symlist_iter;
            continue;
        }

        block = p.block;
        if (p.type->is_array) {
            p = ParseResult(block, p.type_of_address_of_value,
                            p.address_of_value);
        }
        call_args.push_back(p.getValue(ctx));
        call_arg_types.push_back(p.type);
        call_arg_prs.push_back(p);

        ++symlist_iter;
    }

    /* Now have all the argument types. Get the function out of
     * the context. */

    Function *closest_fn = NULL;

    Function *fn =
        ctx->getFunction(t->str_value.c_str(),
                         &call_arg_types,
                         &closest_fn,
                         0);

    /* If the function is a macro, set macro_to_call and return
     * NULL. (It's the caller's responsibility to handle
     * processing of macros.) */

    if (fn && fn->is_macro) {
        /* Remove any basic blocks that have been added by way of
         * the parsing of the macro arguments, and remove any
         * extra instructions added to the current block. Restore
         * the context save point. */

        int block_pop_back =
            dfn->llvm_function->size() - current_block_count;
        while (block_pop_back--) {
            llvm::Function::iterator
            bi = dfn->llvm_function->begin(),
            be = dfn->llvm_function->end(),
            bl;

            while (bi != be) {
                bl = bi;
                ++bi;
            }
            bl->eraseFromParent();
        }

        int to_pop_back = original_block->size() - current_instr_index;
        while (to_pop_back--) {
            llvm::BasicBlock::iterator
            bi = original_block->begin(),
            be = original_block->end(), bl;

            while (bi != be) {
                bl = bi;
                ++bi;
            }
            bl->eraseFromParent();
        }

        int dg_to_pop_back = dfn->deferred_gotos.size() - current_dgcount;
        while (dg_to_pop_back--) {
            dfn->deferred_gotos.pop_back();
        }
        dfn->labels = labels;

        csp->restore();
        delete csp;

        *macro_to_call = fn;
        return false;
    }
    delete csp;

    /* If the function is not a macro, and errors were encountered
     * during argument processing, then this function has been
     * loaded in error (it will be a normal function taking a (p
     * DNode) argument, but the argument is not a real (p DNode)
     * value). Replace all the errors and return NULL. */

    if (errors.size() && fn && !fn->is_macro) {
        for (std::vector<Error*>::reverse_iterator b = errors.rbegin(),
                e = errors.rend();
                b != e;
                ++b) {
            erep->addError(*b);
        }
        return false;
    }

    bool args_cast = false;

    if (!fn) {
        /* If no function was found, and there are errors related
         * to argument parsing, then push those errors back onto
         * the reporter and return. (May change this later to be a
         * bit more friendly - probably if there are any macros or
         * functions with the same name, this should show the
         * overload failure, rather than the parsing failure
         * errors). */
        if (errors.size()) {
            for (std::vector<Error*>::reverse_iterator b = errors.rbegin(),
                    e = errors.rend();
                    b != e;
                    ++b) {
                erep->addError(*b);
            }
            return false;
        }

        if (ctx->existsExternCFunction(t->str_value.c_str())) {
            /* The function name is not overloaded. */
            /* Get this single function, try to cast each integral
             * call_arg to the expected type. If that succeeds
             * without error, then keep going. */

            fn = ctx->getFunction(t->str_value.c_str(),
                                  NULL, NULL, 0);

            std::vector<Variable *> myarg_types =
                fn->parameter_types;
            std::vector<Variable *>::iterator miter =
                myarg_types.begin();

            std::vector<llvm::Value *>::iterator citer =
                call_args.begin();
            std::vector<Type *>::iterator caiter =
                call_arg_types.begin();

            /* Create strings describing the types, for use in a
             * possible error message. */

            std::string expected_args;
            std::string provided_args;
            while (miter != myarg_types.end()) {
                (*miter)->type->toString(&expected_args);
                expected_args.append(" ");
                ++miter;
            }
            if (expected_args.size() == 0) {
                expected_args.append("void");
            } else {
                expected_args.erase(expected_args.size() - 1, 1);
            }
            while (caiter != call_arg_types.end()) {
                (*caiter)->toString(&provided_args);
                provided_args.append(" ");
                ++caiter;
            }
            if (provided_args.size() == 0) {
                provided_args.append("void");
            } else {
                provided_args.erase(provided_args.size() - 1, 1);
            }
            miter = myarg_types.begin();
            caiter = call_arg_types.begin();
            int size = call_args.size();

            if (size < fn->numberOfRequiredArgs()) {
                Error *e = new Error(
                    ErrorInst::Generator::FunctionNotInScope,
                    n,
                    t->str_value.c_str(),
                    provided_args.c_str(),
                    expected_args.c_str()
                );
                erep->addError(e);
                return false;
            }
            if (!fn->isVarArgs()
                    && size != fn->numberOfRequiredArgs()) {
                Error *e = new Error(
                    ErrorInst::Generator::FunctionNotInScope,
                    n,
                    t->str_value.c_str(),
                    provided_args.c_str(),
                    expected_args.c_str()
                );
                erep->addError(e);
                return false;
            }

            while (miter != myarg_types.end()
                    && citer != call_args.end()
                    && caiter != call_arg_types.end()) {
                if ((*caiter)->isEqualTo((*miter)->type, 1)) {
                    call_args_newer.push_back((*citer));
                    call_arg_types_newer.push_back((*caiter));
                    ++miter;
                    ++citer;
                    ++caiter;
                    continue;
                }
                if (!(*miter)->type->isIntegerType()
                        and (*miter)->type->base_type != BaseType::Bool) {
                    Error *e = new Error(
                        ErrorInst::Generator::FunctionNotInScope,
                        n,
                        t->str_value.c_str(),
                        provided_args.c_str(),
                        expected_args.c_str()
                    );
                    erep->addError(e);
                    return false;
                }
                if (!(*caiter)->isIntegerType()
                        and (*caiter)->base_type != BaseType::Bool) {
                    Error *e = new Error(
                        ErrorInst::Generator::FunctionNotInScope,
                        n,
                        t->str_value.c_str(),
                        provided_args.c_str(),
                        expected_args.c_str()
                    );
                    erep->addError(e);
                    return false;
                }

                ParseResult mytemp;
                bool res = Operation::Cast(ctx, block,
                           (*citer),
                           (*caiter),
                           (*miter)->type,
                           n,
                           IMPLICIT,
                           &mytemp);
                if (!res) {
                    Error *e = new Error(
                        ErrorInst::Generator::FunctionNotInScope,
                        n,
                        t->str_value.c_str(),
                        provided_args.c_str(),
                        expected_args.c_str()
                    );
                    erep->addError(e);
                    return false;
                }
                block = mytemp.block;
                call_args_newer.push_back(mytemp.getValue(ctx));
                call_arg_types_newer.push_back(mytemp.type);

                ++miter;
                ++citer;
                ++caiter;
            }

            call_args = call_args_newer;
            call_arg_types = call_arg_types_newer;
            args_cast = true;
        } else if (ctx->existsNonExternCFunction(t->str_value.c_str())) {
            /* Return a no-op ParseResult if the function name is
             * 'destroy', because it's tedious to have to check in
             * generic code whether a particular value can be
             * destroyed or not. */
            if (!t->str_value.compare("destroy")) {
                pr->set(block, ctx->tr->type_void, NULL);
                return true;
            }

            std::vector<Type *>::iterator titer =
                call_arg_types.begin();

            std::string args;
            while (titer != call_arg_types.end()) {
                (*titer)->toString(&args);
                ++titer;
                if (titer != call_arg_types.end()) {
                    args.append(" ");
                }
            }

            if (closest_fn) {
                std::string expected;
                std::vector<Variable *>::iterator viter;
                viter = closest_fn->parameter_types.begin();
                if (closest_fn->is_macro) {
                    ++viter;
                }
                while (viter != closest_fn->parameter_types.end()) {
                    (*viter)->type->toString(&expected);
                    expected.append(" ");
                    ++viter;
                }
                if (expected.size() > 0) {
                    expected.erase(expected.size() - 1, 1);
                }
                Error *e = new Error(
                    ErrorInst::Generator::OverloadedFunctionOrMacroNotInScopeWithClosest,
                    n,
                    t->str_value.c_str(), args.c_str(),
                    expected.c_str()
                );
                erep->addError(e);
                return false;
            } else {
                Error *e = new Error(
                    ErrorInst::Generator::OverloadedFunctionOrMacroNotInScope,
                    n,
                    t->str_value.c_str(), args.c_str()
                );
                erep->addError(e);
                return false;
            }
        } else {
            Error *e = new Error(
                ErrorInst::Generator::NotInScope,
                n,
                t->str_value.c_str()
            );
            erep->addError(e);
            return false;
        }
    }

    llvm::IRBuilder<> builder(block);

    /* If this function is varargs, find the point at which the
     * varargs begin, and then promote any call_args floats to
     * doubles, and any integer types smaller than the native
     * integer size to native integer size. */

    if (fn->isVarArgs()) {
        args_cast = true;
        int n = fn->numberOfRequiredArgs();

        std::vector<llvm::Value *>::iterator call_args_iter
        = call_args.begin();
        std::vector<Type *>::iterator call_arg_types_iter
        = call_arg_types.begin();

        while (n--) {
            ++call_args_iter;
            ++call_arg_types_iter;
        }
        while (call_args_iter != call_args.end()) {
            if ((*call_arg_types_iter)->base_type == BaseType::Float) {
                (*call_args_iter) =
                    builder.CreateFPExt(
                        (*call_args_iter),
                        llvm::Type::getDoubleTy(llvm::getGlobalContext())
                    );
                (*call_arg_types_iter) =
                    ctx->tr->type_double;
            } else if ((*call_arg_types_iter)->isIntegerType()) {
                int real_size =
                    nt->internalSizeToRealSize(
                        (*call_arg_types_iter)->getIntegerSize()
                    );

                if (real_size < nt->getNativeIntSize()) {
                    if ((*call_arg_types_iter)->isSignedIntegerType()) {
                        /* Target integer is signed - use sext. */
                        (*call_args_iter) =
                            builder.CreateSExt((*call_args_iter),
                                               ctx->toLLVMType(
                                                    ctx->tr->type_int,
                                                              NULL, false));
                        (*call_arg_types_iter) = ctx->tr->type_int;
                    } else {
                        /* Target integer is not signed - use zext. */
                        (*call_args_iter) =
                            builder.CreateZExt((*call_args_iter),
                                               ctx->toLLVMType(
                                                    ctx->tr->type_uint,
                                                              NULL, false));
                        (*call_arg_types_iter) = ctx->tr->type_uint;
                    }
                }
            }
            ++call_args_iter;
            ++call_arg_types_iter;
        }
    }

    /* Iterate over the types of the found function. For the reference
     * types, replace the call argument with its address. */
    
    std::vector<llvm::Value *> call_args_final = call_args;
    int caps = call_arg_prs.size();
    int pts  = fn->parameter_types.size();
    int limit = (caps > pts ? pts : caps);
    ParseResult refpr;
    for (int i = 0; i < limit; i++) {
        Type *pt = fn->parameter_types.at(i)->type;
        ParseResult *arg_refpr = &(call_arg_prs.at(i));
        if (pt->is_reference) {
            if (!pt->is_const && !arg_refpr->value_is_lvalue) {
                Error *e = new Error(
                    ErrorInst::Generator::CannotTakeAddressOfNonLvalue,
                    call_arg_nodes.at(i)
                );
                erep->addError(e);
                return false;
            }
            bool res = arg_refpr->getAddressOfValue(ctx, &refpr);
            if (!res) {
                return false;
            }
            call_args_final[i] = refpr.getValue(ctx);
        } else {
            /* If arguments had to be cast, then skip the copies,
             * here. (todo: do the casting after this part, instead.)
             * */
            if (!args_cast) {
                bool res = copyWithSetfIfApplicable(dfn, arg_refpr, arg_refpr);
                if (!res) {
                    return false;
                }
                call_args_final[i] = arg_refpr->getValue(ctx);
            }
        }
    }
   
    processRetval(fn->return_type, block, pr, &call_args_final);

    llvm::Value *call_res = builder.CreateCall(
                                fn->llvm_function,
                                llvm::ArrayRef<llvm::Value*>(call_args_final));

    pr->set(block, fn->return_type, call_res);

    /* If the return type of the function is one that should be
     * copied with an overridden setf, that will occur in the
     * function, so prevent the value from being re-copied here
     * (because no corresponding destructor call will occur). */

    pr->do_not_copy_with_setf = 1;

    return true;
}

void Generator::parseArgument(Variable *var, Node *top,
                              bool allow_anon_structs,
                              bool allow_bitfields,
                              bool allow_refs)
{
    var->linkage = Linkage::Auto;

    if (!top->is_list) {
        /* Can only be void or varargs. */
        Token *t = top->token;

        if (t->type != TokenType::String) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectSingleParameterType,
                top,
                "symbol", t->tokenType()
            );
            erep->addError(e);
            return;
        }

        if (!strcmp(t->str_value.c_str(), "void")) {
            var->type = ctx->tr->type_void;
            return;
        } else if (!strcmp(t->str_value.c_str(), "...")) {
            var->type = ctx->tr->type_varargs;
            return;
        } else {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectSingleParameterType,
                top,
                "'void'/'...'"
            );
            std::string temp;
            temp.append("'")
            .append(t->str_value.c_str())
            .append("'");
            e->addArgString(&temp);
            erep->addError(e);
            return;
        }
    }

    symlist *lst = top->list;

    if (lst->size() != 2) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectParameterTypeNumberOfArgs,
            top,
            2, (int) lst->size()
        );
        erep->addError(e);
        return;
    }

    Node *nname = (*lst)[0];

    if (!nname->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            nname
        );
        erep->addError(e);
        return;
    }

    Token *tname = nname->token;

    if (tname->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            nname
        );
        erep->addError(e);
        return;
    }

    var->name.clear();
    var->name.append(tname->str_value.c_str());

    Type *type = FormTypeParse(this, (*lst)[1], allow_anon_structs,
                                    allow_bitfields, allow_refs);
    var->type = type;

    return;
}

Unit *
Generator::getUnit(void)
{
    return unit_stack->top();
}
}
