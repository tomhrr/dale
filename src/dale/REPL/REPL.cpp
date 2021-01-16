#include "REPL.h"
#include "Config.h"

#include <float.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
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
#include "llvm/ExecutionEngine/GenericValue.h"
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
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include "../CommonDecl/CommonDecl.h"
#include "../Context/Context.h"
#include "../CoreForms/CoreForms.h"
#include "../Decoder/Decoder.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../ErrorType/ErrorType.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Form/Proc/Setf/Setf.h"
#include "../Form/ProcBody/ProcBody.h"
#include "../Form/TopLevel/GlobalVariable/GlobalVariable.h"
#include "../Form/TopLevel/Inst/Inst.h"
#include "../Module/Writer/Writer.h"
#include "../NativeTypes/NativeTypes.h"
#include "../Node/Node.h"
#include "../STL/STL.h"
#include "../TypeRegister/TypeRegister.h"
#include "../Unit/Unit.h"
#include "../Units/Units.h"
#include "../Utils/Utils.h"

extern "C" {
void init_introspection_functions();
void *find_introspection_function(const char *);
}

namespace dale {
REPL::REPL() {
    CoreForms::init();

    llvm::InitializeNativeTarget();
    llvm::InitializeAllAsmPrinters();
}

REPL::~REPL() {}

/* Determine whether the current system is an x86-64 system. */
bool is_x86_64() {
    /* On OS X, SYSTEM_PROCESSOR is i386 even when the underlying
     * processor is x86-64, hence the extra check here. */
    return (
        (!strcmp(SYSTEM_PROCESSOR, "x86_64")) ||
        ((!strcmp(SYSTEM_PROCESSOR, "amd64"))) ||
        ((!strcmp(SYSTEM_NAME, "Darwin")) && (sizeof(char *) == 8)));
}

/* Return the path to the shared DRT library. */
const char *getLibDRTPath() {
    const char *libdrt_path = NULL;
    FILE *drt_file = NULL;
    if ((drt_file = fopen(DALE_LIBRARY_PATH "/libdrt.so", "r"))) {
        libdrt_path = DALE_LIBRARY_PATH "/libdrt.so";
    } else if ((drt_file = fopen("./libdrt.so", "r"))) {
        libdrt_path = "./libdrt.so";
    } else {
        error("unable to find libdrt.so");
    }
    int res = fclose(drt_file);
    if (res != 0) {
        error("unable to close %s", libdrt_path, true);
    }
    return libdrt_path;
}

/* Return the path to the shared arithmetic library. */
const char *getLibArithmeticPath() {
    const char *libarithmetic_path = NULL;
    FILE *arithmetic_file = NULL;
    if ((arithmetic_file = fopen(DALE_LIBRARY_PATH "/libarithmetic.so", "r"))) {
        libarithmetic_path = DALE_LIBRARY_PATH "/libarithmetic.so";
    } else if ((arithmetic_file = fopen("./libarithmetic.so", "r"))) {
        libarithmetic_path = "./libarithmetic.so";
    } else {
        error("unable to find libarithmetic.so");
    }
    int res = fclose(arithmetic_file);
    if (res != 0) {
        error("unable to close %s", libarithmetic_path, true);
    }
    return libarithmetic_path;
}

Variable *processVariable(Units *units, Function *fn, Node *top,
                          ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    std::string unused_name;
    units->top()->getUnusedVarName(&unused_name);

    std::string var_name("_");
    Variable *var = ctx->getVariable(var_name.c_str());
    if (!var) {
        var = new Variable();
        var->name.append(var_name);
        var->once_tag = units->top()->once_tag;
        var->linkage = Linkage::Intern;
    }
    var->symbol.clear();
    var->symbol.append(unused_name);
    var->type = pr->type;
    llvm::Type *llvm_type = ctx->toLLVMType(pr->type, top, false);
    llvm::GlobalVariable *llvm_var = llvm::cast<llvm::GlobalVariable>(
        units->top()->module->getOrInsertGlobal(unused_name.c_str(),
                                                llvm_type));
    llvm_var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    if (pr->type->points_to) {
        llvm_var->setInitializer(getNullPointer(llvm_type));
    } else if (pr->type->struct_name.size() || pr->type->is_array) {
        llvm_var->setInitializer(
            llvm::ConstantAggregateZero::get(llvm_type));
    } else if (pr->type->isIntegerType() ||
               (pr->type->base_type == BaseType::Bool)) {
        llvm_var->setInitializer(ctx->nt->getConstantInt(
            llvm::IntegerType::get(*getContext(),
                                   ctx->nt->internalSizeToRealSize(
                                       pr->type->getIntegerSize())),
            "0"));
    } else if (pr->type->isFloatingPointType()) {
        llvm::ConstantFP *const_float = llvm::ConstantFP::get(
            *getContext(), llvm::APFloat(static_cast<float>(0)));
        llvm_var->setInitializer(
            llvm::cast<llvm::Constant>(const_float));
    }

    var->value = llvm::cast<llvm::Value>(llvm_var);

    ParseResult var_pr;
    var_pr.set(pr->block, ctx->tr->getPointerType(var->type),
               var->value);

    ParseResult setf_pr;
    bool res =
        FormProcSetfProcess(units, fn, pr->block, top, top, false,
                            false, &var_pr, pr, &setf_pr);
    if (!res) {
        return NULL;
    }

    return var;
}

bool REPLLoop(Units *units) {
    Context *ctx = units->top()->ctx;
    ErrorReporter *er = ctx->er;

    int error_count = er->getErrorTypeCount(ErrorType::Error);
    fprintf(stdout, "> ");
    fflush(stdout);

    Node *top = units->top()->parser->getNextNode();
    if (er->getErrorTypeCount(ErrorType::Error) > error_count) {
        er->flush();
        return true;
    }
    if (!top) {
        er->flush();
        return false;
    }

    if (!top->is_token && !top->is_list) {
        units->pop();
        return (!units->empty());
    }

    if (top->is_list && top->list->size()) {
        Node *first = top->list->at(0);
        if (first->is_token) {
            std::string *first_str = &(first->token->str_value);
            if ((first_str->compare("def") == 0) ||
                (first_str->compare("do") == 0) ||
                (first_str->compare("namespace") == 0) ||
                (first_str->compare("include") == 0) ||
                (first_str->compare("import") == 0)) {
                FormTopLevelInstParse(units, top);
                er->flush();
                return true;
            }
        }
    }

    Function *fn = createFunction(units, ctx->tr->type_void, top);
    llvm::Function *llvm_fn = fn->llvm_function;
    std::string fn_name = fn->symbol;

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node *> nodes;
    nodes.push_back(top);

    ctx->enableRetrievalLog();
    Function *temp_fn = new Function();
    temp_fn->llvm_function = llvm_fn;
    units->top()->pushGlobalFunction(temp_fn);
    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*getContext(), "entry", llvm_fn);

    ParseResult res_pr;
    bool res = FormProcInstParse(units, fn, block, top, false, false,
                                 NULL, &res_pr);
    if (!res) {
        er->flush();
        llvm_fn->eraseFromParent();
        ctx->deactivateNamespace(anon_name.c_str());
        units->top()->popGlobalFunction();
        return true;
    }

    std::string var_name("_");
    bool exists = (ctx->getVariable(var_name.c_str()) != NULL);

    Variable *var = NULL;
    llvm::GlobalVariable *llvm_var = NULL;
    if (res_pr.type->base_type != BaseType::Void) {
        var = processVariable(units, fn, top, &res_pr);
        if (!var) {
            ctx->er->flush();
            fn->llvm_function->eraseFromParent();
            ctx->deactivateNamespace(anon_name.c_str());
            units->top()->popGlobalFunction();
            return true;
        }
        llvm_var = llvm::cast<llvm::GlobalVariable>(var->value);
    }

    std::string type_str;
    res_pr.type->toString(&type_str);
    fprintf(stderr, "%s\n", type_str.c_str());

    llvm::IRBuilder<> builder(res_pr.block);
    builder.CreateRetVoid();

    res = resolveDeferredGotos(ctx, top, fn, res_pr.block);
    if (!res) {
        return false;
    }

    res = terminateBlocks(ctx, fn, llvm_fn, res_pr.getValue(ctx),
                          res_pr.type, top);
    if (!res) {
        return false;
    }

    removePostTerminators(llvm_fn);

    ctx->deactivateNamespace(anon_name.c_str());
    if (!exists) {
        res = ctx->addVariable(var_name.c_str(), var);
        if (!res) {
            fprintf(stderr, "Internal error: cannot add variable.\n");
            abort();
        }
    }

    units->top()->popGlobalFunction();

    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count_begin != error_count_end) {
        llvm_fn->replaceAllUsesWith(
            llvm::UndefValue::get(llvm_fn->getType()));
        llvm_fn->eraseFromParent();
    } else {
        cloneModuleIfRequired(units->top());
        llvm_fn =
            units->top()->ee->FindFunctionNamed(fn_name.c_str());
        std::vector<llvm::GenericValue> values;
#if D_LLVM_VERSION_ORD >= 34
        units->top()->ee->getFunctionAddress(fn_name.c_str());
#endif

        units->top()->ee->runFunction(llvm_fn, values);
        llvm_fn->eraseFromParent();

#if D_LLVM_VERSION_ORD >= 36
        if (res_pr.type->base_type != BaseType::Void) {
            uint64_t address = units->top()->ee->getGlobalValueAddress(
                var->symbol.c_str());
            int size;
            llvm::Constant *parsed = decodeRawData(
                units, top, reinterpret_cast<char *>(address),
                res_pr.type, &size, units->top()->module);
            llvm_var->setInitializer(parsed);
        }
#endif
    }

    return true;
}

void REPL::run(std::vector<const char *> *compile_lib_paths,
               std::vector<const char *> *include_paths,
               std::vector<const char *> *module_paths, int debug,
               int no_common, int no_dale_stdlib,
               int print_expansions) {
    init_introspection_functions();

    std::vector<std::string> shared_object_paths;
    std::vector<const char *> static_module_names;
    Module::Reader mr(module_paths, &shared_object_paths, include_paths,
                      &static_module_names, false, false);

    for (std::vector<const char *>::iterator
             b = compile_lib_paths->begin(),
             e = compile_lib_paths->end();
         b != e; ++b) {
        mr.addDynamicLibrary((*b), false, false);
    }

    if (!no_dale_stdlib) {
        const char *libdrt_path = getLibDRTPath();
        mr.addDynamicLibrary(libdrt_path, false, false);
        shared_object_paths.push_back(libdrt_path);

        const char *libarithmetic_path = getLibArithmeticPath();
        mr.addDynamicLibrary(libarithmetic_path, false, false);
        shared_object_paths.push_back(libarithmetic_path);
    }

    Units units(&mr);
    units.cto = false;
    units.no_common = no_common;
    units.no_drt = no_dale_stdlib;
    units.print_expansions = print_expansions;
    units.debug = debug;

    Context *ctx = NULL;
    llvm::Module *mod = NULL;
    llvm::Linker *linker = NULL;

    ErrorReporter er("");
    NativeTypes nt;
    TypeRegister tr;
    bool x86_64 = is_x86_64();

    Unit *unit = new Unit("/dev/stdin", &units, &er, &nt, &tr, NULL,
                          x86_64, NULL, NULL, NULL, NULL, NULL, true);

    units.push(unit);
    ctx = unit->ctx;
    mod = unit->module;
    linker = unit->linker;

    llvm::Triple triple(mod->getTargetTriple());
    if (triple.getTriple().empty()) {
        triple.setTriple(getTriple());
    }

    setDataLayout(mod, x86_64);
    DECLARE_ENGINE_BUILDER(mod, eb);

    eb.setEngineKind(llvm::EngineKind::JIT);
    std::string error;
    eb.setErrorStr(&error);
    llvm::ExecutionEngine *ee = eb.create();
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
        if (no_dale_stdlib) {
            unit->addCommonDeclarations();
        } else {
            std::vector<const char *> import_forms;
            mr.run(ctx, linker, mod, nullNode(), "drt", &import_forms);
            units.top()->mp->setPoolfree();
        }
    }

    for (;;) {
        bool res = REPLLoop(&units);
        if (!res) {
            break;
        }
    }
}
}
