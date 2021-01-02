#include "Unit.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../CommonDecl/CommonDecl.h"
#include "../Lexer/Lexer.h"
#include "../Utils/Utils.h"
#include "../llvmUtils/llvmUtils.h"
#include "../llvm_Linker.h"
#include "../llvm_Module.h"

namespace dale {
Unit::Unit(const char *path, Units *units, ErrorReporter *er,
           NativeTypes *nt, TypeRegister *tr, llvm::ExecutionEngine *ee,
           bool is_x86_64, Context *ctx, MacroProcessor *mp,
           FunctionProcessor *fp, llvm::Module *module,
           llvm::Linker *linker, bool line_buffered) {
    FILE *mfp = fopen(path, "r");
    if (!mfp) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "unable to open %s for reading",
                 path);
        error(buf, true);
    }

    if (!module) {
        has_own_module = true;
    }

    path = strdup(path);
    er->current_filename = path;
    dnc = new DNodeConverter(er);

    if (!ctx) {
        ctx = new Context(er, nt, tr);
        mp = new MacroProcessor(units, ctx, ee);
        fp = new FunctionProcessor(units);
    }
    this->ctx = ctx;
    this->mp = mp;
    this->fp = fp;

    Lexer *lxr = new Lexer(mfp, line_buffered);
    parser = new Parser(lxr, er, path);

    if (!module) {
        module = new llvm::Module(path, *getContext());
    }
    this->module = module;

    if (!linker) {
        linker = newLinker(path, module);
    }
    this->linker = linker;

    this->ee = ee;
    this->is_x86_64 = is_x86_64;
    var_count = 0;
    fn_count = 0;

    /* For now, the unused prefix name will be a randomly-generated
     * string. */
    for (int i = 0; i < 4; i++) {
        unused_name_prefix[i] = (rand() % 25 + 97);
    }

    global_function = NULL;
    global_block = NULL;
}

Unit::Unit(Units *units, ErrorReporter *er,
           NativeTypes *nt, TypeRegister *tr, llvm::ExecutionEngine *ee,
           bool is_x86_64, Context *ctx, MacroProcessor *mp,
           FunctionProcessor *fp, llvm::Module *module,
           llvm::Linker *linker, bool line_buffered) {
    if (!module) {
        has_own_module = true;
    }

    er->current_filename = "";
    dnc = new DNodeConverter(er);

    if (!ctx) {
        ctx = new Context(er, nt, tr);
        mp = new MacroProcessor(units, ctx, ee);
        fp = new FunctionProcessor(units);
    }
    this->ctx = ctx;
    this->mp = mp;
    this->fp = fp;

    if (!module) {
        module = new llvm::Module("", *getContext());
    }
    this->module = module;

    if (!linker) {
        linker = newLinker("", module);
    }
    this->linker = linker;

    this->ee = ee;
    this->is_x86_64 = is_x86_64;
    var_count = 0;
    fn_count = 0;

    /* For now, the unused prefix name will be a randomly-generated
     * string. */
    for (int i = 0; i < 4; i++) {
        unused_name_prefix[i] = (rand() % 25 + 97);
    }

    global_function = NULL;
    global_block = NULL;
}

Unit::~Unit() {
    if (hasOwnModule()) {
        delete ctx;
    }
    delete parser;
    delete dnc;
}

bool Unit::hasOnceTag() { return (once_tag.size() ? true : false); }

bool Unit::setOnceTag(std::string new_once_tag) {
    if (hasOnceTag()) {
        return false;
    }

    once_tag = new_once_tag;
    return true;
}

Function *Unit::getGlobalFunction() { return global_function; }

llvm::BasicBlock *Unit::getGlobalBlock() { return global_block; }

void Unit::pushGlobalFunction(Function *fn) {
    global_functions.push_back(fn);
    global_function = fn;
}

void Unit::pushGlobalBlock(llvm::BasicBlock *block) {
    global_blocks.push_back(block);
    global_block = block;
}

void Unit::popGlobalFunction() {
    global_functions.pop_back();
    if (global_functions.size()) {
        global_function = global_functions.back();
    } else {
        global_function = NULL;
    }
}

void Unit::popGlobalBlock() {
    global_blocks.pop_back();
    if (global_blocks.size()) {
        global_block = global_blocks.back();
    } else {
        global_block = NULL;
    }
}

void Unit::makeTemporaryGlobalFunction() {
    llvm::Type *llvm_return_type =
        ctx->toLLVMType(ctx->tr->type_int, NULL, false);

    std::vector<llvm::Type *> empty_args;
    llvm::FunctionType *ft =
        getFunctionType(llvm_return_type, empty_args, false);

    char buf[32];
    snprintf(buf, sizeof(buf), "_intro%d", fn_count++);

    std::string new_name;
    ctx->ns()->nameToSymbol(buf, &new_name);

    assert(!module->getFunction(llvm::StringRef(new_name.c_str())) &&
           "function already exists in module");

    llvm::Function *llvm_fn = llvm::Function::Create(
        ft, ctx->toLLVMLinkage(Linkage::Intern), new_name.c_str(),
        module);
    assert(llvm_fn && "unable to create function");

    std::vector<Variable *> vars;
    Function *fn = new Function(ctx->tr->type_int, &vars, llvm_fn, 0,
                                new std::string(new_name), 0);
    fn->linkage = Linkage::Intern;
    fn->cto = true;

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*getContext(), "entry", llvm_fn);

    pushGlobalFunction(fn);
    pushGlobalBlock(block);

    ctx->activateAnonymousNamespace();
}

void Unit::removeTemporaryGlobalFunction() {
    ctx->deactivateAnonymousNamespace();
    Function *current = getGlobalFunction();
    if (!current) {
        fprintf(stderr, "No temporary global function found\n");
        return;
    }
    popGlobalFunction();
    popGlobalBlock();
    if (!current->llvm_function) {
        fprintf(stderr, "No temporary global LLVM function found\n");
        return;
    }
    current->llvm_function->eraseFromParent();
}

static bool added_common_declarations = false;

void Unit::addCommonDeclarations() {
    CommonDecl::addBasicTypes(this, is_x86_64);

    /* The basic math functions and the varargs functions are
     * added to every module, but the structs are not, because
     * they can merge backwards and forwards (the other stuff has
     * internal linkage). */

    if (added_common_declarations) {
        return;
    }
    added_common_declarations = true;

    CommonDecl::addVarargsTypes(this, is_x86_64);
    CommonDecl::addStandardVariables(this);

    return;
}

void Unit::getUnusedVarName(std::string *buf) {
    char ibuf[16];
    do {
        snprintf(ibuf, sizeof(ibuf), "_dv%c%c%c%c%d",
                 unused_name_prefix[0], unused_name_prefix[1],
                 unused_name_prefix[2], unused_name_prefix[3],
                 var_count++);
    } while (module->getGlobalVariable(llvm::StringRef(ibuf)));

    buf->append(ibuf);
    return;
}

void Unit::getUnusedFunctionName(std::string *buf) {
    char ibuf[16];
    do {
        snprintf(ibuf, sizeof(ibuf), "_fn%c%c%c%c%d",
                 unused_name_prefix[0], unused_name_prefix[1],
                 unused_name_prefix[2], unused_name_prefix[3],
                 fn_count++);
    } while (module->getFunction(llvm::StringRef(ibuf)));

    buf->append(ibuf);
    return;
}

bool Unit::hasOwnModule() { return has_own_module; }
}
