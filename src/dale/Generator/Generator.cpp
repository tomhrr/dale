#include "Generator.h"
#include "Config.h"

#if LLVM_VERSION_MAJOR >= 3
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/system_error.h"
#elif LLVM_VERSION_MAJOR < 2
#error "LLVM >= 2.8 is required."
#elif LLVM_VERSION_MINOR < 8
#error "LLVM >= 2.8 is required."
#elif LLVM_VERSION_MINOR == 8
#include "llvm/System/DynamicLibrary.h"
#include "llvm/System/Host.h"
#include "llvm/System/Signals.h"
#else
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/system_error.h"
#endif

#include "llvm/PassManager.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetData.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "../STLUtils/STLUtils.h"
#include "../Serialise/Serialise.h"
#include "../NativeTypes/NativeTypes.h"
#include "../ContextSavePoint/ContextSavePoint.h"
#include "../Module/Writer/Writer.h"
#include "../Form/Goto/Goto.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <setjmp.h>
#include <float.h>
#include FFI_HEADER

#define DALE_DEBUG 0

#define IMPLICIT 1

#define eq(str) !strcmp(t->str_value.c_str(), str)

#define DECIMAL_RADIX 10

#define STRTOUL_FAILED(ret, str, end) \
    (((((ret) == ULONG_MAX || ((ret) == 0)) && (errno == ERANGE)) \
        || (((ret) == 0) && ((str) == (end)))))

int core_forms_max = 35;
const char *core_forms_strs[36] = {
    "goto", "label", "return", "setf", "@", ":", "#", "$",
    "get-dnodes", "p=", "p+", "p-", "p<", "p>", "def", "if", "null",
    "nullptr", "do", "cast", "va-arg", "sizeof", "offsetof",
    "va-start", "va-end",
    "alignmentof", "funcall", "using-namespace", "new-scope",
    "array-of", "setv", "@$", "@:", ":@", "@:@", NULL
};

int core_forms_no_max = 31;
const char *core_forms_no[32] = {
    "goto", "label", "return", ":", "get-dnodes", "p=", "p+", "p-", "p<",
    "p>", "def", "if", "null", "nullptr", "do", "cast", "va-arg",
    "va-start", "va-end",
    "sizeof", "offsetof", "alignmentof", "funcall", "using-namespace",
    "new-scope", "array-of",  "setv", "@$", ":@", "@:", "@:@", NULL
};

std::vector<dale::Element::Function*> global_functions;
std::vector<llvm::BasicBlock*>        global_blocks;

dale::Element::Function *global_function;
llvm::BasicBlock        *global_block;

extern "C" {
    void *find_introspection_function(const char *);
}

namespace dale
{
int isValidModuleName(std::string *str)
{
    int i;
    for (i = 0; i < (int) str->length(); ++i) {
        char c = (*str)[i];
        if (!(isalnum(c) || (c == '-') || (c == '_') || (c == '.'))) {
            return 0;
        }
    }
    return 1;
}

void failedDaleToLLVMTypeConversion(Element::Type *type)
{
    /* Currently a no-op: this may change in the future. */
}

/* These should be treated as const, except by the code that
 * initialises everything (i.e. once this is actually processing
 * code, these shouldn't change). */

Element::Type *type_void;
Element::Type *type_varargs;
Element::Type *type_int;
Element::Type *type_intptr;
Element::Type *type_size;
Element::Type *type_ptrdiff;
Element::Type *type_uint;
Element::Type *type_char;
Element::Type *type_pchar;
Element::Type *type_pvoid;
Element::Type *type_bool;
Element::Type *type_float;
Element::Type *type_double;
Element::Type *type_longdouble;

Element::Type *type_int8;
Element::Type *type_uint8;
Element::Type *type_int16;
Element::Type *type_uint16;
Element::Type *type_int32;
Element::Type *type_uint32;
Element::Type *type_int64;
Element::Type *type_uint64;
Element::Type *type_int128;
Element::Type *type_uint128;

Element::Type *type_dnode = NULL;
Element::Type *type_pdnode = NULL;
void (*pool_free_fptr)(MContext *) = NULL;
llvm::Type *llvm_type_dnode = NULL;
llvm::Type *llvm_type_pdnode = NULL;

llvm::Value *llvm_bool_true  = NULL;
llvm::Value *llvm_bool_false = NULL;

llvm::Value *llvm_native_zero = NULL;
llvm::Value *llvm_native_one  = NULL;

int nesting = 0;
int g_no_acd;
int g_nodrt;

std::vector<llvm::Value *> two_zero_indices;

int is_x86_64;
int has_defined_extern_macro;

std::set<std::string> *core_forms;
std::set<std::string> *core_forms_no_override;

char *inc_paths[100];
int inc_path_count = 0;

char *mod_paths[100];
int mod_path_count = 0;

Namespace *prefunction_ns;

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
    prefunction_ns = NULL;

    llvm::InitializeNativeTarget();
    llvm::InitializeAllAsmPrinters();

    nt = new NativeTypes();
    tr = new TypeRegister();

    included_inodes    = new std::multiset<ino_t>;
    included_once_tags = new std::set<std::string>;
    included_modules   = new std::set<std::string>;
    set_module_name    = 0;

    modules         = new std::vector<llvm::Module *>;
    dtm_modules     = new std::map<std::string, llvm::Module*>;
    dtm_nm_modules  = new std::map<std::string, std::string>;
    linkers         = new std::vector<llvm::Linker *>;
    parsers         = new std::vector<Parser *>;
    contexts        = new std::vector<Context *>;

    type_bool        = tr->getBasicType(Type::Bool);
    type_void        = tr->getBasicType(Type::Void);
    type_varargs     = tr->getBasicType(Type::VarArgs);
    type_int         = tr->getBasicType(Type::Int);
    type_intptr      = tr->getBasicType(Type::IntPtr);
    type_size        = tr->getBasicType(Type::Size);
    type_ptrdiff     = tr->getBasicType(Type::PtrDiff);
    type_uint        = tr->getBasicType(Type::UInt);
    type_char        = tr->getBasicType(Type::Char);
    type_float       = tr->getBasicType(Type::Float);
    type_double      = tr->getBasicType(Type::Double);
    type_longdouble  = tr->getBasicType(Type::LongDouble);

    type_int8    = tr->getBasicType(Type::Int8);
    type_uint8   = tr->getBasicType(Type::UInt8);
    type_int16   = tr->getBasicType(Type::Int16);
    type_uint16  = tr->getBasicType(Type::UInt16);
    type_int32   = tr->getBasicType(Type::Int32);
    type_uint32  = tr->getBasicType(Type::UInt32);
    type_int64   = tr->getBasicType(Type::Int64);
    type_uint64  = tr->getBasicType(Type::UInt64);
    type_int128  = tr->getBasicType(Type::Int128);
    type_uint128 = tr->getBasicType(Type::UInt128);

    type_pchar  = tr->getPointerType(type_char);

    llvm_bool_true =
        llvm::ConstantInt::get(
            llvm::IntegerType::get(llvm::getGlobalContext(), 1), 1
        );
    llvm_bool_false =
        llvm::ConstantInt::get(
            llvm::IntegerType::get(llvm::getGlobalContext(), 1), 0
        );
    llvm_native_zero =
        llvm::ConstantInt::get(nt->getNativeIntType(), 0);
    llvm_native_one =
        llvm::ConstantInt::get(nt->getNativeIntType(), 1);

    two_zero_indices.clear();
    stl::push_back2(&two_zero_indices,
                    llvm_native_zero, llvm_native_zero);

    /* On OS X, SYSTEM_PROCESSOR is i386 even when the underlying
     * processor is x86-64, hence the extra check here. */
    is_x86_64 =
        ((!strcmp(SYSTEM_PROCESSOR, "x86_64"))
         || ((!strcmp(SYSTEM_NAME, "Darwin"))
             && (sizeof(char *) == 8)));

    has_defined_extern_macro = 0;

    core_forms = new std::set<std::string>;
    for (int i = 0; i < core_forms_max; i++) {
        core_forms->insert(core_forms_strs[i]);
    }

    core_forms_no_override = new std::set<std::string>;
    for (int i = 0; i < core_forms_no_max; i++) {
        core_forms_no_override->insert(core_forms_no[i]);
    }

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

    dale::stl::deleteElements(parsers);
    dale::stl::deleteElements(contexts);
    dale::stl::deleteElements(modules);

    delete parsers;
    delete linkers;
    delete contexts;
    delete core_forms;
    delete core_forms_no_override;
    delete cto_modules;
    delete modules;
    delete dtm_modules;
    delete dtm_nm_modules;
}

void setPr(ParseResult *pr, llvm::BasicBlock *block, Element::Type *type,
           llvm::Value *value)
{
    pr->block = block;
    pr->type  = type;
    pr->value = value;
}

llvm::Module *loadModule(std::string *path)
{
    const llvm::sys::Path sys_path(*path);

    llvm::OwningPtr<llvm::MemoryBuffer> buffer;
    llvm::MemoryBuffer::getFileOrSTDIN(sys_path.c_str(), buffer);

    std::string errmsg;
    llvm::Module *module = 
        llvm::getLazyBitcodeModule(buffer.get(),
                                   llvm::getGlobalContext(),
                                   &errmsg);

    if (!module) {
        fprintf(stderr,
                "Internal error: cannot load module: %s\n",
                errmsg.c_str());
        abort();
    }

    return module;
}

llvm::FunctionType *getFunctionType(llvm::Type *t,
                                    std::vector<llvm::Type*> &v,
                                    bool b) {
    llvm::ArrayRef<llvm::Type*> temp(v);
    return llvm::FunctionType::get(t, temp, b);
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

Parser *Generator::newParser(FILE *new_fp, const char *filename)
{
    std::vector<Token *> *ungot_tokens = new std::vector<Token *>;
    Lexer *lxr = new Lexer(new_fp, ungot_tokens, 1, 1);

    return new Parser(lxr, erep, filename);
}

llvm::ConstantInt *Generator::getNativeInt(int n)
{
    return llvm::ConstantInt::get(nt->getNativeIntType(), n);
}

llvm::ConstantInt *Generator::getConstantInt(
    llvm::IntegerType *type,
    const char *numstr
) {
    int len = strlen(numstr);
    int radix = 10;
    if ((len >= 3) && (numstr[0] == '0') && (numstr[1] == 'x')) {
        numstr += 2;
        radix = 16;
    }

    return llvm::ConstantInt::get(type,
                                  llvm::StringRef(numstr),
                                  radix);
}

int Generator::addVariable(const char *name,
                           Element::Type *type,
                           llvm::Constant *init,
                           bool ignore_if_present)
{
    Element::Variable *var = new Element::Variable();
    var->name.append(name);
    var->type = type;
    var->internal_name.append(name);
    var->linkage = Linkage::Extern;
    int avres = ctx->ns()->addVariable(name, var);
    if (!avres) {
        if (ignore_if_present) {
            return 1;
        }
        fprintf(stderr, "Unable to add %s.\n", name);
        abort();
    }

    llvm::Type *rdttype =
        toLLVMType(type, NULL, false, false);
    if (!rdttype) {
        failedDaleToLLVMTypeConversion(type);
        return 0;
    }

    llvm::GlobalVariable *lvar =
        llvm::cast<llvm::GlobalVariable>(
            mod->getOrInsertGlobal(name,
                                   rdttype)
        );

    lvar->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_Weak));
    lvar->setInitializer(init);
    var->value = lvar;

    return 1;
}

static int added_common_declarations = 0;

void
Generator::addVoidPointerType(void)
{
    /* (def _vp (struct extern ((p char)))) */

    Token *mychar = new Token(TokenType::String,0,0,0,0);
    mychar->str_value.append("char");
    Token *myp = new Token(TokenType::String,0,0,0,0);
    myp->str_value.append("p");

    std::vector<Node*> *pp = new std::vector<Node*>;
    stl::push_back2(pp, new Node(myp),
                    new Node(mychar));

    std::vector<Node*> *pp2 = new std::vector<Node*>;
    pp2->push_back(new Node(pp));

    Token *mys = new Token(TokenType::String,0,0,0,0);
    mys->str_value.append("struct");
    Token *mye = new Token(TokenType::String,0,0,0,0);
    mye->str_value.append("extern");

    std::vector<Node*> *pp3 = new std::vector<Node*>;
    stl::push_back3(pp3, new Node(mys),
                    new Node(mye),
                    new Node(pp2));

    parseStructDefinition("_vp", new Node(pp3));
    erep->flush();
}

void
Generator::addVarargsFunctions(void)
{
    std::vector<llvm::Type*> va_start_args;
    va_start_args.push_back(toLLVMType(type_pchar, NULL, false));

    llvm::FunctionType *va_start_ft =
        getFunctionType(
            toLLVMType(type_void, NULL, true),
            va_start_args,
            false
        );

    llvm::Function *va_start_fn =
        llvm::cast<llvm::Function>(
            mod->getOrInsertFunction(
                "llvm.va_start",
                va_start_ft
            )
        );

    va_start_fn->setCallingConv(llvm::CallingConv::C);

    llvm::Function *va_end_fn =
        llvm::cast<llvm::Function>(
            mod->getOrInsertFunction(
                "llvm.va_end",
                va_start_ft
            )
        );

    va_end_fn->setCallingConv(llvm::CallingConv::C);

    return;
}

void Generator::addCommonDeclarations(void)
{
    addVoidPointerType();

    BasicTypes::addSignedInt(ctx, mod, &current_once_tag, type_int);
    BasicTypes::addSignedInt(ctx, mod, &current_once_tag, type_char);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_uint);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_intptr);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_ptrdiff);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_size);

    BasicTypes::addSignedInt(ctx, mod, &current_once_tag, type_int8);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_uint8);
    BasicTypes::addSignedInt(ctx, mod, &current_once_tag, type_int16);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_uint16);
    BasicTypes::addSignedInt(ctx, mod, &current_once_tag, type_int32);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_uint32);
    BasicTypes::addSignedInt(ctx, mod, &current_once_tag, type_int64);
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_uint64);

    /* i128 (actually any integer type with a size of more than 64
     * bits) does not work properly in some respects on x86-32
     * (see http://llvm.org/bugs/show_bug.cgi?id=2660). Rather
     * than allowing the hobbled support to be had, disable it
     * completely. */

    if (is_x86_64) {
        BasicTypes::addSignedInt(ctx, mod, &current_once_tag, type_int128);
        BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, type_uint128);
    }

    BasicTypes::addFloatingPoint(ctx, mod, &current_once_tag, type_float);
    BasicTypes::addFloatingPoint(ctx, mod, &current_once_tag, type_double);
    BasicTypes::addFloatingPoint(ctx, mod, &current_once_tag, type_longdouble);

    /* The basic math functions and the varargs functions are
     * added to every module, but the structs are not, because
     * they can merge backwards and forwards (the other stuff has
     * internal linkage). */

    if (added_common_declarations) {
        return;
    }
    added_common_declarations = 1;

    if (is_x86_64) {
        prsr->lxr->pushText(
            "(def va-list "
              "(struct extern ((a uint32) "
                              "(b uint32) "
                              "(c (p char)) "
                              "(d (p char)))))"
        );
    } else {
        prsr->lxr->pushText(
            "(def va-list "
              "(struct extern ((a uint32))))"
        );
    }

    /* Add jmp_buf size constant. */

    addVariable("JMP_BUF_SIZE",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(jmp_buf)));

    /* Sizeof fpos_t. */

    addVariable("FPOS_T",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(fpos_t)));

    /* Sizeof time_t. */

    addVariable("TIME_T",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(time_t)));

    /* Sizeof clock_t. */

    addVariable("CLOCK_T",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(clock_t)));

    /* Sizeof long. */

    addVariable("SIZEOF_LONG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(long)));

    /* Sizeof long long. */

    addVariable("SIZEOF_LONG_LONG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(long long)));

    /* Add float.h constants. */

    addVariable("FLT_RADIX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_RADIX));
    addVariable("FLT_MANT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_MANT_DIG));
    addVariable("FLT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_DIG));
    addVariable("FLT_EPSILON",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(llvm::getGlobalContext()),
                                      FLT_EPSILON));
    addVariable("FLT_ROUNDS",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_ROUNDS));
    addVariable("FLT_MIN_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_MIN_EXP));
    addVariable("FLT_MAX_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_MAX_EXP));
    addVariable("FLT_MIN",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(llvm::getGlobalContext()),
                                      FLT_MIN));
    addVariable("FLT_MAX",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(llvm::getGlobalContext()),
                                      FLT_MAX));

    addVariable("DBL_MANT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_MANT_DIG));
    addVariable("DBL_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_DIG));
    addVariable("DBL_EPSILON",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                                      DBL_EPSILON));
    addVariable("DBL_MIN_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_MIN_EXP));
    addVariable("DBL_MAX_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_MAX_EXP));
    addVariable("DBL_MIN",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                                      DBL_MIN));
    addVariable("DBL_MAX",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                                      DBL_MAX));

    addVariable("LDBL_MANT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_MANT_DIG));
    addVariable("LDBL_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_DIG));
    addVariable("LDBL_EPSILON",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(llvm::getGlobalContext()),
                                      LDBL_EPSILON));
    addVariable("LDBL_MIN_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_MIN_EXP));
    addVariable("LDBL_MAX_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_MAX_EXP));
    addVariable("LDBL_MIN",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(llvm::getGlobalContext()),
                                      LDBL_MIN));
    addVariable("LDBL_MAX",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(llvm::getGlobalContext()),
                                      LDBL_MAX));

    /* Add floating-point infinity constants for float, double and
     * long-double. */

    addVariable("HUGE_VAL",
                type_double,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getDoubleTy(llvm::getGlobalContext())
                ));

    addVariable("HUGE_VALF",
                type_float,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getFloatTy(llvm::getGlobalContext())
                ));

    addVariable("HUGE_VALL",
                type_longdouble,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getX86_FP80Ty(llvm::getGlobalContext())
                ));

    /* Other misc. constants. */

    addVariable("L_tmpnam",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       L_tmpnam));
    addVariable("TMP_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       TMP_MAX));
    addVariable("FILENAME_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FILENAME_MAX));
    addVariable("FOPEN_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FOPEN_MAX));
    addVariable("RAND_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       RAND_MAX));
    addVariable("EXIT_FAILURE",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       EXIT_FAILURE));
    addVariable("EXIT_SUCCESS",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       EXIT_SUCCESS));

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
    g_no_acd = no_acd;
    g_nodrt  = nodrt;

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
    global_function  = NULL;
    global_block     = NULL;

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
        erep->current_filename = filename;

        ctx  = new Context(erep, nt, tr);
        FILE *new_fp = fopen(filename, "r");
        if (!new_fp) {
            perror("Unable to open file");
            return 0;
        }

        prsr = newParser(new_fp, filename);

        mod = new llvm::Module(filename, llvm::getGlobalContext());
        linker = new llvm::Linker(filename, mod, false);

        llvm::Triple TheTriple(mod->getTargetTriple());
        if (TheTriple.getTriple().empty()) {
            TheTriple.setTriple(llvm::sys::getHostTriple());
        }

        llvm::EngineBuilder eb = llvm::EngineBuilder(mod);
        eb.setEngineKind(llvm::EngineKind::JIT);
        ee = eb.create();

        if (ee == NULL) {
            fprintf(stderr,
                    "Internal error: cannot create execution "
                    "engine.\n");
            abort();
        }

        ee->InstallLazyFunctionCreator(myLFC);
        addVarargsFunctions();

        if (!no_acd) {
            if (nodrt) {
                addCommonDeclarations();
            } else {
                std::vector<const char*> import_forms;
                addDaleModule(nullNode(), "drt", &import_forms);
                setPdnode();
                setPoolfree();
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
                /* Check for remaining modules/parsers/contexts. */
                if (!modules->empty()) {
                    linker = linkers->back();
                    current_once_tag.clear();
                    linkers->pop_back();

                    std::string link_error;
                    if (linker->LinkInModule(mod, &link_error)) {
                        fprintf(stderr,
                                "Internal error: cannot link modules: "
                                "%s\n", link_error.c_str());
                        abort();
                    }

                    mod = modules->back();
                    modules->pop_back();

                    delete prsr;
                    prsr = parsers->back();
                    parsers->pop_back();

                    contexts->back()->merge(ctx);
                    ctx = contexts->back();
                    contexts->pop_back();

                    ctx->regetPointers(mod);

                    continue;
                }

                break;
            }
            parseTopLevel(top);
            erep->flush();
        } while (1);

        removeMacroTemporaries();

        if (remove_macros) {
            ctx->eraseLLVMMacros();
            has_defined_extern_macro = 0;
        }

        if (last_module) {
            std::string link_error;
            if (linker->LinkInModule(last_module, &link_error)) {
                Error *e = new Error(
                    "not applicable",
                    ErrorInst::Generator::CannotLinkModules,
                    0, 0, 0, 0
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
        GTheTriple.setTriple(llvm::sys::getHostTriple());
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

    std::string Features;
    std::auto_ptr<llvm::TargetMachine> target =
        std::auto_ptr<llvm::TargetMachine>
        (TheTarget->createTargetMachine(
             GTheTriple.getTriple(), llvm::sys::getHostCPUName(), Features
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
            const llvm::sys::Path bb(*b);
            bool is_native = false;
            if (linker->LinkInFile(bb, is_native)) {
                fprintf(stderr, "Internal error: unable to link "
                        "bitcode file.\n");
                abort();
            }
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

    PM.add(new llvm::TargetData(mod));
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
                            loadModule(&(b->second))
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
                    if (linker->LinkInModule(b->second, &err)) {
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
                    if (linker->LinkInModule(found->second, &err)) {
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
        ctx->eraseLLVMMacrosAndCTOFunctions();

        llvm::formatted_raw_ostream *temp_fro
        = new llvm::formatted_raw_ostream(temp,
            llvm::formatted_raw_ostream::DELETE_STREAM);

        if (produce == IR) {
            PM.add(llvm::createPrintModulePass(&temp));
        } else if (produce == ASM) {
            Target.setAsmVerbosityDefault(true);
            bool res = Target.addPassesToEmitFile(
                PM, *temp_fro, llvm::TargetMachine::CGFT_AssemblyFile, 
                OLvl, false);
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
        tr->dump();
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

/* The only time this returns 0 is on EOF - possibly problematic. */
int Generator::parseTopLevel(Node *top)
{
    /* Remove all anonymous namespaces. */
    ctx->deleteAnonymousNamespaces();

    if (!top) {
        return 0;
    }

    if (!top->is_token && !top->is_list) {
        /* Check for remaining modules/parsers/contexts. */
        if (!modules->empty()) {
            std::string link_error;
            linker = linkers->back();
            current_once_tag.clear();
            linkers->pop_back();

            if (linker->LinkInModule(mod, &link_error)) {
                fprintf(stderr,
                        "Internal error: cannot link modules: "
                        "%s\n", link_error.c_str());
                abort();
            }

            mod = modules->back();
            modules->pop_back();

            delete prsr;
            prsr = parsers->back();
            parsers->pop_back();

            contexts->back()->merge(ctx);
            ctx = contexts->back();
            contexts->pop_back();
            ctx->regetPointers(mod);

            return 1;
        }

        return 0;
    }

    if (!top->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::OnlyListsAtTopLevel, top
        );
        erep->addError(e);
        return 0;
    }

    symlist *lst = top->list;

    if (lst->size() == 0) {
        Error *e = new Error(
            ErrorInst::Generator::NoEmptyLists, top
        );
        erep->addError(e);
        return 0;
    }

    Node *n = (*lst)[0];

    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        erep->addError(e);
        return 0;
    }

    Token *t = n->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol, n
        );
        erep->addError(e);
        return 0;
    }

    if (!t->str_value.compare("do")) {
        if (lst->size() < 2) {
            Error *e = new Error(
                ErrorInst::Generator::NoEmptyDo,
                n
            );
            erep->addError(e);
            return 0;
        }

        std::vector<Node *>::iterator node_iter;
        node_iter = lst->begin();

        ++node_iter;

        while (node_iter != lst->end()) {
            parseTopLevel(*node_iter);
            erep->flush();
            ++node_iter;
        }

        return 1;
    } else if (!t->str_value.compare("def")) {
        parseDefine(top);
        return 1;
    } else if (!t->str_value.compare("namespace")) {
        parseNamespace(top);
        return 1;
    } else if (!t->str_value.compare("using-namespace")) {
        parseUsingNamespaceTopLevel(top);
        return 1;
    } else if (!t->str_value.compare("include")) {
        parseInclude(top);
        return 1;
    } else if (!t->str_value.compare("module")) {
        parseModuleName(top);
        return 1;
    } else if (!t->str_value.compare("import")) {
        parseImport(top);
        return 1;
    } else if (!t->str_value.compare("once")) {
        if (!assertArgNums("once", top, 1, 1)) {
            return 1;
        }
        symlist *lst = top->list;
        Node *n = (*lst)[1];
        n = parseOptionalMacroCall(n);
        if (!n) {
            return 1;
        }
        if (!assertArgIsAtom("once", n, "1")) {
            return 1;
        }
        const char *once_name = n->token->str_value.c_str();
        std::string once_tag(once_name);

        if (included_once_tags->find(once_tag) !=
                included_once_tags->end()) {
            if (parsers->size() == 0) {
                Error *e = new Error(
                    ErrorInst::Generator::CannotOnceTheLastOpenFile,
                    n
                );
                erep->addError(e);
                return 0;
            }
            delete prsr;
            prsr = parsers->back();
            parsers->pop_back();
            ctx = contexts->back();
            contexts->pop_back();
            mod = modules->back();
            modules->pop_back();
            linker = linkers->back();
            linkers->pop_back();
            current_once_tag.clear();
        }
        included_once_tags->insert(once_tag);
        current_once_tag = once_tag;

        return 1;
    } else {
        Node *newtop = parseOptionalMacroCall(top);
        if (!newtop) {
            return 0;
        }
        if (newtop != top) {
            return parseTopLevel(newtop);
        }
        Error *e = new Error(
            ErrorInst::Generator::NotInScope,
            n,
            t->str_value.c_str()
        );
        erep->addError(e);
        return 0;
    }
}

void Generator::parseUsingNamespaceTopLevel(Node *top)
{
    assert(top->list && "parseUsingNamespace must receive a list!");

    if (!assertArgNums("using-namespace", top, 1, -1)) {
        return;
    }

    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = parseOptionalMacroCall(n);
    if (!n) {
        return;
    }
    if (!assertArgIsAtom("using-namespace", n, "1")) {
        return;
    }
    if (!assertAtomIsSymbol("using-namespace", n, "1")) {
        return;
    }

    Token *t = n->token;

    int res = ctx->useNamespace(t->str_value.c_str());
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::NamespaceNotInScope,
            n,
            t->str_value.c_str()
        );
        erep->addError(e);
        return;
    }

    std::vector<Node *>::iterator symlist_iter;
    symlist_iter = lst->begin();

    /* Skip the namespace token and the name token/form. */

    ++symlist_iter;
    ++symlist_iter;

    while (symlist_iter != lst->end()) {
        parseTopLevel((*symlist_iter));
        erep->flush();
        ++symlist_iter;
    }

    ctx->unuseNamespace();
    //ctx->unuseNamespace(t->str_value.c_str());

    return;
}

void Generator::parseNamespace(Node *top)
{
    assert(top->list && "parseNamespace must receive a list!");

    if (!assertArgNums("namespace", top, 1, -1)) {
        return;
    }

    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = parseOptionalMacroCall(n);
    if (!n) {
        return;
    }
    if (!assertArgIsAtom("namespace", n, "1")) {
        return;
    }
    if (!assertAtomIsSymbol("namespace", n, "1")) {
        return;
    }

    Token *t = n->token;

    int success = ctx->activateNamespace(t->str_value.c_str());
    if (!success) {
        fprintf(stderr, "Internal error: cannot activate "
                "namespace '%s'.\n",
                t->str_value.c_str());
        abort();
    }

    std::vector<Node *>::iterator symlist_iter;
    symlist_iter = lst->begin();

    /* Skip the namespace token and the name token/form. */

    ++symlist_iter;
    ++symlist_iter;

    while (symlist_iter != lst->end()) {
        parseTopLevel((*symlist_iter));
        erep->flush();
        ++symlist_iter;
    }

    ctx->deactivateNamespace(t->str_value.c_str());

    return;
}

static int module_number = 0;

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

    llvm::Module *new_module = loadModule(&module_path);

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

void Generator::parseModuleName(Node *top)
{
    if (module_name.size() > 0) {
        fprintf(stderr, "Internal error: module name already set.\n");
        abort();
    }

    assert(top->list && "parseModuleName must receive a list!");

    if (!assertArgNums("module", top, 1, 2)) {
        return;
    }

    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = parseOptionalMacroCall(n);
    if (!n) {
        return;
    }
    if (!assertArgIsAtom("module", n, "1")) {
        return;
    }

    if (!isValidModuleName(&(n->token->str_value))) {
        Error *e = new Error(
            ErrorInst::Generator::InvalidModuleName,
            n,
            n->token->str_value.c_str()
        );
        erep->addError(e);
        return;
    }

    const char *my_module_name = n->token->str_value.c_str();

    if (lst->size() == 3) {
        n = (*lst)[2];
        n = parseOptionalMacroCall(n);
        if (!n) {
            return;
        }
        if (!assertArgIsList("module", n, "2")) {
            return;
        }
        if (!(n->list->at(0)->is_token)
                ||
                (n->list->at(0)->token->str_value.compare("attr"))) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                n,
                "attr",
                0,
                0
            );
            erep->addError(e);
            return;
        }

        symlist *attr_list = n->list;
        std::vector<Node*>::iterator b = attr_list->begin(),
                                     e = attr_list->end();
        ++b;
        for (; b != e; ++b) {
            if ((*b)->is_list) {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                erep->addError(e);
                return;
            }
            if (!((*b)->token->str_value.compare("cto"))) {
                cto = 1;
            } else {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                erep->addError(e);
                return;
            }
        }
    }

    module_name = std::string("lib");
    module_name.append(my_module_name);
    set_module_name = 1;

    return;
}

void Generator::parseImport(Node *top)
{
    assert(top->list && "parseImport must receive a list!");

    if (!assertArgNums("import", top, 1, 2)) {
        return;
    }

    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = parseOptionalMacroCall(n);
    if (!n) {
        return;
    }
    if (!assertArgIsAtom("import", n, "1")) {
        return;
    }

    const char *my_module_name = n->token->str_value.c_str();

    std::vector<const char *> import_forms;
    if (lst->size() == 3) {
        n = (*lst)[2];
        if (!assertArgIsList("import", n, "2")) {
            return;
        }
        symlist *formlst = n->list;
        for (symlist::iterator b = formlst->begin(),
                e = formlst->end();
                b != e;
                ++b) {
            if (!assertArgIsAtom("import", (*b), "2")) {
                return;
            }
            import_forms.push_back((*b)->token->str_value.c_str());
        }
    }

    int res = addDaleModule(top, my_module_name, &import_forms);
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::UnableToLoadModule,
            top,
            my_module_name
        );
        erep->addError(e);
        return;
    }

    return;
}

void Generator::parseInclude(Node *top)
{
    assert(top->list && "parseInclude must receive a list!");

    if (!assertArgNums("include", top, 1, 1)) {
        return;
    }

    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = parseOptionalMacroCall(n);
    if (!n) {
        return;
    }
    if (!assertArgIsAtom("include", n, "1")) {
        return;
    }
    if (!assertAtomIsStringLiteral("include", n, "1")) {
        return;
    }

    Token *t = n->token;

    std::string filename_buf(t->str_value.c_str());

    /* Check if the file exists in the current directory, or in ./include.
     * If it doesn't, go through each of the -I (inc_paths) directories.
     * If it doesn't exist in any of them, check DALE_INCLUDE_PATH (set at
     * compile time - used to be an environment variable).  Otherwise,
     * print an error and return nothing. */

    FILE *include_file = fopen(filename_buf.c_str(), "r");

    if (!include_file) {
        filename_buf.clear();
        filename_buf.append("./include/");
        filename_buf.append(t->str_value.c_str());
        include_file = fopen(filename_buf.c_str(), "r");
        if (!include_file) {
            int mi;
            for (mi = 0; mi < inc_path_count; ++mi) {
                filename_buf.clear();
                filename_buf.append(inc_paths[mi])
                .append("/")
                .append(t->str_value.c_str());
                include_file = fopen(filename_buf.c_str(), "r");
                if (include_file) {
                    break;
                }
            }
        }
        if (!include_file) {
            filename_buf.clear();
            filename_buf.append(DALE_INCLUDE_PATH)
            .append("/")
            .append(t->str_value.c_str());

            include_file = fopen(filename_buf.c_str(), "r");

            if (!include_file) {
                Error *e = new Error(
                    ErrorInst::Generator::FileError,
                    n,
                    filename_buf.c_str(),
                    strerror(errno)
                );
                erep->addError(e);
                return;
            }
        }
    }
    /* Add the current parser/module/context to their respective
     * stacks, create new parser/module/context for the new file.
     * */

    modules->push_back(mod);
    parsers->push_back(prsr);
    contexts->push_back(ctx);
    linkers->push_back(linker);

    prsr = newParser(include_file, filename_buf.c_str());
    Context *old_ctx = ctx;
    ctx = new Context(erep, nt, tr);
    ctx->merge(old_ctx);

    std::string module_name("MyModule");
    char buf[256];
    sprintf(buf, "%d", ++module_number);
    module_name.append(buf);

    mod = new llvm::Module(module_name.c_str(), llvm::getGlobalContext());
    linker = new llvm::Linker(module_name.c_str(), mod, false);

    ee->addModule(mod);
    addVarargsFunctions();

    if (!g_no_acd) {
        if (g_nodrt) {
            addCommonDeclarations();
        } else {
            std::vector<const char*> import_forms;
            addDaleModule(nullNode(), "drt", &import_forms);
            setPdnode();
        }
    }

    return;
}

void Generator::parseDefine(Node *top)
{
    assert(top->list && "parseDefine must receive a list!");

    symlist *lst = top->list;

    if (lst->size() != 3) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectNumberOfArgs,
            top,
            "def", 2, (int) (lst->size() - 1)
        );
        erep->addError(e);
        return;
    }

    Node *n = (*lst)[1];

    n = parseOptionalMacroCall(n);
    if (!n) {
        return;
    }
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "an atom", "1", "a list"
        );
        erep->addError(e);
        return;
    }

    Token *name = n->token;

    if (name->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "a symbol", "1", name->tokenType()
        );
        erep->addError(e);
        return;
    }

    n = (*lst)[2];

    if (!n->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "a list", "2", "an atom"
        );
        erep->addError(e);
        return;
    }

    n = parseOptionalMacroCall(n);
    if (!n) {
        return;
    }

    symlist *sublst = n->list;

    Node *subn = (*sublst)[0];

    if (!subn->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "an atom", "2:1", "a list"
        );
        erep->addError(e);
        return;
    }

    Token *subt = subn->token;

    if (subt->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "a symbol", "2:1", subt->tokenType()
        );
        erep->addError(e);
        return;
    }

    if (!subt->str_value.compare("fn")) {
        parseFunction(name->str_value.c_str(), n, NULL,
                      Linkage::Null, 0);
    } else if (!subt->str_value.compare("var")) {
        parseGlobalVariable(name->str_value.c_str(), n);
    } else if (!subt->str_value.compare("struct")) {
        parseStructDefinition(name->str_value.c_str(), n);
    } else if (!subt->str_value.compare("macro")) {
        parseMacroDefinition(name->str_value.c_str(), n);
    } else if (!subt->str_value.compare("enum")) {
        parseEnumDefinition(name->str_value.c_str(), n);
    } else {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "'fn'/'var'/'struct'/'macro'",
            "2:1"
        );
        std::string temp;
        temp.append("'");
        temp.append(subt->str_value);
        temp.append("'");
        e->addArgString(&temp);
        erep->addError(e);
        return;
    }

    return;
}

void Generator::parseEnumDefinition(const char *name, Node *top)
{
    assert(top->list && "must receive a list!");

    symlist *lst = top->list;

    if (lst->size() < 4) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            top,
            "enum", 3, (int) lst->size() - 1
        );
        erep->addError(e);
        return;
    }

    Node *lnk = (*lst)[1];
    int linkage = parseEnumLinkage(lnk);
    if (!linkage) {
        return;
    }

    Node *enumtypen = (*lst)[2];

    Element::Type *enumtype = parseType(enumtypen, false, false);
    if (!enumtype) {
        return;
    }
    if (!enumtype->isIntegerType()) {
        Error *e = new Error(
            ErrorInst::Generator::EnumTypeMustBeInteger,
            enumtypen
        );
        erep->addError(e);
        return;
    }

    /* Enums have a maximum size of 64 bits. */
    llvm::Type *d_enumtype =
        toLLVMType(enumtype, NULL, false);
    if (!d_enumtype) {
        failedDaleToLLVMTypeConversion(enumtype);
        return;
    }

    Node *elements = (*lst)[3];

    if (!elements->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            elements,
            "enum", "a list", "1", "an atom"
        );
        erep->addError(e);
        return;
    }

    Element::Enum *enm = new Element::Enum();
    enm->once_tag = current_once_tag;
    enm->linkage = linkage;

    std::vector<Node *>::iterator iter =
        elements->list->begin();

    while (iter != elements->list->end()) {
        Node *n = (*iter);

        if (n->is_token) {
            if (n->token->type != TokenType::String) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    n,
                    "symbol", "enum element", n->token->tokenType()
                );
                erep->addError(e);
                return;
            }
            int res =
                enm->addElement(n->token->str_value.c_str());
            if (!res) {
                Error *e = new Error(
                    ErrorInst::Generator::RedeclarationOfEnumElement,
                    n, n->token->str_value.c_str()
                );
                erep->addError(e);
                return;
            }
        } else {
            n = parseOptionalMacroCall(n);
            if (!n) {
                return;
            }
            symlist *mylst = n->list;
            if (mylst->size() != 2) {
                Error *e = new Error(
                    ErrorInst::Generator::IncorrectNumberOfArgs,
                    n, 2, mylst->size()
                );
                erep->addError(e);
                return;
            }
            Node *tn = (*mylst)[0];
            if (!tn->is_token) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    tn,
                    "atom", "enum element list", "list"
                );
                erep->addError(e);
                return;
            }
            if (tn->token->type != TokenType::String) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    tn,
                    "symbol", "enum element list",
                    tn->token->tokenType()
                );
                erep->addError(e);
                return;
            }
            Node *num = (*mylst)[1];
            if (!num->is_token) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    num,
                    "atom", "enum element list", "list"
                );
                erep->addError(e);
                return;
            }
            if (num->token->type != TokenType::Int) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    num,
                    "integer", "enum element index",
                    num->token->tokenType()
                );
                erep->addError(e);
                return;
            }

            llvm::ConstantInt *c =
                getConstantInt(llvm::cast<llvm::IntegerType>(d_enumtype),
                               num->token->str_value.c_str());
            int index = (int) c->getLimitedValue();
            int res =
                enm->addElement(tn->token->str_value.c_str(),
                                index);
            if (!res) {
                fprintf(stderr,
                        "Internal error: cannot add enum element.\n");
                abort();
            }
        }

        ++iter;
    }

    int res = ctx->ns()->addEnum(name, enm);
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfEnum,
            top,
            name
        );
        erep->addError(e);
        return;
    }

    Element::Struct *enum_str = new Element::Struct();
    enum_str->addElement("_enum_value", enumtype);
    enum_str->once_tag = current_once_tag;
    enum_str->linkage =
        (linkage == EnumLinkage::Extern) ? StructLinkage::Extern
        : StructLinkage::Intern;

    std::vector<llvm::Type*> elements_llvm;
    elements_llvm.push_back(d_enumtype);

    /* Second arg here is 'ispacked'. */
    llvm::StructType *llvm_new_struct =
        llvm::StructType::create(llvm::getGlobalContext(),
                                 "new_enum_struct");
    llvm_new_struct->setBody(elements_llvm);

    std::string name2;
    name2.append("struct_");
    std::string name3;
    ctx->ns()->nameToSymbol(name, &name3);
    name2.append(name3);
    enum_str->internal_name.append(name2);

    llvm_new_struct->setName(name2.c_str());
    if (llvm_new_struct->getName() != llvm::StringRef(name2)) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfStruct,
            top,
            name
        );
        erep->addError(e);
        return;
    }

    enum_str->type = llvm_new_struct;

    res = ctx->ns()->addStruct(name, enum_str);
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfStruct,
            top,
            name
        );
        erep->addError(e);
        return;
    }

    /* Got a struct type - return it. */
    Element::Type *ttt = new Element::Type();
    ttt->struct_name = new std::string(name);

    std::vector<std::string> *new_namespaces =
        new std::vector<std::string>;

    ctx->setNamespacesForEnum(name, new_namespaces);
    ttt->namespaces = new_namespaces;

    int flinkage = (linkage == EnumLinkage::Extern)
                   ? Linkage::Extern
                   : Linkage::Intern;

    BasicTypes::addEnum(ctx, mod, &current_once_tag, ttt,
                        enumtype, d_enumtype, flinkage);

    return;
}

void Generator::parseMacroDefinition(const char *name, Node *top)
{
    /* Ensure this isn't core (core forms cannot be overridden
     * with a macro). */
    std::set<std::string>::iterator cf =
        core_forms->find(name);
    if (cf != core_forms->end()) {
        Error *e = new Error(
            ErrorInst::Generator::NoCoreFormNameInMacro,
            top
        );
        erep->addError(e);
        return;
    }

    symlist *lst = top->list;

    if (lst->size() < 3) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            top,
            "macro", 2, (int) (lst->size() - 1)
        );
        erep->addError(e);
        return;
    }

    int linkage = parseLinkage((*lst)[1]);
    if (!linkage) {
        return;
    }

    setPdnode();
    Element::Type *r_type = type_pdnode;

    /* Parse arguments - push onto the list that gets created. */

    Node *nargs = (*lst)[2];

    if (!nargs->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            nargs,
            "list", "macro parameters", "atom"
        );
        erep->addError(e);
        return;
    }

    symlist *args = nargs->list;

    Element::Variable *var;

    std::vector<Element::Variable *> *mc_args_internal =
        new std::vector<Element::Variable *>;

    /* Parse argument - need to keep names. */

    std::vector<Node *>::iterator node_iter;
    node_iter = args->begin();

    bool varargs = false;

    /* An implicit MContext argument is added to every macro. */

    Element::Type *pst = tr->getStructType("MContext");
    Element::Type *ptt = tr->getPointerType(pst);

    Element::Variable *var1 = new Element::Variable(
        (char*)"mc", ptt
    );
    var1->linkage = Linkage::Auto;
    mc_args_internal->push_back(var1);

    int past_first = 0;

    while (node_iter != args->end()) {
        if (!(*node_iter)->is_token) {
            var = new Element::Variable();
            parseArgument(var, (*node_iter), false, false);
            if (!var->type) {
                return;
            }
            mc_args_internal->push_back(var);
            ++node_iter;
        } else {
            if (!((*node_iter)->token->str_value.compare("void"))) {
                if (past_first || (args->size() > 1)) {
                    Error *e = new Error(
                        ErrorInst::Generator::VoidMustBeTheOnlyParameter,
                        nargs
                    );
                    erep->addError(e);
                    return;
                }
                break;
            }
            if (!((*node_iter)->token->str_value.compare("..."))) {
                if ((args->end() - node_iter) != 1) {
                    Error *e = new Error(
                        ErrorInst::Generator::VarArgsMustBeLastParameter,
                        nargs
                    );
                    erep->addError(e);
                    return;
                }
                var = new Element::Variable();
                var->type = type_varargs;
                var->linkage = Linkage::Auto;
                mc_args_internal->push_back(var);
                break;
            }
            var = new Element::Variable();
            var->type = r_type;
            var->linkage = Linkage::Auto;
            var->name.append((*node_iter)->token->str_value);
            past_first = 1;
            mc_args_internal->push_back(var);
            ++node_iter;
        }
    }

    std::vector<llvm::Type*> mc_args;

    /* Convert to llvm args. The MContext argument is converted as per
     * its actual type. The remaining arguments, notwithstanding the
     * macro argument's 'actual' type, will always be (p DNode)s. */

    std::vector<Element::Variable *>::iterator iter;
    iter = mc_args_internal->begin();
    llvm::Type *temp;

    int count = 0;
    while (iter != mc_args_internal->end()) {
        if ((*iter)->type->base_type == Type::VarArgs) {
            /* Varargs - finish. */
            varargs = true;
            break;
        }
        if (count == 0) {
            temp = toLLVMType((*iter)->type, NULL, false);
            if (!temp) {
                failedDaleToLLVMTypeConversion((*iter)->type);
                return;
            }
        } else {
            temp = toLLVMType(r_type, NULL, false);
            if (!temp) {
                failedDaleToLLVMTypeConversion(r_type);
                return;
            }
        }
        mc_args.push_back(temp);
        ++count;
        ++iter;
    }

    temp = toLLVMType(r_type, NULL, false);
    if (!temp) {
        failedDaleToLLVMTypeConversion(r_type);
        return;
    }

    llvm::FunctionType *ft =
        getFunctionType(
            temp,
            mc_args,
            varargs
        );

    std::string new_name;

    ctx->ns()->functionNameToSymbol(name,
                            &new_name,
                            linkage,
                            mc_args_internal);

    if (mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfFunctionOrMacro,
            top,
            name
        );
        erep->addError(e);
        return;
    }

    llvm::Constant *fnc =
        mod->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    llvm::Function *fn = llvm::dyn_cast<llvm::Function>(fnc);

    /* This is probably unnecessary, given the previous
     * getFunction call. */
    if ((!fn) || (fn->size())) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfFunctionOrMacro,
            top,
            name
        );
        erep->addError(e);
        return;
    }

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(linkage));

    llvm::Function::arg_iterator largs = fn->arg_begin();

    /* Note that the values of the Variables of the macro's
     * parameter list will not necessarily match the Types of
     * those variables (to support overloading). */

    iter = mc_args_internal->begin();
    while (iter != mc_args_internal->end()) {
        if ((*iter)->type->base_type == Type::VarArgs) {
            break;
        }

        llvm::Value *temp = largs;
        ++largs;
        temp->setName((*iter)->name.c_str());
        (*iter)->value = temp;
        ++iter;
    }

    /* Add the macro to the context. */
    Element::Function *dfn =
        new Element::Function(r_type, mc_args_internal, fn, 1,
                              &new_name);
    dfn->linkage = linkage;

    if (!ctx->ns()->addFunction(name, dfn, top)) {
        return;
    }
    if (current_once_tag.length() > 0) {
        dfn->once_tag = current_once_tag;
    }

    /* If the list has only three arguments, the macro is a
     * declaration and you can return straightaway. */

    if (lst->size() == 3) {
        return;
    }

    /* This is used later on when determining whether to remove
     * all macro-related content from the linked module. If no
     * extern macros have been defined (cf. declared), then all
     * macro content should be removed, since it's not needed at
     * runtime. This also allows createConstantMergePass to run,
     * since it doesn't work if the macro content is not removed,
     * for some reason. */
    if (linkage == Linkage::Extern) {
        has_defined_extern_macro = 1;
    }

    int error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    global_functions.push_back(dfn);
    global_function = dfn;

    parseFunctionBody(dfn, fn, top, 3, 0);

    global_functions.pop_back();
    if (global_functions.size()) {
        global_function = global_functions.back();
    } else {
        global_function = NULL;
    }

    ctx->deactivateNamespace(anon_name.c_str());

    int error_post_count =
        erep->getErrorTypeCount(ErrorType::Error);
    if (error_count != error_post_count) {
        std::map<std::string, std::vector<Element::Function*>*
        >::iterator i = ctx->ns()->functions.find(name);
        if (i != ctx->ns()->functions.end()) {
            for (std::vector<Element::Function *>::iterator
                    j = i->second->begin(),
                    k = i->second->end();
                    j != k;
                    ++j) {
                if ((*j)->is_macro) {
                    i->second->erase(j);
                    break;
                }
            }
        }
    }

    return;
}

int Generator::addOpaqueStruct(const char *name, Node *top,
                               int linkage, int must_init)
{
    if (!top) {
        top = new Node();
    }

    llvm::StructType *sty = llvm::StructType::create(
                                llvm::getGlobalContext(), "created_opaque_type");

    std::string name2;
    name2.append("struct_");
    std::string name3;
    ctx->ns()->nameToSymbol(name, &name3);
    name2.append(name3);

    sty->setName(name2.c_str());

    Element::Struct *new_struct = new Element::Struct();
    new_struct->must_init = must_init;
    new_struct->type = sty;
    new_struct->is_opaque = 1;
    new_struct->linkage = linkage;
    new_struct->internal_name.append(name2.c_str());
    new_struct->once_tag = current_once_tag;

    if (!ctx->ns()->addStruct(name, new_struct)) {
        /* Only an error if there is not an existing struct. This
         * used to add an error message if the struct had already
         * been fully defined, but that is not an error in C, so
         * it won't be here, either. */
        Element::Struct *temp = ctx->getStruct(name);
        if (!temp) {
            Error *e = new Error(
                ErrorInst::Generator::UnableToParseForm,
                top
            );
            erep->addError(e);
            return 0;
        }
    }

    /* On upgrading to 3, if a struct was not used in a module, it
     * was not included when the module was output. To maintain
     * the previous behaviour, add an intern function declaration
     * that takes no arguments and returns a value of the type of
     * this struct. */

    std::vector<llvm::Type*> args;
    llvm::FunctionType *ft =
        getFunctionType(
            sty,
            args,
            false
        );

    int index = 0;
    char buf[100];
    while (1) {
        sprintf(buf, "__retain_struct_%d", index);
        if (!mod->getFunction(buf)) {
            mod->getOrInsertFunction(buf, ft);
            break;
        }
        ++index;
    }

    return 1;
}

static int anonstructcount = 0;

void Generator::parseStructDefinition(const char *name, Node *top)
{
    assert(top->list && "parseStructDefinition must receive a list!");
    anonstructcount++;

    if (!assertArgNums("struct", top, 1, 3)) {
        return;
    }

    symlist *lst = top->list;
    int must_init = 0;

    /* Struct attributes. */

    int next_index = 1;
    Node *test = ((*lst)[next_index]);
    if (test->is_list
            && test->list->at(0)->is_token
            && !(test->list->at(0)->token
                 ->str_value.compare("attr"))) {
        symlist *attr_list = test->list;
        std::vector<Node*>::iterator b = attr_list->begin(),
                                     e = attr_list->end();
        ++b;
        for (; b != e; ++b) {
            if ((*b)->is_list) {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                erep->addError(e);
                return;
            }
            if (!((*b)->token->str_value.compare("must-init"))) {
                must_init = 1;
            } else {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                erep->addError(e);
                return;
            }
        }
        ++next_index;
    }

    int linkage = parseStructLinkage((*lst)[next_index]);
    if (!linkage) {
        return;
    }
    ++next_index;

    int res = addOpaqueStruct(name, top, linkage, must_init);
    if (!res) {
        return;
    }

    /* If the list contains two elements (name and linkage), or
     * three elements (name, attributes and linkage), the struct
     * is actually opaque, so return now. */

    if ((lst->size() == 2) || ((lst->size() == 3) && (next_index == 3))) {
        return;
    }

    /* Parse elements - push onto the list that gets created. */

    Node *nelements = (*lst)[next_index];

    if (!nelements->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            nelements,
            "struct", "a list", "1", "an atom"
        );
        erep->addError(e);
        return;
    }

    symlist *elements = nelements->list;

    Element::Variable *var;

    std::vector<Element::Variable *> *elements_internal =
        new std::vector<Element::Variable *>;

    std::vector<Node *>::iterator node_iter;
    node_iter = elements->begin();

    while (node_iter != elements->end()) {

        var = new Element::Variable();
        var->type = NULL;

        parseArgument(var, (*node_iter), true, true);

        if (!var || !var->type) {
            Error *e = new Error(
                ErrorInst::Generator::InvalidType,
                (*node_iter)
            );
            erep->addError(e);
            return;
        }

        /* Can't have non-pointer void. */
        if (var->type->base_type == Type::Void) {
            Error *e = new Error(
                ErrorInst::Generator::TypeNotAllowedInStruct,
                (*node_iter),
                "void"
            );
            erep->addError(e);
            delete var;
            return;
        }

        /* This code can't be hit at the moment, but is left here
         * just in case. */
        if (var->type->base_type == Type::VarArgs) {
            Error *e = new Error(
                ErrorInst::Generator::TypeNotAllowedInStruct,
                (*node_iter),
                "varargs"
            );
            erep->addError(e);
            delete var;
            return;
        }

        elements_internal->push_back(var);

        ++node_iter;
    }

    /* Convert to llvm args and add the struct to the module. */

    std::vector<llvm::Type*> elements_llvm;

    std::vector<Element::Variable *>::iterator iter;
    iter = elements_internal->begin();
    llvm::Type *temp;

    while (iter != elements_internal->end()) {
        temp = toLLVMType((*iter)->type, NULL, false);
        if (!temp) {
            failedDaleToLLVMTypeConversion((*iter)->type);
            return;
        }
        elements_llvm.push_back(temp);
        ++iter;
    }

    std::string name2;

    std::string name3;
    ctx->ns()->nameToSymbol(name, &name3);
    name2.append(name3);

    bool already_exists = true;

    Element::Struct *new_struct;

    if (already_exists) {
        /* If the struct does not already exist in context, then
         * there has been some strange error. */
        Element::Struct *temp = ctx->getStruct(name);
        if (!temp) {
            Error *e = new Error(
                ErrorInst::Generator::UnableToParseForm,
                top
            );
            erep->addError(e);
            return;
        }

        /* If it does exist, but is not opaque, then it cannot be
         * redefined. */

        if (!temp->is_opaque) {
            Error *e = new Error(
                ErrorInst::Generator::RedeclarationOfStruct,
                top,
                name
            );
            erep->addError(e);
            return;
        }

        /* If it does exist, and is opaque, get its Type, cast it
         * to a StructType and add the elements. */

        llvm::StructType *opaque_struct_type =
            llvm::cast<llvm::StructType>(temp->type);
        opaque_struct_type->setBody(
            llvm::ArrayRef<llvm::Type*>(elements_llvm)
        );
        new_struct = temp;
        new_struct->is_opaque = 0;
    } else {
        new_struct = new Element::Struct();
    }

    /* Re-get the type from the module, because the type in
     * llvm_new_struct will be buggered if the module factors
     * types out (e.g. where the current structure is the same as
     * some previously defined structure and has a
     * self-reference). */

    new_struct->internal_name.clear();
    new_struct->internal_name.append(name2.c_str());

    new_struct->linkage = linkage;

    iter = elements_internal->begin();

    while (iter != elements_internal->end()) {
        new_struct->addElement((*iter)->name.c_str(),
                               (*iter)->type);
        delete (*iter);
        ++iter;
    }

    elements_internal->clear();
    delete elements_internal;

    if (!already_exists) {
        if (!ctx->ns()->addStruct(name, new_struct)) {
            printf("Does not already exist, but can't add\n");
            Error *e = new Error(
                ErrorInst::Generator::RedeclarationOfStruct,
                top,
                name
            );
            erep->addError(e);
            return;
        }
    }

    return;
}

void Generator::parseGlobalVariable(const char *name, Node *top)
{
    assert(top->list && "parseGlobalVariable must receive a list!");

    symlist *lst = top->list;
    int has_initialiser;

    if (lst->size() < 3) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            top,
            "var", "2"
        );
        char buf[100];
        sprintf(buf, "%d", (int) lst->size() - 1);
        e->addArgString(buf);
        erep->addError(e);
        return;
    } else if (lst->size() == 3) {
        has_initialiser = 0;
    } else if (lst->size() == 4) {
        has_initialiser = 1;
    } else {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMaximumNumberOfArgs,
            top,
            "var", "3"
        );
        char buf[100];
        sprintf(buf, "%d", (int) lst->size() - 1);
        e->addArgString(buf);
        erep->addError(e);
        return;
    }

    int linkage = parseLinkage((*lst)[1]);

    Element::Type *r_type = parseType((*lst)[2], false, false);
    if (r_type == NULL) {
        return;
    }
    if (r_type->array_type && (r_type->array_size == 0)) {
        Error *e = new Error(
            ErrorInst::Generator::ZeroLengthGlobalArraysAreUnsupported,
            top
        );
        erep->addError(e);
        return;
    }

    int size = 0;

    Node *n2 = NULL;
    if (has_initialiser) {
        n2 = parseOptionalMacroCall((*lst)[3]);
        if (!n2) {
            return;
        }
    }

    llvm::Constant *init =
        (has_initialiser)
        ? parseLiteral(r_type, n2, &size)
        : NULL;

    if ((init == NULL) && (has_initialiser)) {
        return;
    }

    std::string new_name;
    if (linkage == Linkage::Extern_C) {
        new_name.append(name);
    } else {
        ctx->ns()->nameToSymbol(name, &new_name);
    }

    Element::Variable *check = ctx->getVariable(name);
    if (check
            && check->type->isEqualTo(r_type)
            && (check->linkage == linkage)
            && !has_initialiser) {
        /* Redeclaration of global variable - no problem. */
        return;
    }

    /* Add the variable to the context. */

    Element::Variable *var2 = new Element::Variable();
    var2->name.append(name);
    var2->type = r_type;
    var2->internal_name.append(new_name);
    var2->once_tag = current_once_tag;
    var2->linkage = linkage;
    int avres = ctx->ns()->addVariable(name, var2);

    if (!avres) {
        Error *e = new Error(
            ErrorInst::Generator::RedefinitionOfVariable,
            top,
            name
        );
        erep->addError(e);
        return;
    }

    /* todo: an 'is_extern_linkage' function. */
    int has_extern_linkage =
        ((linkage != Linkage::Auto)
         && (linkage != Linkage::Intern));

    llvm::Type *rdttype =
        toLLVMType(r_type, top, false,
                       (has_extern_linkage && !has_initialiser));
    if (!rdttype) {
        failedDaleToLLVMTypeConversion(r_type);
        return;
    }

    /* Add the variable to the module. */

    if (mod->getGlobalVariable(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "global variable already exists in "
                "module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::GlobalVariable *var =
        llvm::cast<llvm::GlobalVariable>(
            mod->getOrInsertGlobal(new_name.c_str(),
                                   rdttype)
        );

    var->setLinkage(ctx->toLLVMLinkage(linkage));

    if (init) {
        var->setInitializer(init);
    } else {
        if ((linkage != Linkage::Extern)
                && (linkage != Linkage::Extern_C)
                && (linkage != Linkage::Extern_Weak)) {
            has_initialiser = 1;
            if (r_type->points_to) {
                llvm::ConstantPointerNull *mynullptr =
                    llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(rdttype)
                    );
                var->setInitializer(mynullptr);
            } else if (r_type->struct_name) {
                llvm::ConstantAggregateZero* const_values_init =
                    llvm::ConstantAggregateZero::get(rdttype);
                var->setInitializer(const_values_init);
            } else if (r_type->is_array) {
                llvm::ConstantAggregateZero* const_values_init =
                    llvm::ConstantAggregateZero::get(rdttype);
                var->setInitializer(const_values_init);
            } else if (r_type->isIntegerType()) {
                var->setInitializer(
                    getConstantInt(
                        llvm::IntegerType::get(
                            llvm::getGlobalContext(),
                            mySizeToRealSize(
                                r_type->getIntegerSize()
                            )
                        ),
                        "0"
                    )
                );
            } else {
                has_initialiser = 0;
            }
            var2->has_initialiser = has_initialiser;
        }
    }

    var2->value = llvm::cast<llvm::Value>(var);

    return;
}

llvm::Constant *Generator::parseLiteralElement(Node *top,
        char *thing,
        Element::Type
        *type,
        int *size)
{
    std::string t;
    type->toStringProper(&t);

    if (type->base_type == Type::Bool) {
        llvm::APInt myint(1,
                          *thing);
        llvm::ConstantInt *myconstint =
            llvm::ConstantInt::get(llvm::getGlobalContext(),
                                   myint);
        return llvm::cast<llvm::Constant>(myconstint);
    }

    if (type->base_type == Type::Char) {
        llvm::APInt myint(8,
                          *thing);
        llvm::ConstantInt *myconstint =
            llvm::ConstantInt::get(llvm::getGlobalContext(),
                                   myint);
        return llvm::cast<llvm::Constant>(myconstint);
    }

    if (type->isIntegerType()) {
        union mynum {
            unsigned char udata[8];
            uint64_t      nvalue;
        } bling;
        bling.nvalue = 0;
        int pr_size =
            mySizeToRealSize(type->getIntegerSize());
        int i;
        if (pr_size == 128) {
            uint64_t nvalues[2];
            for (i = 0; i < 8; i++) {
                bling.udata[i] = *(thing + i);
            }
            nvalues[0] = bling.nvalue;
            for (i = 8; i < 16; i++) {
                bling.udata[i - 8] = *(thing + i);
            }
            nvalues[1] = bling.nvalue;
            llvm::APInt myint((unsigned) pr_size,
                              2,
                              nvalues);
            llvm::ConstantInt *myconstint =
                llvm::ConstantInt::get(llvm::getGlobalContext(),
                                       myint);
            return llvm::cast<llvm::Constant>(myconstint);
        } else {
            bling.nvalue = 0;
            for (i = 0; i < (pr_size / 8); i++) {
                bling.udata[i] = *(thing + i);
            }
            llvm::APInt myint(pr_size,
                              bling.nvalue);
            llvm::ConstantInt *myconstint =
                llvm::ConstantInt::get(llvm::getGlobalContext(),
                                       myint);
            return llvm::cast<llvm::Constant>(myconstint);
        }
    }

    if (type->base_type == Type::Float) {
        union float_hex {
            unsigned char udata[4];
            float         fvalue;
        } bling;
        bling.udata[3] = thing[3];
        bling.udata[2] = thing[2];
        bling.udata[1] = thing[1];
        bling.udata[0] = thing[0];
        llvm::APFloat myfloat(bling.fvalue);
        llvm::ConstantFP *myconstfloat =
            llvm::ConstantFP::get(llvm::getGlobalContext(),
                                  myfloat);
        return llvm::cast<llvm::Constant>(myconstfloat);
    }

    if (type->base_type == Type::Double) {
        union double_hex {
            unsigned char udata[8];
            double        dvalue;
        } bling;
        bling.udata[7] = thing[7];
        bling.udata[6] = thing[6];
        bling.udata[5] = thing[5];
        bling.udata[4] = thing[4];

        bling.udata[3] = thing[3];
        bling.udata[2] = thing[2];
        bling.udata[1] = thing[1];
        bling.udata[0] = thing[0];
        llvm::APFloat mydouble(bling.dvalue);
        llvm::ConstantFP *myconstdouble =
            llvm::ConstantFP::get(llvm::getGlobalContext(),
                                  mydouble);
        return llvm::cast<llvm::Constant>(myconstdouble);
    }

    if (type->struct_name) {
        std::vector<llvm::Constant *> constants;

        Element::Struct *str =
            ctx->getStruct(
                type->struct_name->c_str(),
                type->namespaces
            );
        if (!str) {
            fprintf(stderr, "Internal error: invalid struct.\n");
            abort();
        }

        std::vector<Element::Type *>::iterator begin =
            str->element_types.begin();

        int i = 0;
        int last_el_size = -1;
        int last_offset = -1;
        int incr = 0;

        while (begin != str->element_types.end()) {
            Element::Type *current = (*begin);
            size_t el_size =
                getSizeofTypeImmediate(current);
            size_t offset =
                getOffsetofTypeImmediate(type, NULL, i);
            size_t padding = 0;
            if (i != 0) {
                padding = (offset - last_offset - last_el_size);
            }
            if (padding) {
                Error *e = new Error(
                    ErrorInst::Generator::StructContainsPadding,
                    top
                );
                erep->addError(e);
            }
            incr += padding;
            char *addr = thing;
            addr += offset;
            char aligned[256];
            memcpy(aligned, addr, el_size);

            llvm::Constant *el =
                parseLiteralElement(top,
                                    (char*) aligned,
                                    current,
                                    size);
            if (!el) {
                return NULL;
            }
            constants.push_back(el);
            last_offset  = offset - incr;
            last_el_size = el_size;
            ++i;
            ++begin;
        }

        llvm::Type *llvm_type =
            toLLVMType(type, NULL, false);
        if (!llvm_type) {
            failedDaleToLLVMTypeConversion(type);
            return NULL;
        }

        llvm::StructType *st =
            llvm::cast<llvm::StructType>(llvm_type);

        llvm::Constant *init =
            llvm::ConstantStruct::get(
                st,
                constants
            );

        return init;
    }

    if (type->points_to && (type->points_to->base_type == Type::Char)) {
        char *temp =
            *(char**)
            (((uintptr_t) thing));
        *size = strlen(temp) + 1;
        llvm::Constant *myconststr =
            llvm::cast<llvm::Constant>(
                llvm::ConstantArray::get(llvm::getGlobalContext(),
                                         temp,
                                         true)
            );

        std::string varname2;
        getUnusedVarname(&varname2);

        Element::Type *archar = tr->getArrayType(type_char, *size);

        if (mod->getGlobalVariable(llvm::StringRef(varname2.c_str()))) {
            fprintf(stderr, "Internal error: "
                    "global variable already exists "
                    "in module ('%s').\n",
                    varname2.c_str());
            abort();
        }

        llvm::GlobalVariable *svar2 =
            llvm::cast<llvm::GlobalVariable>(
                mod->getOrInsertGlobal(varname2.c_str(),
                                       toLLVMType(archar, NULL, false))
            );

        svar2->setInitializer(myconststr);
        svar2->setConstant(true);
        svar2->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

        llvm::Value *temps[2];
        temps[0] = llvm_native_zero;
        temps[1] = llvm_native_zero;

        llvm::Constant *pce =
            llvm::ConstantExpr::getGetElementPtr(
                llvm::cast<llvm::Constant>(svar2),
                temps,
                2
            );

        return pce;
    }

    if (type->points_to) {
        if (*thing) {
            uint64_t value = *(uint64_t*)thing;
            if (sizeof(char*) == 4) {
                value <<= 32;
                if (!value) {
                    goto a;
                }
            }
            Error *e = new Error(
                ErrorInst::Generator::NonNullPointerInGlobalStructDeclaration,
                top
            );
            erep->addError(e);
        }
a:
        llvm::Type *llvm_type =
            toLLVMType(type, NULL, false);
        if (!llvm_type) {
            failedDaleToLLVMTypeConversion(type);
            return NULL;
        }
        llvm::Constant *pce =
            llvm::ConstantPointerNull::get(
                llvm::cast<llvm::PointerType>(llvm_type)
            );
        return pce;
    }

    if (type->is_array) {
        /* Take the portion devoted to whatever the element is,
         * and re-call this function. */
        size_t el_size =
            getSizeofTypeImmediate(type->array_type);
        int i = 0;
        int els = type->array_size;
        std::vector<llvm::Constant *> constants;

        char elmemm[256];
        char *elmem = elmemm;

        for (i = 0; i < els; i++) {
            // Memset it to nothing.

            memset(elmem, 0, 256);

            // Offset thing by the index, cast to a char pointer, and
            // copy x elements into the new block.
            char *mp = (char*) thing;
            mp += (i * el_size);
            memcpy(elmem, mp, el_size);

            // Re-call parseLiteralElement, push the new constant onto
            // the vector.
            llvm::Constant *mycon =
                parseLiteralElement(top,
                                    elmem,
                                    type->array_type,
                                    size);

            constants.push_back(mycon);
        }

        llvm::Constant *mine =
            llvm::ConstantArray::get(
                llvm::cast<llvm::ArrayType>(
                    toLLVMType(type, top, false, false)
                ),
                constants
            );

        return mine;
    }

    Error *e = new Error(
        ErrorInst::Generator::CannotParseLiteral,
        top,
        t.c_str()
    );
    erep->addError(e);

    return NULL;
}

static int myn = 0;
/* Size is only set when you are parsing a string literal - it
 * will contain the final size of the returned array. */
llvm::Constant *Generator::parseLiteral(Element::Type *type,
                                        Node *top,
                                        int *size)
{
    /* Extreme special-case - if top is a two-element list, and
     * the first element is #, and the second element is a global
     * variable name, then return the address of that global
     * variable as a constant value. This is to get around the
     * fact that arbitrary pointer values returned from the
     * function created below will not be valid with respect to
     * global variables. (todo - not as useful as you thought it
     * was.) */

    if (top->is_list
            && (top->list->size() == 2)
            && (top->list->at(0)->is_token)
            && (!top->list->at(0)->token->str_value.compare("#"))
            && (type->points_to)) {
        Node *var = top->list->at(1);
        var = parseOptionalMacroCall(var);
        if (var && var->is_token) {
            Element::Variable *gv =
                ctx->getVariable(var->token->str_value.c_str());
            if (!(type->points_to->isEqualTo(gv->type))) {
                std::string want;
                std::string got;
                gv->type->toStringProper(&got);
                type->toStringProper(&want);
                Error *e = new Error(
                    ErrorInst::Generator::IncorrectType,
                    top,
                    want.c_str(),
                    got.c_str()
                );
                erep->addError(e);
                return NULL;
            }
            llvm::Constant *pce =
                llvm::cast<llvm::Constant>(gv->value);
            return pce;
        }
    }

    std::string str;
    type->toStringProper(&str);

    // Create an empty no-argument function that returns the
    // specified type.

    llvm::Type *llvm_return_type =
        toLLVMType(type, top, false);
    if (!llvm_return_type) {
        failedDaleToLLVMTypeConversion(type);
        return NULL;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[255];
    sprintf(buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        mod->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    std::vector<Element::Variable*> args;

    llvm::Function *fn = llvm::cast<llvm::Function>(fnc);

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_C));

    Element::Function *dfn =
        new Element::Function(type, &args, fn, 0,
                              &new_name);
    dfn->linkage = Linkage::Intern;
    int error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    std::vector<Node*> nodes;
    nodes.push_back(top);
    Node *topwrapper = new Node(&nodes);

    parseFunctionBody(dfn, fn, topwrapper, 0, 0);
    int error_post_count =
        erep->getErrorTypeCount(ErrorType::Error);
    if (error_count != error_post_count) {
        return NULL;
    }

    llvm::Type *tttt = toLLVMType(type_void, NULL, true);
    llvm::FunctionType *wrapft =
        getFunctionType(
            tttt,
            mc_args,
            false
        );

    std::string wrap_new_name;
    char wrap_buf[255];
    sprintf(wrap_buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(wrap_buf, &wrap_new_name);

    if (mod->getFunction(llvm::StringRef(wrap_new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                wrap_new_name.c_str());
        abort();
    }

    llvm::Constant *wrap_fnc =
        mod->getOrInsertFunction(
            wrap_new_name.c_str(),
            wrapft
        );

    llvm::Function *wrap_fn =
        llvm::cast<llvm::Function>(wrap_fnc);

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "entry", wrap_fn);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args;
    llvm::Value *ret = builder.CreateCall(
                           fn, llvm::ArrayRef<llvm::Value*>(call_args)
                       );

    llvm::Value *reta = builder.CreateAlloca(
                            llvm_return_type
                        );
    llvm::Value *reta2 = builder.CreateAlloca(
                             llvm_return_type
                         );
    builder.CreateStore(ret, reta2);

    std::vector<Element::Type *> call_arg_types;
    Element::Type *ptype = tr->getPointerType(type);
    stl::push_back2(&call_arg_types, ptype, ptype);

    std::vector<llvm::Value *> call_args2;
    stl::push_back2(&call_args2, reta, reta2);

    if (Element::Function *or_setf =
                ctx->getFunction("setf-assign", &call_arg_types, NULL, 0)) {
        builder.CreateCall(
            or_setf->llvm_function,
            llvm::ArrayRef<llvm::Value*>(call_args2)
        );
    } else {
        builder.CreateStore(ret, reta);
    }

    ParseResult temp_pr;
    bool res =
        doCast(block,
               reta,
               tr->getPointerType(type),
               type_pchar,
               top, 0, &temp_pr);
    if (!res) {
        return false;
    }
    block = temp_pr.block;
    llvm::Value *retaa = temp_pr.value;

    typedef struct temp_t {
        char c[256];
    } temp;
    temp thing;
    memset(&thing, 0, 256);

    char buf6[100];
    sprintf(buf6, "%lld", (long long int) &thing);

    llvm::Value *v =
        getConstantInt(
            llvm::IntegerType::get(
                llvm::getGlobalContext(),
                sizeof(char*) * 8
            ),
            buf6
        );
    ParseResult storeor;
    res =
        doCast(block,
               v,
               type_intptr,
               type_pchar,
               top, 0, &storeor
              );
    if (!res) {
        return false;
    }
    llvm::Value *store = storeor.value;
    builder.SetInsertPoint(storeor.block);
    Element::Function *memcpy = ctx->getFunction("memcpy", NULL,
                                NULL, 0);
    if (!memcpy) {
        fprintf(stderr,
                "Internal error: no memcpy function available.\n");
        abort();
    }

    size_t struct_size =
        getSizeofTypeImmediate(type);
    char buf5[5];
    sprintf(buf5, "%u", (unsigned) struct_size);

    std::vector<llvm::Value*> memcpy_args;
    memcpy_args.push_back(store);
    memcpy_args.push_back(retaa);
    memcpy_args.push_back(
        getConstantInt(llvm::IntegerType::get(llvm::getGlobalContext(),
                       32), buf5));
    builder.CreateCall(memcpy->llvm_function,
                       llvm::ArrayRef<llvm::Value*>(memcpy_args)
                      );

    /* Take this value, put it in the struct pointer. */

    builder.CreateRetVoid();

    void* fptr =
        ee->getPointerToFunction(wrap_fn);
    if (!fptr) {
        fprintf(stderr,
                "Internal error: could not get pointer "
                "to function for literal.\n");
        abort();
    }

    ((void (*)(void)) fptr)();

    llvm::Constant *parsed =
        parseLiteralElement(top, (char*) &thing, type, size);

    wrap_fn->eraseFromParent();
    (llvm::cast<llvm::Function>(fnc))->eraseFromParent();

    if (parsed) {
        return parsed;
    }

    std::string temp2;
    type->toStringProper(&temp2);
    Error *e = new Error(
        ErrorInst::Generator::CannotParseLiteral,
        top,
        temp2.c_str()
    );
    erep->addError(e);
    return NULL;
}

llvm::Constant *Generator::parseLiteral1(Element::Type *type,
        Node *top,
        int *size) {
    if (type->base_type == Type::Int) {
        if (!top->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "atom", "literal", "list"
            );
            erep->addError(e);
            return NULL;
        }
        Token *t = top->token;

        if (t->type != TokenType::Int) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "integer", "literal", t->tokenType()
            );
            erep->addError(e);
            return NULL;
        }

        llvm::Constant *myconstint =
            getConstantInt(nt->getNativeIntType(),
                           t->str_value.c_str());

        llvm::Value *myconstvalue =
            llvm::cast<llvm::Value>(myconstint);

        llvm::Constant *myconstint2 =
            llvm::cast<llvm::Constant>(myconstvalue);

        return myconstint2;
    }

    int underlying_type =
          (!type->base_type && type->points_to) ? type->points_to->base_type
        : (type->is_array)                      ? type->array_type->base_type
        : 0;

    if (underlying_type == Type::Char) {
        if (!top->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "atom", "literal", "list"
            );
            erep->addError(e);
            return NULL;
        }
        Token *t = top->token;

        if (t->type != TokenType::StringLiteral) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "string", "literal", t->tokenType()
            );
            erep->addError(e);
            return NULL;
        }

        size_t pos = 0;
        while ((pos = t->str_value.find("\\n", pos)) != std::string::npos) {
            t->str_value.replace(pos, 2, "\n");
        }

        *size = strlen(t->str_value.c_str()) + 1;

        return
            llvm::ConstantArray::get(llvm::getGlobalContext(),
                                     t->str_value.c_str(),
                                     true);
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::CannotParseLiteral,
        top,
        temp.c_str()
    );
    erep->addError(e);
    return NULL;
}

int Generator::parseEnumLinkage(Node *n)
{
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "atom", "linkage", "list"
        );
        erep->addError(e);
        return 0;
    }

    if (n->token->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "symbol", "linkage", n->token->tokenType()
        );
        erep->addError(e);
        return 0;
    }

    const char *lnk = n->token->str_value.c_str();

    if (!strcmp(lnk, "extern")) {
        return EnumLinkage::Extern;
    } else if (!strcmp(lnk, "intern")) {
        return EnumLinkage::Intern;
    }

    std::string temp;
    temp.append("'")
    .append(lnk)
    .append("'");

    Error *e = new Error(
        ErrorInst::Generator::UnexpectedElement,
        n,
        "'extern'/'intern'/'opaque'", "linkage",
        temp.c_str()
    );
    erep->addError(e);
    return 0;
}

int Generator::parseStructLinkage(Node *n)
{
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "atom", "linkage", "list"
        );
        erep->addError(e);
        return 0;
    }

    if (n->token->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "symbol", "linkage", n->token->tokenType()
        );
        erep->addError(e);
        return 0;
    }

    const char *lnk = n->token->str_value.c_str();

    if (!strcmp(lnk, "extern")) {
        return StructLinkage::Extern;
    } else if (!strcmp(lnk, "intern")) {
        return StructLinkage::Intern;
    } else if (!strcmp(lnk, "opaque")) {
        return StructLinkage::Opaque;
    }

    std::string temp;
    temp.append("'")
    .append(lnk)
    .append("'");

    Error *e = new Error(
        ErrorInst::Generator::UnexpectedElement,
        n,
        "'extern'/'intern'/'opaque'", "linkage",
        temp.c_str()
    );
    erep->addError(e);
    return 0;
}

int Generator::parseLinkage(Node *n)
{
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "atom", "linkage", "list"
        );
        erep->addError(e);
        return 0;
    }

    if (n->token->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "symbol", "linkage", n->token->tokenType()
        );
        erep->addError(e);
        return 0;
    }

    const char *lnk = n->token->str_value.c_str();

    if (!strcmp(lnk, "extern"))       {
        return Linkage::Extern;
    }
    else if (!strcmp(lnk, "intern"))       {
        return Linkage::Intern;
    }
    else if (!strcmp(lnk, "auto"))         {
        return Linkage::Auto;
    }
    else if (!strcmp(lnk, "extern-c"))     {
        return Linkage::Extern_C;
    }
    else if (!strcmp(lnk, "_extern-weak")) {
        return Linkage::Extern_Weak;
    }

    std::string temp;
    temp.append("'")
    .append(lnk)
    .append("'");

    Error *e = new Error(
        ErrorInst::Generator::UnexpectedElement,
        n,
        "'extern'/'intern'/'auto'/'extern-c'", "linkage",
        temp.c_str()
    );
    erep->addError(e);
    return 0;
}

void Generator::parseFunction(const char *name, Node *n,
                              Element::Function **new_function,
                              int override_linkage,
                              int is_anonymous)
{
    if (!is_anonymous) {
        prefunction_ns = ctx->ns();
    }

    assert(n->list && "must receive a list!");

    /* Ensure this isn't a no-override core. */
    std::set<std::string>::iterator cf =
        core_forms_no_override->find(name);
    if (cf != core_forms_no_override->end()) {
        Error *e = new Error(
            ErrorInst::Generator::ThisCoreFormCannotBeOverridden,
            n
        );
        erep->addError(e);
        return;
    }

    symlist *lst = n->list;

    if (lst->size() < 4) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            n,
            "fn", "3"
        );
        char buf[100];
        sprintf(buf, "%d", (int) lst->size() - 1);
        e->addArgString(buf);
        erep->addError(e);
        return;
    }

    int next_index = 1;
    int always_inline = 0;
    /* Whole modules, as well as specific functions, can be
     * declared as being compile-time-only. If the global cto
     * value is set to one, that overrides a zero value here.
     * However, a global cto value of zero does not take
     * precedence when a function has explicitly declared that it
     * should be CTO. */
    int my_cto = 0;

    /* Function attributes. */

    Node *test = ((*lst)[next_index]);
    if (test->is_list
            && test->list->at(0)->is_token
            && !(test->list->at(0)->token
                 ->str_value.compare("attr"))) {
        symlist *attr_list = test->list;
        std::vector<Node*>::iterator b = attr_list->begin(),
                                     e = attr_list->end();
        ++b;
        for (; b != e; ++b) {
            if ((*b)->is_list) {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                erep->addError(e);
                return;
            }
            if (!((*b)->token->str_value.compare("inline"))) {
                always_inline = 1;
            } else if (!((*b)->token->str_value.compare("cto"))) {
                my_cto = 1;
            } else {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                erep->addError(e);
                return;
            }
        }
        ++next_index;
    }

    if (cto) {
        my_cto = 1;
    }

    /* Linkage. */

    int linkage =
        (override_linkage)
        ? override_linkage
        : parseLinkage((*lst)[next_index]);

    if (!linkage) {
        return;
    }
    if (!override_linkage) {
        ++next_index;
    }

    /* Store the return type index at this point. The return type
     * is not parsed yet, because it may depend on the types of
     * the function parameters. */

    int return_type_index = next_index;

    /* Parse arguments - push onto the list that gets created. */

    Node *nargs = (*lst)[next_index + 1];

    if (!nargs->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            nargs,
            "list", "parameters", "symbol"
        );
        erep->addError(e);
        return;
    }

    symlist *args = nargs->list;

    Element::Variable *var;

    std::vector<Element::Variable *> *fn_args_internal =
        new std::vector<Element::Variable *>;

    /* Parse argument - need to keep names. */

    std::vector<Node *>::iterator node_iter;
    node_iter = args->begin();

    bool varargs = false;

    while (node_iter != args->end()) {
        var = new Element::Variable();
        var->type = NULL;

        parseArgument(var, (*node_iter), false, false);

        if (var->type == NULL) {
            delete var;
            return;
        }

        if (var->type->is_array) {
            delete var;
            Error *e = new Error(
                ErrorInst::Generator::ArraysCannotBeFunctionParameters,
                (*node_iter)
            );
            erep->addError(e);
            return;
        }

        if (var->type->base_type == Type::Void) {
            delete var;
            if (args->size() != 1) {
                Error *e = new Error(
                    ErrorInst::Generator::VoidMustBeTheOnlyParameter,
                    nargs
                );
                erep->addError(e);
                return;
            }
            break;
        }

        /* Have to check that none comes after this. */
        if (var->type->base_type == Type::VarArgs) {
            if ((args->end() - node_iter) != 1) {
                delete var;
                Error *e = new Error(
                    ErrorInst::Generator::VarArgsMustBeLastParameter,
                    nargs
                );
                erep->addError(e);
                return;
            }
            varargs = true;
            fn_args_internal->push_back(var);
            break;
        }

        if (var->type->is_function) {
            delete var;
            Error *e = new Error(
                ErrorInst::Generator::NonPointerFunctionParameter,
                (*node_iter)
            );
            erep->addError(e);
            return;
        }

        fn_args_internal->push_back(var);

        ++node_iter;
    }

    std::vector<llvm::Type*> fn_args;

    /* Convert to llvm args. */

    std::vector<Element::Variable *>::iterator iter;
    iter = fn_args_internal->begin();
    llvm::Type *temp;

    while (iter != fn_args_internal->end()) {
        if ((*iter)->type->base_type == Type::VarArgs) {
            break;
        }
        temp = toLLVMType((*iter)->type, NULL, false);
        if (!temp) {
            failedDaleToLLVMTypeConversion((*iter)->type);
            return;
        }
        fn_args.push_back(temp);
        ++iter;
    }

    /* Return type. First, activate an anonymous namespace and add
     * all of the function parameters to it. This is so that if
     * the return type uses a macro that depends on one of those
     * parameters, it will work properly. */

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    for (std::vector<Element::Variable *>::iterator
            b = fn_args_internal->begin(),
            e = fn_args_internal->end();
            b != e;
            ++b) {
        ctx->ns()->addVariable((*b)->name.c_str(), (*b));
    }

    Element::Type *r_type = parseType((*lst)[return_type_index], false,
                                      false);

    ctx->deactivateNamespace(anon_name.c_str());

    if (r_type == NULL) {
        return;
    }
    if (r_type->is_array) {
        Error *e = new Error(
            ErrorInst::Generator::ReturnTypesCannotBeArrays,
            (*lst)[next_index]
        );
        erep->addError(e);
        return;
    }

    /* Convert the return type into an LLVM type and create the
     * LLVM function type. */

    llvm::Type *llvm_r_type =
        toLLVMType(r_type, NULL, true);
    if (!llvm_r_type) {
        failedDaleToLLVMTypeConversion(r_type);
        return;
    }

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_r_type,
            fn_args,
            varargs
        );

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name,
                                    &new_name,
                                    linkage,
                                    fn_args_internal);

    /* todo: extern_c functions in namespaces. */

    Element::Function *dfn =
        new Element::Function(r_type, fn_args_internal, NULL, 0,
                              &new_name, always_inline);
    dfn->linkage = linkage;
    dfn->cto = my_cto;
    if (!strcmp(name, "setf-copy")) {
        dfn->is_setf_fn = 1;
    } else if (!strcmp(name, "setf-assign")) {
        dfn->is_setf_fn = 1;
    } else if (!strcmp(name, "destroy")) {
        dfn->is_destructor = 1;
    }

    /* If the function is a setf function, the return type must be
     * bool. */

    if (dfn->is_setf_fn) {
        if (!r_type->isEqualTo(type_bool)) {
            Error *e = new Error(
                ErrorInst::Generator::SetfOverridesMustReturnBool,
                (*lst)[return_type_index]
            );
            erep->addError(e);
            return;
        }
    }

    /* If the function already exists, but has a different
     * prototype, then fail. */

    llvm::Function *temp23;
    if ((temp23 = mod->getFunction(new_name.c_str()))) {
        Element::Function *temp25 =
            ctx->getFunction(new_name.c_str(), NULL,
                             NULL, 0);
        if (temp25 && !temp25->isEqualTo(dfn)) {
            Error *e = new Error(
                ErrorInst::Generator::RedeclarationOfFunctionOrMacro,
                n,
                name
            );
            erep->addError(e);
            return;
        }
        if (temp25 && !temp25->attrsAreEqual(dfn)) {
            Error *e = new Error(
                ErrorInst::Generator::AttributesOfDeclAndDefAreDifferent,
                n,
                name
            );
            erep->addError(e);
            return;
        }
    }

    if (current_once_tag.length() > 0) {
        dfn->once_tag = current_once_tag;
    }

    llvm::Constant *fnc =
        mod->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    llvm::Function *fn = llvm::dyn_cast<llvm::Function>(fnc);

    /* If fn is null, then the function already exists and the
     * extant function has a different prototype, so it's an
     * invalid redeclaration. If fn is not null, but has content,
     * then it's an invalid redefinition. */

    if ((!fn) || (fn->size())) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfFunctionOrMacro,
            n,
            name
        );
        erep->addError(e);
        return;
    }

    if (always_inline) {
        fn->addFnAttr(llvm::Attribute::AlwaysInline);
    }

    fn->setCallingConv(llvm::CallingConv::C);
    fn->setLinkage(
        (lst->size() == (unsigned) (next_index + 2))
        ? (ctx->toLLVMLinkage(override_linkage))
        : (ctx->toLLVMLinkage(linkage))
    );

    llvm::Function::arg_iterator largs = fn->arg_begin();

    iter = fn_args_internal->begin();
    while (iter != fn_args_internal->end()) {
        if ((*iter)->type->base_type == Type::VarArgs) {
            /* varargs - finish */
            break;
        }

        llvm::Value *temp = largs;
        ++largs;
        temp->setName((*iter)->name.c_str());
        (*iter)->value = temp;
        ++iter;
    }

    /* If this is an extern-c function, if any non-extern-c function
     * with this name already exists, don't add the current
     * function, and vice-versa.
     *
     * (Dropping this for now, because you may well want
     * non-extern-c and extern-c functions to coexist - think
     * 'read', 'write', 'open'.)
     */

    /* Add the function to the context. */
    dfn->llvm_function = fn;
    if (!ctx->ns()->addFunction(name, dfn, n)) {
        return;
    }

    if (new_function) {
        *new_function = dfn;
    }

    /* If the list has only four arguments, function is a
     * declaration and you can return straightaway. */

    if (lst->size() == (unsigned) (next_index + 2)) {
        return;
    }

    ctx->activateAnonymousNamespace();
    std::string anon_name2 = ctx->ns()->name;

    global_functions.push_back(dfn);
    global_function = dfn;

    parseFunctionBody(dfn, fn, n, (next_index + 2), is_anonymous);

    global_functions.pop_back();
    if (global_functions.size()) {
        global_function = global_functions.back();
    } else {
        global_function = NULL;
    }

    if (!strcmp(name, "main")
            && ctx->getVariable("stdin")
            && ctx->getVariable("stdout")
            && ctx->getVariable("stderr")) {

        std::vector<llvm::Value *> call_args;
        std::vector<Element::Type *> tempparams;
        Element::Function *ic =
            ctx->getFunction("init-channels", &tempparams,
                             NULL, 0);
        if (!ic or !ic->llvm_function) {
            fprintf(stderr,
                    "Internal error: cannot find init-channels "
                    "function.\n");
            abort();
        }

        llvm::Function::iterator i = fn->begin();
        llvm::BasicBlock *b = i;

        if (b->empty()) {
            llvm::CallInst::Create(
                ic->llvm_function,
                llvm::ArrayRef<llvm::Value*>(call_args),
                "",
                b
            );
        } else {
            llvm::Instruction *fnp = b->getFirstNonPHI();
            if (fnp) {
                llvm::CallInst::Create(
                    ic->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args),
                    "",
                    fnp
                );
            } else {
                llvm::CallInst::Create(
                    ic->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args),
                    "",
                    b
                );
            }
        }
    }

    ctx->deactivateNamespace(anon_name2.c_str());

    return;
}

/* This always assumes that n is a list.
   If max_args is -1, then max_args is not checked. */
int Generator::assertArgNums(const char *form_name,
                             Node *n,
                             int min_args,
                             int max_args)
{
    symlist *lst = n->list;

    char buf1[100];
    char buf2[100];

    if (!lst) {
        fprintf(stderr,
                "Internal error: no list to assert arg nums.\n");
        return 0;
    }

    int num_args = (int) lst->size() - 1;

    if (min_args == max_args) {
        if (num_args == min_args) {
            return 1;
        } else {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                n,
                form_name
            );
            sprintf(buf1, "%d", min_args);
            sprintf(buf2, "%d", num_args);
            e->addArgString(buf1);
            e->addArgString(buf2);
            erep->addError(e);
            return 0;
        }
    }

    if (num_args < min_args) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            n,
            form_name
        );
        sprintf(buf1, "%d", min_args);
        sprintf(buf2, "%d", num_args);
        e->addArgString(buf1);
        e->addArgString(buf2);
        erep->addError(e);
        return 0;
    }

    if ((max_args != -1) && (num_args > max_args)) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMaximumNumberOfArgs,
            n,
            form_name
        );
        sprintf(buf1, "%d", max_args);
        sprintf(buf2, "%d", num_args);
        e->addArgString(buf1);
        e->addArgString(buf2);
        erep->addError(e);
        return 0;
    }

    return 1;
}

int Generator::assertArgIsAtom(const char *form_name,
                               Node *n,
                               const char *arg_number)
{
    if (n->is_token) {
        return 1;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "an atom", arg_number, "a list"
    );
    erep->addError(e);
    return 0;
}

int Generator::assertArgIsList(const char *form_name,
                               Node *n,
                               const char *arg_number)
{
    if (n->is_list) {
        return 1;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a list", arg_number, "a symbol"
    );
    erep->addError(e);
    return 0;
}

int Generator::assertAtomIsSymbol(const char *form_name,
                                  Node *n,
                                  const char *arg_number)
{
    if (n->token && (n->token->type == TokenType::String)) {
        return 1;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a symbol", arg_number, n->token->tokenType()
    );
    erep->addError(e);
    return 0;
}

int Generator::assertAtomIsStringLiteral(const char *form_name,
        Node *n,
        const char *arg_number)
{
    if (n->token && (n->token->type == TokenType::StringLiteral)) {
        return 1;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a string literal", arg_number, n->token->tokenType()
    );
    erep->addError(e);
    return 0;
}

Node *Generator::parseDerefStructDeref(Node *n)
{
    assert(n->list && "parseDerefStructDeref must receive a list!");

    if (!assertArgNums("@:@", n, 2, 2)) {
        return NULL;
    }

    symlist *lst = n->list;

    // (@:@ array element) => (@ (: (@ array) element))

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("@");

    // Create a new nodelist, first element @, second element the
    // second element from the first list.

    symlist *newlst = new std::vector<Node *>;
    Token *de = new Token(TokenType::String,0,0,0,0);
    de->str_value.append("@");
    Node *nde = new Node(de);
    (*lst)[1]->copyMetaTo(nde);
    newlst->push_back(nde);
    newlst->push_back((*lst)[1]);

    // Create a new nodelist, first element :, second element
    // newlst, third element the third element from the first
    // list. Adjust the original list to suit.

    symlist *newlst2 = new std::vector<Node *>;
    Token *ad = new Token(TokenType::String,0,0,0,0);
    ad->str_value.append(":");
    Node *nad = new Node(ad);
    (*lst)[1]->copyMetaTo(nad);
    newlst2->push_back(nad);
    Node *nnewlst = new Node(newlst);
    (*lst)[1]->copyMetaTo(nnewlst);
    newlst2->push_back(nnewlst);
    newlst2->push_back((*lst)[2]);

    (*lst)[1] = new Node(newlst2);
    nnewlst->copyMetaTo((*lst)[1]);
    lst->pop_back();

    // Return the original node.
    return n;
}

Node *Generator::parseDerefStruct(Node *n)
{
    assert(n->list && "parseDerefStruct must receive a list!");

    if (!assertArgNums(":@", n, 2, 2)) {
        return NULL;
    }

    symlist *lst = n->list;

    // (:@ array element) => (: (@ array) element)

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append(":");

    // Create a new nodelist, first element @ and second element
    // as per the original second element. Remove that element
    // from the provided list, and make the second element of the
    // provided list this new list.

    symlist *newlst = new std::vector<Node *>;
    Token *ad = new Token(TokenType::String,0,0,0,0);
    ad->str_value.append("@");
    newlst->push_back(new Node(ad));
    newlst->push_back((*lst)[1]);
    (*lst)[1] = new Node(newlst);

    // Return the original node.
    return n;
}

Node *Generator::parseStructDeref(Node *n)
{
    assert(n->list && "parseStructDeref must receive a list!");

    if (!assertArgNums("@:", n, 2, 2)) {
        return NULL;
    }

    symlist *lst = n->list;

    // (@: array element) => (@ (: array element))

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("@");

    // Create a new nodelist, first element :, second element and
    // third element from the current list. Remove the third
    // element from the provided list, make the second element of
    // the provided list this new list.

    symlist *newlst = new std::vector<Node *>;
    Token *ad = new Token(TokenType::String,0,0,0,0);
    ad->str_value.append(":");
    newlst->push_back(new Node(ad));
    newlst->push_back((*lst)[1]);
    newlst->push_back((*lst)[2]);
    (*lst)[1] = new Node(newlst);
    lst->pop_back();

    // Return the original node.
    return n;
}

Node *Generator::parseArrayDeref(Node *n)
{
    assert(n->list && "parseArrayDeref must receive a list!");

    if (!assertArgNums("@$", n, 2, 2)) {
        return NULL;
    }

    symlist *lst = n->list;

    // (@$ array element) => (@ ($ array element))

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("@");

    // Create a new nodelist, first element $, second element and
    // third element from the current list. Remove the third
    // element from the provided list, make the second element of
    // the provided list this new list.

    symlist *newlst = new std::vector<Node *>;
    Token *ad = new Token(TokenType::String,0,0,0,0);
    ad->str_value.append("$");
    newlst->push_back(new Node(ad));
    newlst->push_back((*lst)[1]);
    newlst->push_back((*lst)[2]);
    (*lst)[1] = new Node(newlst);
    lst->pop_back();

    // Return the original node.
    return n;
}

Node *Generator::parseSetv(Node *n)
{
    assert(n->list && "parseSetv must receive a list!");

    if (!assertArgNums("setv", n, 2, 2)) {
        return NULL;
    }

    symlist *lst = n->list;
    Node *varn = (*lst)[1];

    if (!assertArgIsAtom("setv", varn, "1")) {
        return NULL;
    }

    // (setv a 10) => (setf (# a) 10)

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("setf");

    // Create a new nodelist, first element #, second element
    // whatever the current second element is, make it the second
    // element of the current list.

    symlist *newlst = new std::vector<Node *>;
    Token *addrof = new Token(TokenType::String,0,0,0,0);
    addrof->str_value.append("#");
    newlst->push_back(new Node(addrof));
    newlst->push_back((*lst)[1]);
    (*lst)[1] = new Node(newlst);

    // Return the original node.
    return n;
}

int Generator::assertTypeEquality(const char *form_name,
                                  Node *n,
                                  Element::Type *got,
                                  Element::Type *expected,
                                  int ignore_arg_constness)
{
    if (got->isEqualTo(expected, ignore_arg_constness)) {
        return 1;
    }

    std::string got_str;
    std::string exp_str;
    got->toStringProper(&got_str);
    expected->toStringProper(&exp_str);

    Error *e = new Error(
        ((!strcmp(form_name, "return"))
         ? ErrorInst::Generator::IncorrectReturnType
         : ErrorInst::Generator::IncorrectType),
        n,
        exp_str.c_str(), got_str.c_str()
    );
    erep->addError(e);
    return 0;
}

/* There are a number of 'treat_as_terminator' calls here, which
 * is just feral given that each ParseResult adds a ret
 * instruction, but something else is adding extra bits to the end
 * and it's the easiest way to get things to work again. */
bool Generator::parseReturn(Element::Function *dfn,
                            llvm::BasicBlock *block,
                            Node *n,
                            bool getAddress,
                            bool prefixed_with_core,
                            ParseResult *pr)
{
    assert(n->list && "parseReturn must receive a list!");

    symlist *lst = n->list;

    if (!assertArgNums("return", n, 0, 1)) {
        return false;
    }
    if (lst->size() == 1) {
        llvm::IRBuilder<> builder(block);
        scopeClose(dfn, block, NULL);
        builder.CreateRetVoid();
        setPr(pr, block, type_int, llvm_native_zero);
        pr->do_not_destruct       = 1;
        pr->do_not_copy_with_setf = 1;
        pr->treat_as_terminator   = 1;
        return true;
    }

    ParseResult p;
    bool res =
        parseFunctionBodyInstr(
            dfn, block, (*lst)[1], getAddress, NULL, &p
        );
    if (!res) {
        return false;
    }
    if (!assertTypeEquality("return", n, p.type, dfn->return_type)) {
        return false;
    }
    block = p.block;
    llvm::IRBuilder<> builder(block);
    /* Both branches here create a ParseResult with an integer
     * value but a type that is the same as the return type of the
     * function. This is purposeful - the type is so that if other
     * instructions occur between here and the conclusion of the
     * function (for whatever reason), the last value continues to
     * have the correct type, and in any event the value from a
     * parseReturn parse result should never be used. (Woops, this
     * last part isn't correct - see e.g. the call to CreateCondBr
     * in parseIf. So, return the proper value in the second
     * branch.) */
    if (p.type->base_type == Type::Void) {
        scopeClose(dfn, block, NULL);
        builder.SetInsertPoint(block);
        builder.CreateRetVoid();
        setPr(pr, block, dfn->return_type, llvm_native_zero);
        pr->do_not_destruct       = 1;
        pr->do_not_copy_with_setf = 1;
        pr->treat_as_terminator   = 1;
        return true;
    } else {
        scopeClose(dfn, block, NULL);
        builder.SetInsertPoint(block);
        builder.CreateRet(p.value);
        setPr(pr, block, dfn->return_type, p.value);
        pr->do_not_destruct       = 1;
        pr->do_not_copy_with_setf = 1;
        pr->treat_as_terminator   = 1;
        return true;
    }
}

bool Generator::parseSetf(Element::Function *dfn,
                            llvm::BasicBlock *block,
                            Node *n,
                            bool getAddress,
                            bool prefixed_with_core,
                            ParseResult *pr)
{
    assert(n->list && "parseSetf must receive a list!");

    symlist *lst = n->list;

    if (!assertArgNums("setf", n, 2, 2)) {
        return false;
    }

    /* Used to use getAddress for the first argument, but now setf
     * always takes a pointer as its first argument, to facilitate
     * overloading etc. */

    ParseResult pr_variable;
    bool res =
        parseFunctionBodyInstr(dfn, block, (*lst)[1], false, NULL,
                               &pr_variable);

    if (!res) {
        return false;
    }

    /* Make sure that the first argument is a pointer. */

    if (!pr_variable.type->points_to) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            (*lst)[1],
            "setf", "a pointer", "1", "a value"
        );
        erep->addError(e);
        return false;
    }

    /* Can't modify const variables. */

    if (pr_variable.type->points_to->is_const) {
        Error *e = new Error(
            ErrorInst::Generator::CannotModifyConstVariable,
            n
        );
        erep->addError(e);
        return false;
    }

    Node *val_node = (*lst)[2];
    val_node = parseOptionalMacroCall(val_node);
    if (!val_node) {
        return false;
    }

    /* Previously: 'if pr_value is a variable, use the value
     * directly, rather than calling PFBI and getting a copy'.
     * That only worked because there was a branch further down
     * that accepted two pointers-to-type as the arguments to this
     * form, and set the destination to be the value of the
     * source, directly. That form of call is no longer supported,
     * because it was extremely error-prone and unintuitive. So
     * this will now cause additional copies, which is
     * unfortunate; there is probably a way to make it better. */

    llvm::IRBuilder<> builder(pr_variable.block);
    ParseResult pr_value;
    res =
        parseFunctionBodyInstr(
            dfn, pr_variable.block, val_node, false,
            pr_variable.type->points_to,
            &pr_value
        );

    if (!res) {
        return NULL;
    }

    builder.SetInsertPoint(pr_value.block);

    /* If overridden setf exists, and pr_value is a value of the
     * pointee type of pr_variable, then call overridden setf
     * after allocating memory for pr_value and copying it into
     * place. */

    if (!prefixed_with_core
            && pr_value.type->isEqualTo(pr_variable.type->points_to)) {
        std::vector<Element::Type *> types;
        types.push_back(pr_variable.type);
        types.push_back(pr_variable.type);
        Element::Function *over_setf =
            ctx->getFunction("setf-assign", &types, NULL, 0);
        if (!over_setf) {
            goto cont1;
        }
        llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(
                                        toLLVMType(pr_value.type, NULL, false)
                                    )
                                );
        builder.CreateStore(pr_value.value, new_ptr2);
        std::vector<llvm::Value *> call_args;
        call_args.push_back(pr_variable.value);
        call_args.push_back(new_ptr2);
        llvm::Value *ret =
            builder.CreateCall(over_setf->llvm_function,
                               llvm::ArrayRef<llvm::Value*>(call_args));

        ParseResult temp;
        pr_variable.block = pr_value.block; 
        bool mres = destructIfApplicable(&pr_variable, &builder, &temp);
        if (!mres) {
            return false;
        }
        pr_value.block = temp.block;
        mres = destructIfApplicable(&pr_value, &builder, &temp);
        if (!mres) {
            return false;
        }
        setPr(pr, temp.block, type_bool, ret);
        return true;
    }

cont1:

    /* If an appropriate setf definition exists, which matches
     * the arguments exactly, then use it. */

    if (!prefixed_with_core) {
        std::vector<Element::Type *> types;
        types.push_back(pr_variable.type);
        types.push_back(pr_value.type);
        Element::Function *over_setf =
            ctx->getFunction("setf-assign", &types, NULL, 0);
        if (!over_setf) {
            goto cont2;
        }
        std::vector<llvm::Value *> call_args;
        call_args.push_back(pr_variable.value);
        call_args.push_back(pr_value.value);
        llvm::Value *ret =
            builder.CreateCall(over_setf->llvm_function,
                               llvm::ArrayRef<llvm::Value*>(call_args));

        ParseResult temp;
        pr_variable.block = pr_value.block;
        bool mres = destructIfApplicable(&pr_variable, &builder, &temp);
        if (!mres) {
            return false;
        }
        pr_value.block = temp.block;
        mres = destructIfApplicable(&pr_value, &builder, &temp);
        if (!mres) {
            return false;
        }

        setPr(pr, temp.block, type_bool, ret);
        return true;
    }

cont2:

    if (pr_value.type->isEqualTo(pr_variable.type->points_to)) {
        builder.CreateStore(pr_value.value, pr_variable.value);

        ParseResult temp;
        pr_variable.block = pr_value.block;
        bool mres = destructIfApplicable(&pr_variable, &builder, &temp);
        if (!mres) {
            return false;
        }
        pr_value.block = temp.block;
        mres = destructIfApplicable(&pr_value, &builder, &temp);
        if (!mres) {
            return false;
        }

        setPr(pr, temp.block, type_bool, llvm_bool_true);
        pr->do_not_copy_with_setf = 1;
        return true;
    }

    /* todo: it would be good to also show the setf-assign
     * candidates here, if applicable. */
    assertTypeEquality("setf", (*lst)[2],
                       pr_value.type,
                       pr_variable.type->points_to);

    return false;
}

bool Generator::parseDereference(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        bool getAddress,
        bool prefixed_with_core,
        ParseResult *pr)
{
    assert(n->list && "parseDereference must receive a list!");

    symlist *lst = n->list;

    if (!assertArgNums("@", n, 1, 1)) {
        return false;
    }

    ParseResult p;
    bool res =
        parseFunctionBodyInstr(dfn, block, (*lst)[1], false, NULL,
                               &p);

    if (!res) {
        return false;
    }

    /* It should be a pointer. */

    if (!(p.type->points_to)) {
        std::string temp;
        p.type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::CannotDereferenceNonPointer,
            n,
            temp.c_str()
        );
        erep->addError(e);
        return false;
    }

    if (p.type->points_to->base_type == Type::Void) {
        Error *e = new Error(
            ErrorInst::Generator::CannotDereferenceVoidPointer,
            n
        );
        erep->addError(e);
        return false;
    }

    /* If getAddress is false (the usual case), append a load
     * instruction, otherwise just return the value. */

    setPr(pr, p.block, NULL, NULL);

    if (!getAddress) {
        llvm::IRBuilder<> builder(p.block);
        llvm::Value *res =
            llvm::cast<llvm::Value>(builder.CreateLoad(p.value));

        pr->type  = p.type->points_to;
        pr->value = res;
    } else {
        pr->type  = p.type;
        pr->value = p.value;
    }

    /* If this isn't here, then an overloaded setf that calls @
     * on the pointer to the underlying value will not be able to
     * work, because even (core @ x) will return a pointee to x,
     * which will be copied to the caller of this function. */
    if (prefixed_with_core) {
        pr->do_not_copy_with_setf = 1;
    }

    if (hasRelevantDestructor(&p)) {
        ParseResult temp;
        bool res = destructIfApplicable(&p, NULL, &temp);
        if (!res) {
            return false;
        }
        pr->block = temp.block;
    }

    return true;
}

bool Generator::parseGetDNodes(Element::Function *dfn,
                                       llvm::BasicBlock *block,
                                       Node *n,
                                       bool getAddress,
                                       bool prefixed_with_core,
                                       ParseResult *pr)
{
    if (!assertArgNums("get-dnodes", n, 1, 1)) {
        return false; 
    }

    llvm::Value *v = IntNodeToStaticDNode(n->list->at(1), NULL);

    llvm::IRBuilder<> builder(block);

    setPr(pr, block, type_pdnode, v);
    return true;
}

bool Generator::parseAddressOf(Element::Function *dfn,
                                       llvm::BasicBlock *block,
                                       Node *n,
                                       bool getAddress,
                                       bool prefixed_with_core,
                                       ParseResult *pr)
{
    assert(n->list && "parseAddressOf must receive a list!");

    if (!assertArgNums("#", n, 1, -1)) {
        return false;
    }
    symlist *lst = n->list;

    /* If the argument is a single token, and that token
     * designates a variable, then return the address directly. */
    Node *nn = (*lst)[1];
    if (nn->is_token) {
        Element::Variable *var =
            ctx->getVariable(nn->token->str_value.c_str());
        if (var) {
            setPr(pr, block,
                  tr->getPointerType(var->type),
                  var->value);
            return true;
        }
    }

    int error_count = erep->getErrorTypeCount(ErrorType::Error);

    /* There's no destruction of the argument here, because
     * there's no corresponding copy with setf (or an overloaded
     * setf). (todo: this comment does not appear to be correct.) */

    bool res =
        parseFunctionBodyInstr(
            dfn, block, (*lst)[1], true, NULL, pr
        );

    int diff = erep->getErrorTypeCount(ErrorType::Error)
               - error_count;

    if (!res) {
        std::vector<Error*> errors;
        while (diff--) {
            errors.push_back(erep->popLastError());
        }

        /* If the second element is a token, check if it is the name
         * of a function. If it is, return a pointer to that
         * function. */

        Element::Function *fn;

        if (((*lst)[1])->is_token &&
                (fn =
                     ctx->getFunction(
                         ((*lst)[1])->token->str_value.c_str(),
                         NULL,
                         NULL,
                         0
                     )
                )
           ) {
            /* Also do the 'typed' version where types have been
             * provided (lst->size > 2), regardless of whether it's
             * overloaded. */

            int is_overloaded =
                ctx->isOverloadedFunction(
                    ((*lst)[1])->token->str_value.c_str()
                );
            int lst_size = lst->size();

            if (is_overloaded || lst_size > 2) {
                /* Parse each type, add it to a vector of types, get
                 * the relevant function, return it. */

                std::vector<Element::Type *> types;
                std::vector<Node *>::iterator iter = lst->begin();
                ++iter;
                ++iter;
                while (iter != lst->end()) {
                    Element::Type *type = parseType((*iter),
                                                    false, false);
                    if (!type) {
                        return NULL;
                    }
                    types.push_back(type);
                    ++iter;
                }
                if (types.empty()) {
                    types.push_back(type_void);
                }

                Element::Function *closest_fn = NULL;

                fn =
                    ctx->getFunction(((*lst)[1])->token->str_value.c_str(),
                                     &types,
                                     &closest_fn,
                                     0);

                std::vector<Element::Type *>::iterator titer =
                    types.begin();

                std::string args;
                while (titer != types.end()) {
                    (*titer)->toStringProper(&args);
                    ++titer;
                    if (titer != types.end()) {
                        args.append(" ");
                    }
                }
                if (!fn) {
                    Error *e;
                    if (closest_fn) {
                        std::string expected;
                        std::vector<Element::Variable *>::iterator viter;
                        viter = closest_fn->parameter_types->begin();
                        if (closest_fn->is_macro) {
                            ++viter;
                        }
                        while (viter != closest_fn->parameter_types->end()) {
                            (*viter)->type->toStringProper(&expected);
                            expected.append(" ");
                            ++viter;
                        }
                        if (expected.size() > 0) {
                            expected.erase(expected.size() - 1, 1);
                        }
                        e = new Error(
                            ErrorInst::Generator::OverloadedFunctionOrMacroNotInScopeWithClosest,
                            n,
                            ((*lst)[1])->token->str_value.c_str(),
                            args.c_str(),
                            expected.c_str()
                        );

                    } else {
                        e = new Error(
                            ErrorInst::Generator::OverloadedFunctionOrMacroNotInScope,
                            n,
                            ((*lst)[1])->token->str_value.c_str(),
                            args.c_str()
                        );
                    }
                    erep->addError(e);
                    return NULL;
                }
            }

            Element::Type *type = new Element::Type();
            type->is_function = 1;
            type->return_type = fn->return_type;

            std::vector<Element::Type *> *parameter_types =
                new std::vector<Element::Type *>;

            std::vector<Element::Variable *>::iterator iter;

            iter = fn->parameter_types->begin();

            while (iter != fn->parameter_types->end()) {
                parameter_types->push_back((*iter)->type);
                ++iter;
            }

            type->parameter_types = parameter_types;
            setPr(pr, block, tr->getPointerType(type),
                       llvm::cast<llvm::Value>(fn->llvm_function)
                   );
        } else {
            for (std::vector<Error*>::iterator b = errors.begin(),
                    e = errors.end();
                    b != e;
                    ++b) {
                erep->addError((*b));
            }
            return false;
        }
    }

    /* Confirm that pr's value "points to" something - this error
     * message should probably be "cannot take address of
     * non-lvalue" once confirmed all good. */

    if (!pr->type->points_to) {
        Error *e = new Error(
            ErrorInst::Generator::CannotTakeAddressOfNonLvalue,
            (*lst)[1]
        );
        erep->addError(e);
        return false;
    }

    return true;
}

bool Generator::parseLabel(Element::Function *dfn,
                                   llvm::BasicBlock *block,
                                   Node *n,
                                   bool getAddress,
                                   bool prefixed_with_core,
                                   ParseResult *pr)
{
    assert(n->list && "parseLabel must receive a list!");

    if (!assertArgNums("label", n, 1, 1)) {
        return false;
    }

    symlist *lst = n->list;
    Node *lname = (*lst)[1];

    if (!assertArgIsAtom("label", lname, "1")) {
        return false;
    }
    if (!assertAtomIsSymbol("label", lname, "1")) {
        return false;
    }
    Token *t = lname->token;

    /* Create a block with this name. */

    llvm::BasicBlock *new_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 t->str_value.c_str(),
                                 block->getParent());

    /* If the existing block does not terminate, create a branch to
     * this label. */

    if (block->size() == 0
            || !(block->back().isTerminator())) {
        llvm::IRBuilder<> builder(block);
        builder.CreateBr(new_block);
    }

    if (dfn->getLabel(t->str_value.c_str())) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfLabel,
            n,
            t->str_value.c_str()
        );
        erep->addError(e);
        return NULL;
    }

    int index = ctx->ns()->lv_index++;
    Element::Label *my_label = new Element::Label();
    my_label->block = new_block;
    my_label->ns = ctx->ns();
    my_label->index = index;
    dfn->addLabel(t->str_value.c_str(), my_label);

    setPr(pr, new_block, type_int, llvm_native_zero);
    return true;
}

int Generator::assertIsPointerType(const char *form_name,
                                   Node *n,
                                   Element::Type *type,
                                   const char *arg_number)
{
    if (type->points_to) {
        return 1;
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a pointer", arg_number, temp.c_str()
    );
    erep->addError(e);
    return 0;
}

int Generator::assertIsPointerOrIntegerType(const char *form_name,
        Node *n,
        Element::Type *type,
        const char *arg_number)
{
    if (type->points_to || type->isIntegerType()) {
        return 1;
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a pointer or integer", arg_number, temp.c_str()
    );
    erep->addError(e);
    return 0;
}

int Generator::assertIsIntegerType(const char *form_name,
                                   Node *n,
                                   Element::Type *type,
                                   const char *arg_number)
{
    if (type->base_type == Type::Int) {
        return 1;
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "int", arg_number, temp.c_str()
    );
    erep->addError(e);
    return 0;
}

bool Generator::parsePtrEquals(Element::Function *dfn,
                                       llvm::BasicBlock *block,
                                       Node *n,
                                       bool getAddress,
                                       bool prefixed_with_core,
                                       ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("p=", n, 2, 2)) {
        return false;
    }

    symlist *lst = n->list;

    ParseResult p1;
    bool mres = parseFunctionBodyInstr(
                         dfn, block, (*lst)[1], getAddress, NULL, &p1
                     );
    if (!mres) {
        return false;
    }
    if (!assertIsPointerType("p=", n, p1.type, "1")) {
        return false;
    }

    ParseResult p2;
    mres = parseFunctionBodyInstr(
                          dfn, p1.block, (*lst)[2], getAddress, NULL,
                          &p2
                      );
    if (!mres) {
        return false;
    }
    if (!assertIsPointerType("p=", n, p2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(p2.block);
    llvm::Value *res = llvm::cast<llvm::Value>(
                           builder.CreateICmpEQ(p1.value, p2.value)
                       );

    setPr(pr, p2.block, type_bool, res);

    p1.block = p2.block;
    ParseResult ret;
    ret.block = p2.block;
    if (hasRelevantDestructor(&p1)) {
        destructIfApplicable(&p1, NULL, &ret);
        p2.block = ret.block;
    }
    if (hasRelevantDestructor(&p2)) {
        destructIfApplicable(&p2, NULL, &ret);
    }
    pr->block = ret.block;

    return true;
}

bool Generator::parsePtrAdd(Element::Function *dfn,
                               llvm::BasicBlock *block,
                                    Node *n,
                                    bool getAddress,
                                    bool prefixed_with_core,
                                    ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("p+", n, 2, 2)) {
        return false;
    }

    symlist *lst = n->list;

    ParseResult ptr;
    bool mres = parseFunctionBodyInstr(
                         dfn, block, (*lst)[1], getAddress, NULL, &ptr
                     );
    if (!mres) {
        return false;
    }
    if (!assertIsPointerType("p+", (*lst)[1], ptr.type, "1")) {
        return false;
    }

    ParseResult val;
    mres = parseFunctionBodyInstr(
                 dfn, ptr.block, (*lst)[2], getAddress, NULL, &val
          );
    if (!mres) {
        return NULL;
    }
    if (!assertIsPointerOrIntegerType("p+", (*lst)[2], val.type, "2")) {
        return NULL;
    }

    llvm::BasicBlock *mynextb;
    ParseResult temp2;

    if (val.type->isIntegerType()) {
        ParseResult size;
        mres = getSizeofType(val.block, ptr.type->points_to, &size);
        if (!mres) {
            return false;
        }
        mres = doCast(size.block, size.value, size.type,
                     type_intptr, (*lst)[2], 0, &temp2);
        if (!mres) {
            return false;
        }
        mynextb = temp2.block;
    } else {
        mynextb = val.block;
    }

    ParseResult cast1;
    ParseResult cast2;
    mres = doCast(mynextb, ptr.value, ptr.type,
                 type_intptr, (*lst)[1], 0, &cast1);
    if (!mres) {
        return false;
    }
    mres = doCast(cast1.block, val.value, val.type,
                 type_intptr, (*lst)[2], 0, &cast2);
    if (!mres) {
        return false;
    }

    llvm::Value *res;

    {
        llvm::IRBuilder<> builder(cast2.block);

        if (val.type->isIntegerType()) {
            res = llvm::cast<llvm::Value>(
                      builder.CreateAdd(cast1.value,
                                        builder.CreateMul(
                                            temp2.value, cast2.value))
                  );
        } else {
            res = llvm::cast<llvm::Value>(
                      builder.CreateAdd(cast1.value, cast2.value)
                  );
        }
    }

    ParseResult final_res;
    doCast(cast2.block, res, type_intptr, ptr.type, n, 0, &final_res);

    ptr.block = final_res.block;
    ParseResult temp;
    if (hasRelevantDestructor(&ptr)) {
        destructIfApplicable(&ptr, NULL, &temp);
        val.block = temp.block;
    }
    if (hasRelevantDestructor(&val)) {
        destructIfApplicable(&val, NULL, &temp);
        final_res.block = temp.block;
    }
    setPr(pr, final_res.block, final_res.type, final_res.value);

    return true;
}

bool Generator::parsePtrSubtract(Element::Function *dfn,
                               llvm::BasicBlock *block,
                                    Node *n,
                                    bool getAddress,
                                    bool prefixed_with_core,
                                    ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("p-", n, 2, 2)) {
        return false;
    }

    symlist *lst = n->list;

    ParseResult ptr;
    bool res = parseFunctionBodyInstr(
                         dfn, block, (*lst)[1], getAddress, NULL, &ptr
                     );
    if (!res) {
        return false;
    }
    if (!assertIsPointerType("p-", (*lst)[1], ptr.type, "1")) {
        return false;
    }

    ParseResult val;
    res = parseFunctionBodyInstr(
                 dfn, ptr.block, (*lst)[2], getAddress, NULL, &val
          );
    if (!res) {
        return NULL;
    }
    if (!assertIsPointerOrIntegerType("p-", (*lst)[2], val.type, "2")) {
        return NULL;
    }

    llvm::BasicBlock *mynextb;
    ParseResult temp2;

    if (val.type->isIntegerType()) {
        ParseResult size;
        res = getSizeofType(val.block, ptr.type->points_to, &size);
        if (!res) {
            return false;
        }
        res = doCast(size.block, size.value, size.type,
                     type_intptr, (*lst)[2], 0, &temp2);
        if (!res) {
            return false;
        }
        mynextb = temp2.block;
    } else {
        mynextb = val.block;
    }

    ParseResult cast1;
    ParseResult cast2;
    res = doCast(mynextb, ptr.value, ptr.type,
                 type_intptr, (*lst)[1], 0, &cast1);
    if (!res) {
        return false;
    }
    res = doCast(cast1.block, val.value, val.type,
                 type_intptr, (*lst)[2], 0, &cast2);
    if (!res) {
        return false;
    }

    llvm::Value *vres;

    {
        llvm::IRBuilder<> builder(cast2.block);

        if (val.type->isIntegerType()) {
            vres = llvm::cast<llvm::Value>(
                      builder.CreateSub(cast1.value,
                                        builder.CreateMul(
                                            temp2.value, cast2.value))
                  );
        } else {
            vres = llvm::cast<llvm::Value>(
                      builder.CreateSub(cast1.value, cast2.value)
                  );
        }
    }

    ParseResult final_res;
    doCast(cast2.block, vres, type_intptr, ptr.type, n, 0, &final_res);

    ParseResult temp;
    if (hasRelevantDestructor(&ptr)) {
        res = destructIfApplicable(&ptr, NULL, &temp);
        val.block = temp.block;
    }
    if (hasRelevantDestructor(&val)) {
        res = destructIfApplicable(&val, NULL, &temp);
        final_res.block = temp.block;
    }
    setPr(pr, final_res.block, final_res.type, final_res.value);

    return true;
}

bool Generator::parsePtrLessThan(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        bool getAddress,
        bool prefixed_with_core,
        ParseResult *pr)
{
    if (!assertArgNums("p<", n, 2, 2)) {
        return NULL;
    }

    symlist *lst = n->list;

    ParseResult p;
    bool res = parseFunctionBodyInstr(
                    dfn, block, (*lst)[1], getAddress, NULL, &p
               );
    if (!res) {
        return false;
    }
    if (!assertIsPointerType("p<", (*lst)[1], p.type, "1")) {
        return false;
    }

    ParseResult p2;
    res = parseFunctionBodyInstr(
               dfn, p.block, (*lst)[2], getAddress, NULL, &p2
          );
    if (!res) {
        return false;
    }
    if (!assertIsPointerType("p<", (*lst)[2], p2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(p2.block);
    llvm::Value *vres = llvm::cast<llvm::Value>(
                           builder.CreateICmpULT(p.value, p2.value)
                       );

    setPr(pr, p2.block, type_bool, vres);

    p.block = p2.block;
    ParseResult temp;
    if (hasRelevantDestructor(&p)) {
        destructIfApplicable(&p, NULL, &temp);
        p2.block = temp.block;
    }
    if (hasRelevantDestructor(&p2)) {
        destructIfApplicable(&p2, NULL, &temp);
        pr->block = temp.block;
    }

    return true;
}

bool Generator::parsePtrMoreThan(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        bool getAddress,
        bool prefixed_with_core,
        ParseResult *pr)
{
    if (!assertArgNums("p>", n, 2, 2)) {
        return NULL;
    }

    symlist *lst = n->list;

    ParseResult p;
    bool res = parseFunctionBodyInstr(
                    dfn, block, (*lst)[1], getAddress, NULL, &p
               );
    if (!res) {
        return false;
    }
    if (!assertIsPointerType("p>", (*lst)[1], p.type, "1")) {
        return false;
    }

    ParseResult p2;
    res = parseFunctionBodyInstr(
               dfn, p.block, (*lst)[2], getAddress, NULL, &p2
          );
    if (!res) {
        return false;
    }
    if (!assertIsPointerType("p>", (*lst)[2], p2.type, "2")) {
        return false;
    }

    llvm::IRBuilder<> builder(p2.block);
    llvm::Value *vres = llvm::cast<llvm::Value>(
                           builder.CreateICmpUGT(p.value, p2.value)
                       );

    setPr(pr, p2.block, type_bool, vres);

    p.block = p2.block;
    ParseResult temp;
    if (hasRelevantDestructor(&p)) {
        destructIfApplicable(&p, NULL, &temp);
        p2.block = temp.block;
    }
    if (hasRelevantDestructor(&p2)) {
        destructIfApplicable(&p2, NULL, &temp);
        pr->block = temp.block;
    }

    return true;
}

bool Generator::parseNullPtr(Element::Function *dfn,
                             llvm::BasicBlock *block,
                             Node *n,
                             bool getAddress,
                             bool prefixed_with_core,
                             ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("nullptr", n, 1, 1)) {
        return false;
    }

    symlist *lst = n->list;

    Element::Type *type = parseType((*lst)[1], false, false);
    if (!type) {
        return false;
    }
    /* Create a pointer to the provided type. */
    Element::Type *ptype = tr->getPointerType(type);

    llvm::IRBuilder<> builder(block);
    llvm::Type *llvm_ptype =
        toLLVMType(ptype, NULL, false);
    if (!llvm_ptype) {
        failedDaleToLLVMTypeConversion(ptype);
        return false;
    }

    llvm::Value *res =
        builder.CreateAlloca(llvm_ptype);

    llvm::Value *val =
        builder.CreateIntToPtr(
            llvm_native_zero,
            llvm_ptype
        );

    builder.CreateStore(val, res);
    setPr(pr, block, ptype, val);

    return true;
}

bool Generator::parseNull(Element::Function *dfn,
                          llvm::BasicBlock *block,
                          Node *n,
                          bool getAddress,
                          bool prefixed_with_core,
                          ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("null", n, 1, 1)) {
        return false;
    }

    symlist *lst = n->list;

    /* Take the second value, parseMacroCall, see if it's a list,
     * if it is a list and the form is : or $, then show an error
     * about that value never being null. */

    Node *arg = (*lst)[1];
    if (arg->is_list) {
        arg = parseOptionalMacroCall(arg);
        if (!arg) {
            return false;
        }
        if (arg->is_list) {
            Node *first = arg->list->at(0);
            if (first->is_token) {
                const char *v = first->token->str_value.c_str();
                if ((!strcmp(v, ":")) || (!strcmp(v, "$"))) {
                    Error *e = new Error(
                        ErrorInst::Generator::ValueWillNeverBeNull,
                        arg
                    );
                    erep->addError(e);
                    return false;
                }
            }
        }
    }

    ParseResult pr_value;
    bool res = parseFunctionBodyInstr(dfn, block, arg, false, NULL, &pr_value);

    if (!res) {
        return false;
    }
    if (!assertIsPointerType("null", arg, pr_value.type, "1")) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_value.block);
    llvm::Value *vres =
        builder.CreatePtrToInt(pr_value.value, nt->getNativeIntType());

    llvm::Value *icmpres = llvm::cast<llvm::Value>(
                               builder.CreateICmpEQ(vres,
                                       llvm_native_zero)
                           );
    
    setPr(pr, pr_value.block, type_bool, icmpres);
    if (hasRelevantDestructor(&pr_value)) {
        ParseResult temp;
        bool res = destructIfApplicable(&pr_value, NULL, &temp);
        if (!res) { 
            return false;
        }
        pr->block = temp.block;
    }

    return true;
}

bool Generator::parseUsingNamespace(Element::Function *dfn,
                                    llvm::BasicBlock *block,
                                    Node *n,
                                    bool getAddress,
                                    bool prefixed_with_core,
                                    ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("using-namespace", n, 1, -1)) {
        return false;
    }

    symlist *lst = n->list;
    Node *n2 = (*lst)[1];
    n2 = parseOptionalMacroCall(n2);
    if (!n2) {
        return false;
    }
    if (!assertArgIsAtom("using-namespace", n2, "1")) {
        return false;
    }
    if (!assertAtomIsSymbol("using-namespace", n2, "1")) {
        return false;
    }

    Token *t = n2->token;

    bool res = ctx->useNamespace(t->str_value.c_str());
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::NamespaceNotInScope,
            n,
            t->str_value.c_str()
        );
        erep->addError(e);
        return false;
    }

    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;
    ++node_iter;

    pr->block = block;
    while (node_iter != lst->end()) {
        bool res = parseFunctionBodyInstr(
                       dfn, pr->block, (*node_iter), getAddress, NULL, pr
                   );
        if (!res) {
            return false;
        }
        ++node_iter;
    }

    ctx->unuseNamespace();

    return true;
}

bool Generator::parseArrayOf(Element::Function *dfn,
                            llvm::BasicBlock *block,
                            Node *n,
                            bool getAddress,
                            bool prefixed_with_core,
                            ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    symlist *lst = n->list;

    if (!assertArgNums("array-of", n, 3, -1)) {
        return false;
    }

    Node *newnum = parseOptionalMacroCall((*lst)[1]);
    if (!newnum) {
        return false;
    }

    int size = parseInteger(newnum);
    if (size == -1) {
        return false;
    }

    Element::Type *type = parseType((*lst)[2], false, false);
    if (!type) {
        return false;
    }

    Element::Type *arrtype = tr->getArrayType(type, size);

    int size2;
    bool res = 
        parseArrayLiteral(
            dfn,
            block,
            ((*lst)[3]),
            "array literal",
            arrtype,
            getAddress,
            &size2,
            pr
        );

    return res;
}

bool Generator::hasRelevantDestructor(ParseResult *pr)
{
    if (pr->do_not_destruct) {
        return false;
    }
    std::vector<Element::Type *> types;
    types.push_back(tr->getPointerType(pr->type));
    Element::Function *fn = ctx->getFunction("destroy", &types,
                            NULL, 0);
    if (!fn) {
        return false;
    }
    return true;
}

bool Generator::destructIfApplicable(ParseResult *pr,
        llvm::IRBuilder<> *builder,
        ParseResult *pr_ret)
{
    pr->copyTo(pr_ret);

    if (pr->do_not_destruct) {
        return true;
    }

    if (DALE_DEBUG) {
        std::string mytype;
        pr->type->toStringProper(&mytype);
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
        Element::Type *mine =
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

        std::vector<Element::Type *> types;
        if (!mine->is_array) {
            types.push_back(tr->getPointerType(mine));
            Element::Function *fn = ctx->getFunction("destroy", &types,
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
                            toLLVMType(pr->type, NULL, false))
                        );
                builder->CreateStore(pr->value, new_ptr2);
                actual_value = new_ptr2;
            } else {
                llvm::IRBuilder<> builder(mbl);
                llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                        builder.CreateAlloca(
                            toLLVMType(pr->type, NULL, false))
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
            stl::push_back2(&indices,
                            llvm_native_zero,
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

    std::vector<Element::Type *> types;
    types.push_back(tr->getPointerType(pr->type));
    Element::Function *fn = ctx->getFunction("destroy", &types,
                            NULL, 0);
    if (!fn) {
        return true;
    }
    int destroy_builder = 0;
    if (!builder) {
        destroy_builder = 1;
        builder = new llvm::IRBuilder<>(pr->block);
    }
    std::vector<llvm::Value *> call_args;
    llvm::Value *new_ptr2 = 
        llvm::cast<llvm::Value>(
            builder->CreateAlloca(toLLVMType(pr->type, NULL, false))
        );
    builder->CreateStore(pr->value, new_ptr2);
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
    Element::Function *dfn,
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
    std::vector<Element::Type *> types;
    Element::Type *copy_type = tr->getPointerType(pr->type);
    types.push_back(copy_type);
    types.push_back(copy_type);
    Element::Function *over_setf =
        ctx->getFunction("setf-copy", &types, NULL, 0);
    if (!over_setf) {
        return true;
    }
    llvm::IRBuilder<> builder(pr->block);
    llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                builder.CreateAlloca(toLLVMType(pr->type, NULL,
                                        false))
                            );
    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                builder.CreateAlloca(toLLVMType(pr->type, NULL,
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

    setPr(pr_res, pr->block, pr->type, result);
    pr_res->freshly_copied = 1;

    return true;
}

/* todo: this does part of the work of destructIfApplicable. It
 * should call that function instead. */
bool Generator::scopeClose(Element::Function *dfn,
                          llvm::BasicBlock *block,
                          llvm::Value *no_destruct)
{
    /* Get variables in the current scope. */
    std::vector<Element::Variable *> stack_vars;
    ctx->ns()->getVariables(&stack_vars);

    /* For each variable, look for an associated destructor.
     * (todo: cache the results here so you don't call getFunction
     * unnecessarily.) */
    llvm::IRBuilder<> builder(block);
    std::vector<Element::Type *> types;
    for (std::vector<Element::Variable *>::iterator
            b = stack_vars.begin(),
            e = stack_vars.end();
            b != e;
            ++b) {

        if (no_destruct && ((*b)->value == no_destruct)) {
            continue;
        }
        /* If it's an array type, destruct it normally. */
        if ((*b)->type->array_type && (*b)->type->array_size) {
            ParseResult mnew;
            mnew.type = (*b)->type;
            mnew.value = (*b)->value;
            mnew.block = block;
            ParseResult temp;
            destructIfApplicable(&mnew, NULL, &temp);
        } else {
            types.clear();
            Element::Type *pbtype = tr->getPointerType((*b)->type);
            types.push_back(pbtype);
            Element::Function *fn = ctx->getFunction("destroy",
                                    &types, NULL, 0);
            if (!fn) {
                continue;
            }

            if (!((*b)->value->getType()->isPointerTy())) {
                fprintf(stderr,
                        "Variable value is not a pointer! (%s)\n",
                        (*b)->name.c_str());
                abort();
            }

            std::vector<llvm::Value *> call_args;
            // Destroy takes a pointer now, instead of a value.
            call_args.push_back((*b)->value);
            builder.CreateCall(
                fn->llvm_function,
                llvm::ArrayRef<llvm::Value*>(call_args));
        }
    }

    return true;
}

bool Generator::parseNewScope(Element::Function *dfn,
                                      llvm::BasicBlock *block,
                                      Node *n,
                                      bool getAddress,
                                      bool prefixed_with_core,
                                      ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    symlist *lst = n->list;

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;

    pr->block = block;
    bool success = true;
    while (node_iter != lst->end()) {
        ParseResult local_pr;
        bool res = parseFunctionBodyInstr(
                       dfn, pr->block, (*node_iter), getAddress, NULL, 
                       &local_pr
                   );
        ++node_iter;
        if (!res) {
            success = false;
            continue;
        }
       
        if (node_iter != lst->end()) {
            ParseResult pr_value;
            bool res = destructIfApplicable(&local_pr, NULL, &pr_value);
            if (!res) {
                return false;
            }
            pr->block = pr_value.block;
        } else {
            local_pr.copyTo(pr);
        }
    }

    scopeClose(dfn, block, NULL);
    ctx->deactivateNamespace(anon_name.c_str());

    return success;
}

bool Generator::typeToString(DNode *dnode, char *buf)
{
    Node *n = WrapNode(DNodeToIntNode(dnode));

    Element::Type *type = parseType((*(n->list))[0], false,
                                    false);
    if (!type) {
        return false;
    }

    std::string temp;
    type->toEncStr(&temp);
    if (temp.size() > 255) {
        fprintf(stderr, "Internal error: encoded type "
                "string is too long (>255 "
                "characters): %s.\n",
                temp.c_str());
    }
    std::copy(temp.begin(), temp.end(), buf);
    buf[temp.size()] = '\0';

    return true;
}

bool Generator::typeToDisplayString(DNode *dnode, char *buf)
{
    Node *n = WrapNode(DNodeToIntNode(dnode));

    Element::Type *type = parseType((*(n->list))[0], false,
                                    false);
    if (!type) {
        return false;
    }

    std::string temp;
    type->toStringProper(&temp);
    std::copy(temp.begin(), temp.end(), buf);
    buf[temp.size()] = '\0';

    return true;
}

void Generator::popErrors(int original_count)
{
    int diff = erep->getErrorTypeCount(ErrorType::Error)
               - original_count;

    while (diff--) {
        erep->popLastError();
    }
}

bool Generator::existsType(DNode *dnode)
{
    Node *n = WrapNode(DNodeToIntNode(dnode));
    assert(n->list && "must receive a list!");

    if (!assertArgNums("exists-type", n, 0, 0)) {
        printf("wrong arg count! (existsType)\n");
        return false;
    }

    symlist *lst = n->list;

    int error_count = erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *type = parseType((*lst)[0], false,
                                    false);
    popErrors(error_count);

    return !!type;
}

void Generator::reportError(DNode *dnode, char *str)
{
    Node *n = DNodeToIntNode(dnode);

    Error *e = new Error(
        ErrorInst::Generator::ExternalError, n, str
    );
    erep->addError(e);
    return;
}

Node *Generator::WrapNode(Node *n)
{
    std::vector<Node *> *nodes = new std::vector<Node *>;
    nodes->push_back(n);
    Node *holder = new Node(nodes);
    return holder;
}

bool Generator::existsVariable(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!n->token) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Variable *thevar =
        ctx->getVariable(n->token->str_value.c_str());

    popErrors(original_error_count);
    return ((thevar) ? true : false);
}

bool Generator::isCharType(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!n->token) {
        return false;
    }
    if (n->token->str_value.compare("char")) {
        return false;
    }
    return true;
}

bool Generator::isIntegerType(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype = parseType(n, false, false);

    popErrors(original_error_count);
    if (!thetype) {
        return false;
    }
    return (thetype->isIntegerType());
}

bool Generator::typesEqual(DNode *T1, DNode *T2)
{
    Node *n = DNodeToIntNode(T1);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    Node *n2 = DNodeToIntNode(T2);
    n2 = parseOptionalMacroCall(n2);
    if (!n2) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype  = parseType(n,  false, false);
    Element::Type *thetype2 = parseType(n2, false, false);

    popErrors(original_error_count);
    if (!thetype || !thetype2) {
        return false;
    }
    return (thetype->isEqualTo(thetype2));
}

bool Generator::isSignedIntegerType(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype = parseType(n, false, false);

    popErrors(original_error_count);
    if (!thetype) {
        return false;
    }
    return (thetype->isSignedIntegerType());
}

bool Generator::isUnsignedIntegerType(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype = parseType(n, false, false);

    popErrors(original_error_count);
    if (!thetype) {
        return false;
    }
    return (thetype->isIntegerType()
            && !(thetype->isSignedIntegerType()));
}

bool Generator::isFloatingPointType(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype = parseType(n, false, false);

    popErrors(original_error_count);
    if (!thetype) {
        return false;
    }
    return (thetype->isFloatingPointType());
}

bool Generator::isPointerType(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype = parseType(n, false, false);

    popErrors(original_error_count);
    if (!thetype) {
        return false;
    }
    return (thetype->points_to ? true : false);
}

bool Generator::isPointerToType(DNode *dnode, DNode *pointee_type)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    Node *n2 = DNodeToIntNode(pointee_type);
    n2 = parseOptionalMacroCall(n2);
    if (!n2) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype  = parseType(n,  false, false);
    Element::Type *thetype2 = parseType(n2, false, false);

    popErrors(original_error_count);
    if (!thetype || !thetype2) {
        return false;
    }
    return (thetype->points_to->isEqualTo(thetype2));
}

DNode *Generator::pointeeType(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    n = parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *thetype = parseType(n, false, false);

    popErrors(original_error_count);
    if (!thetype) {
        return false;
    }
    return
        IntNodeToDNode(thetype->points_to->toNode());
}

/* todo - pushing here to prevent destruction, so as not to ruin
 * mynode - there has to be a better way of dealing with this
 * problem. (todo 2: comment doesn't make sense.) */
std::vector<ParseResult*> g_parse_results;
std::vector<Node *> g_nodes;

DNode *Generator::typeOf(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    g_nodes.push_back(n);

    if (!global_function) {
        fprintf(stderr, "No global function defined (typeOf).\n");
        abort();
    }
    if (!global_block) {
        fprintf(stderr, "No global block defined (typeOf).\n");
        abort();
    }

    ParseResult *p = new ParseResult();
    bool res =
        parseFunctionBodyInstr(
            global_function,
            global_block,
            n,
            false,
            NULL,
            p
        );

    if (!res) {
        return NULL;
    }

    g_parse_results.push_back(p);

    DNode *mynode = IntNodeToDNode(p->type->toNode());
    return mynode;
}

const char *Generator::printfLength(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);
    if (!n->is_token) {
        return "";
    }
    std::string *str = &(n->token->str_value);

    if (!(str->compare("char"))) {
        return "hh";
    }
    if (!(str->compare("size"))) {
        return "z";
    }
    if (!(str->compare("ptrdiff"))) {
        return "t";
    }
    if (!(str->compare("long-double"))) {
        return "L";
    }
    /* The type token now should be an int or uint, followed by a
     * number. Confirm that this is so, get the number, divide it
     * by four and return the appropriate length character. */
    if (!(str->find("int"))) {
        str->erase(str->begin(), str->begin() + 3);
    } else if (!(str->find("uint"))) {
        str->erase(str->begin(), str->begin() + 4);
    } else {
        return "";
    }

    unsigned long addnum =
        strtoul(str->c_str(), NULL, DECIMAL_RADIX);

    if (STRTOUL_FAILED(addnum, str->c_str(), NULL)) {
        return "";
    }
    addnum /= 8;

    if (addnum == sizeof(short)) {
        return "h";
    }
    if (addnum == sizeof(long)) {
        return "l";
    }
    if (addnum >= sizeof(long long)) {
        return "ll";
    }
    return "";
}

int Generator::makeTemporaryGlobalFunction(
    std::vector<DeferredGoto*> *dgs,
    std::map<std::string, Element::Label*> *mls
)
{
    /* Create a temporary function for evaluating the arguments. */

    llvm::Type *llvm_return_type =
        toLLVMType(type_int, NULL, false);
    if (!llvm_return_type) {
        failedDaleToLLVMTypeConversion(type_int);
        return 0;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[255];
    sprintf(buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        mod->getOrInsertFunction(new_name.c_str(), ft);
    if (!fnc) {
        fprintf(stderr, "Internal error: unable to add "
                "function ('%s') to module.\n",
                new_name.c_str());
        abort();
    }

    llvm::Function *fn =
        llvm::dyn_cast<llvm::Function>(fnc);
    if (!fn) {
        fprintf(stderr, "Internal error: unable to convert "
                "function constant to function "
                "for function '%s'.\n",
                new_name.c_str());
        abort();
    }

    std::vector<Element::Variable *> vars;

    Element::Function *dfn =
        new Element::Function(type_int,
                              &vars,
                              fn,
                              0,
                              new std::string(new_name),
                              0);
    dfn->linkage = Linkage::Intern;
    if (!dfn) {
        fprintf(stderr, "Internal error: unable to create new "
                "function (!) '%s'.\n",
                new_name.c_str());
        abort();
    }

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "entry",
                                 fn);

    int error_count = erep->getErrorTypeCount(ErrorType::Error);

    global_functions.push_back(dfn);
    global_function = dfn;

    global_blocks.push_back(block);
    global_block = block;

    ctx->activateAnonymousNamespace();

    return error_count;
}

void Generator::removeTemporaryGlobalFunction(
    int error_count,
    std::vector<DeferredGoto*> *dgs,
    std::map<std::string, Element::Label*> *mls
)
{
    if (error_count >= 0) {
        popErrors(error_count);
    }

    ctx->deactivateAnonymousNamespace();
    Element::Function *current = global_function;

    global_functions.pop_back();
    if (global_functions.size()) {
        global_function = global_functions.back();
    } else {
        global_function = NULL;
    }

    global_blocks.pop_back();
    if (global_blocks.size()) {
        global_block = global_blocks.back();
    } else {
        global_block = NULL;
    }

    /* Remove the temporary function. */
    current->llvm_function->eraseFromParent();

    return;
}

bool Generator::hasErrors(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    assert(n->list && "must receive a list!");

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    int made_temp = 0;
    std::vector<DeferredGoto*> dgs;
    std::map<std::string, Element::Label*> mls;
    if (!global_function) {
        makeTemporaryGlobalFunction(&dgs, &mls);
        made_temp = 1;
    }

    ParseResult temp;

    /* POMC may succeed, but the underlying macro may return a
     * null DNode pointer. This is not necessarily an error. */
    n = parseOptionalMacroCall(n);
    if (n) {
        parseFunctionBodyInstr(
            global_function,
            global_block,
            n,
            false,
            NULL,
            &temp
        );
    }

    if (made_temp) {
        removeTemporaryGlobalFunction(-1, &dgs, &mls);
    }

    int new_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    bool has_errors =
        ((new_error_count - original_error_count) != 0);

    popErrors(original_error_count);
    return has_errors;
}

bool Generator::mustInit(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *ptype = parseType(n, false, false);
    if (!ptype) {
        popErrors(original_error_count);
        return 0;
    }

    if (ptype->is_const) {
        return true;
    }
    if (ptype->struct_name) {
        Element::Struct *structp =
            ctx->getStruct(
                ptype->struct_name->c_str(),
                ptype->namespaces
            );
        return !!(structp->must_init);
    }
    return false;
}

bool Generator::isConst(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *ptype = parseType(n, false, false);
    if (!ptype) {
        popErrors(original_error_count);
        return 0;
    }

    if (ptype->is_const) {
        return true;
    }

    return false;
}

/* Function name lists will be stored here by fnByArgsCount, keyed on
 * the stringification of of the parameter types. This map will, in
 * turn, be used by fnByArgsName. */
std::map<std::string, std::vector<std::string>*> fn_by_args;

int Generator::fnByArgsCount(DNode *dnode, const char *prefix)
{
    Node *n = DNodeToIntNode(dnode);

    assert(n->list && "must receive a list!");

    if (!assertArgNums("fn-by-args-count", n, 0, -1)) {
        return 0;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    symlist *lst = n->list;

    std::vector<Node *>::iterator iter
    = lst->begin();
    int c = 0;

    std::string map_key;
    std::vector<Element::Type *> parameter_types;
    while (iter != lst->end()) {
        Element::Type *ptype = parseType((*iter), false,
                                         false);
        if (!ptype) {
            popErrors(original_error_count);
            return 0;
        }
        ptype->toStringProper(&map_key);
        parameter_types.push_back(ptype);
        ++iter;
        ++c;
    }

    std::map<std::string, std::vector<std::string>*>::iterator
        b = fn_by_args.find(map_key),
        e = fn_by_args.end();
    if (b != e) {
        std::vector<std::string>* fn_by_args_list = b->second;
        delete fn_by_args_list;
        fn_by_args.erase(b);
    }

    /* For each function that exists, see if it has an instance
     * for the provided parameter types. */

    std::set<std::string> function_names;
    bool has_prefix = prefix;
    if (!prefix) {
        prefix = "";
    }
    std::string ss_prefix(prefix);
    ctx->getFunctionNames(&function_names, (has_prefix ? &ss_prefix : NULL));
    std::vector<std::string> *fn_by_args_list = 
        new std::vector<std::string>;

    for (std::set<std::string>::iterator
            b = function_names.begin(),
            e = function_names.end();
            b != e;
            ++b) {
        Element::Function *thefn =
            ctx->getFunction(b->c_str(),
                             &parameter_types,
                             NULL,
                             0);
        if (thefn && (!(thefn->is_macro))) {
            fn_by_args_list->push_back((*b));
        }
    }

    fn_by_args.insert(
        std::pair<std::string, std::vector<std::string>*>(
            map_key,
            fn_by_args_list
        )
    );

    return (int) fn_by_args_list->size();
}

const char *Generator::fnByArgsName(DNode *dnode, int acount)
{
    Node *n = DNodeToIntNode(dnode);

    assert(n->list && "must receive a list!");

    if (!assertArgNums("fn-by-args-name", n, 0, -1)) {
        return 0;
    }

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    symlist *lst = n->list;

    std::vector<Node *>::iterator iter = lst->begin();

    std::string map_key;
    while (iter != lst->end()) {
        Element::Type *ptype = parseType((*iter), false,
                                         false);
        if (!ptype) {
            popErrors(original_error_count);
            return 0;
        }
        ptype->toStringProper(&map_key);
        ++iter;
    }

    std::map<std::string, std::vector<std::string>*>::iterator
        b = fn_by_args.find(map_key),
        e = fn_by_args.end();

    if (b == e) {
        return NULL;
    }

    std::vector<std::string> *fn_by_args_list = b->second;
    if (((unsigned) acount) > (fn_by_args_list->size() - 1)) {
        return NULL;
    }

    return fn_by_args_list->at(acount).c_str();
}

bool Generator::existsFunction(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    assert(n->list && "must receive a list!");

    if (!assertArgNums("exists-fn", n, 1, -1)) {
        return false;
    }

    symlist *lst = n->list;

    Node *nret_type = (*lst)[0];
    Node *nfn_name  = (*lst)[1];

    std::vector<Node *>::iterator iter
    = lst->begin();
    ++iter;
    ++iter;

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Element::Type *ret_type = parseType(nret_type, false,
                                        false);
    if (!ret_type) {
        popErrors(original_error_count);
        return false;
    }

    int c = 0;

    std::vector<Element::Type *> parameter_types;
    while (iter != lst->end()) {
        Element::Type *ptype = parseType((*iter), false,
                                         false);
        if (!ptype) {
            popErrors(original_error_count);
            return false;
        }
        if (ptype->base_type == Type::Void) {
            break;
        }
        parameter_types.push_back(ptype);
        ++iter;
        ++c;
    }

    Element::Function *thefn =
        ctx->getFunction(nfn_name->token->str_value.c_str(),
                         &parameter_types,
                         NULL,
                         0);

    popErrors(original_error_count);
    return (thefn && !thefn->is_macro);
}

DNode *Generator::inputType(DNode *fn_name_nd, int arg_count)
{
    Node *fn_node = DNodeToIntNode(fn_name_nd);
    fn_node = parseOptionalMacroCall(fn_node);
    if (!fn_node) {
        return NULL;
    }
    const char *fn_name = fn_node->token->str_value.c_str();

    Element::Function *fn = ctx->getFunction(fn_name, NULL,
                            NULL, 0);
    if (!fn) {
        return NULL;
    }
    if (fn->linkage != Linkage::Extern_C) {
        return NULL;
    }
    if ((int) (fn->parameter_types->size() - 1) < arg_count) {
        return NULL;
    }

    return
        IntNodeToDNode(
            fn->parameter_types->at(arg_count)->type->toNode()
        );
}

int Generator::arity(DNode *fn_name_nd)
{
    Node *fn_node = DNodeToIntNode(fn_name_nd);
    fn_node = parseOptionalMacroCall(fn_node);
    if (!fn_node) {
        return -1;
    }
    const char *fn_name = fn_node->token->str_value.c_str();

    Element::Function *fn = ctx->getFunction(fn_name, NULL, NULL,
                            0);
    if (!fn) {
        return -1;
    }
    if (fn->linkage != Linkage::Extern_C) {
        return -1;
    }
    return fn->numberOfRequiredArgs();
}

int Generator::structMemberCount(DNode *struct_name_node)
{
    Node *s_node = DNodeToIntNode(struct_name_node);
    s_node = parseOptionalMacroCall(s_node);
    if (!s_node || !s_node->token) {
        return -1;
    }
    const char *struct_name = s_node->token->str_value.c_str();

    Element::Struct *st = ctx->getStruct(struct_name);
    if (!st) {
        return -1;
    }
    return st->element_types.size();
}

DNode *Generator::structMemberType(DNode *struct_name_node, int arg_count)
{
    Node *s_node = DNodeToIntNode(struct_name_node);
    s_node = parseOptionalMacroCall(s_node);
    if (!s_node || !s_node->token) {
        return NULL;
    }
    const char *struct_name = s_node->token->str_value.c_str();

    Element::Struct *st = ctx->getStruct(struct_name);
    if (!st) {
        return NULL;
    }
    if ((int) st->element_types.size() < arg_count) {
        return NULL;
    }
    return
        IntNodeToDNode(
            st->element_types.at(arg_count)->toNode()
        );
}

const char *Generator::structMemberName(DNode *struct_name_node, int arg_count)
{
    Node *s_node = DNodeToIntNode(struct_name_node);
    s_node = parseOptionalMacroCall(s_node);
    if (!s_node || !s_node->token) {
        return NULL;
    }
    const char *struct_name = s_node->token->str_value.c_str();

    Element::Struct *st = ctx->getStruct(struct_name);
    if (!st) {
        return NULL;
    }
    if ((int) st->element_types.size() < arg_count) {
        return NULL;
    }
    return st->indexToName(arg_count);
}

DNode *Generator::codomain(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    assert(n->list && "must receive a list!");

    if (!assertArgNums("codomain", n, 0, -1)) {
        return NULL;
    }

    symlist *lst = n->list;

    Node *nfn_name  = (*lst)[0];

    std::vector<Node *>::iterator iter
    = lst->begin();
    ++iter;

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    int c = 0;

    std::vector<Element::Type *> parameter_types;
    while (iter != lst->end()) {
        Element::Type *ptype = parseType((*iter), false,
                                         false);
        if (!ptype) {
            popErrors(original_error_count);
            return NULL;
        }
        if (ptype->base_type == Type::Void) {
            break;
        }
        parameter_types.push_back(ptype);
        ++iter;
        ++c;
    }

    Element::Function *thefn =
        ctx->getFunction(nfn_name->token->str_value.c_str(),
                         &parameter_types,
                         NULL,
                         0);

    popErrors(original_error_count);
    if (thefn && !thefn->is_macro) {
        DNode *dn =
            IntNodeToDNode(
                thefn->return_type->toNode()
            );
        return dn;
    }
    else {
        return NULL;
    }
}

bool Generator::existsMacro(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    assert(n->list && "must receive a list!");

    if (!assertArgNums("exists-macro", n, 0, -1)) {
        return false;
    }

    symlist *lst = n->list;

    Node *nfn_name  = (*lst)[0];

    std::vector<Node *>::iterator iter
    = lst->begin();
    ++iter;

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    int c = 0;

    std::vector<Element::Type *> parameter_types;
    while (iter != lst->end()) {
        Element::Type *ptype = parseType((*iter), false,
                                         false);
        if (!ptype) {
            popErrors(original_error_count);
            return false;
        }
        if (ptype->base_type == Type::Void) {
            break;
        }
        parameter_types.push_back(ptype);
        ++iter;
        ++c;
    }

    Element::Function *thefn =
        ctx->getFunction(nfn_name->token->str_value.c_str(),
                         &parameter_types,
                         NULL,
                         1);

    popErrors(original_error_count);
    return (thefn && thefn->is_macro);
}

/* Exact means exact, as in, this includes the implicit starting
 * arguments. You probably want existsMacro. */
bool Generator::existsMacroExact(DNode *dnode)
{
    Node *n = DNodeToIntNode(dnode);

    assert(n->list && "must receive a list!");

    if (!assertArgNums("exists-macro-exact", n, 1, -1)) {
        return false;
    }

    symlist *lst = n->list;

    Node *nfn_name  = (*lst)[0];

    std::vector<Node *>::iterator iter
    = lst->begin();
    ++iter;

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    int c = 0;

    std::vector<Element::Type *> parameter_types;
    while (iter != lst->end()) {
        Element::Type *ptype = parseType((*iter), false,
                                         false);
        if (!ptype) {
            popErrors(original_error_count);
            return false;
        }
        if (ptype->base_type == Type::Void) {
            break;
        }
        parameter_types.push_back(ptype);
        ++iter;
        ++c;
    }

    Element::Function *thefn =
        ctx->getFunction(nfn_name->token->str_value.c_str(),
                         &parameter_types,
                         NULL,
                         1);

    popErrors(original_error_count);

    if (!thefn || !(thefn->is_macro)) {
        return false;
    }

    std::vector<Element::Type *> types;
    for (std::vector<Element::Variable *>::iterator
            b = thefn->parameter_types->begin(),
            e = thefn->parameter_types->end();
            b != e;
            ++b) {
        types.push_back((*b)->type);
    }

    if (!dale::stl::isEqualTo(&types, &parameter_types)) {
        return false;
    }

    return true;
}

bool Generator::parseDo(Element::Function *dfn,
                        llvm::BasicBlock *block,
                        Node *n,
                        bool getAddress,
                        bool prefixed_with_core,
                        ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("do", n, 1, -1)) {
        return false;
    }

    symlist *lst = n->list;
    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;

    pr->block = block;
    while (node_iter != lst->end()) {
        ParseResult local_pr;
        bool res = parseFunctionBodyInstr(
                       dfn, pr->block, (*node_iter), getAddress, NULL,
                       &local_pr
                   );
        if (!res) {
            return false;
        }
        ++node_iter;

        if (node_iter != lst->end()) {
            ParseResult pr_value;
            bool res = destructIfApplicable(&local_pr, NULL, &pr_value);
            if (!res) {
                return false;
            }
            pr->block = pr_value.block;
            pr_value.copyTo(pr);
        } else {
            local_pr.copyTo(pr);
        }
    }

    return true;
}

int mystructs = 0;
bool Generator::getAlignmentofType(llvm::BasicBlock *block,
        Element::Type *type,
        ParseResult *pr)
{
    std::vector<llvm::Type*> elements_llvm;
    elements_llvm.push_back(toLLVMType(type_char, NULL, false));

    llvm::Type *llvm_type =
        toLLVMType(type, NULL, false);
    if (!llvm_type) {
        failedDaleToLLVMTypeConversion(type);
        return false;
    }
    elements_llvm.push_back(llvm_type);

    llvm::StructType *llvm_new_struct =
        llvm::StructType::get(llvm::getGlobalContext(),
                              elements_llvm,
                              false);

    char buf[100];
    sprintf(buf, "__mystruct%d", mystructs++);
    std::string name2;
    name2.append("struct_");
    std::string name3;
    ctx->ns()->nameToSymbol(buf, &name3);
    name2.append(name3);

    llvm_new_struct->setName(name2.c_str());
    if (llvm_new_struct->getName() != llvm::StringRef(name2)) {
        fprintf(stderr, "Internal error: unable to add struct.");
        abort();
    }

    llvm::IRBuilder<> builder(block);

    llvm::PointerType *lpt =
        llvm::PointerType::getUnqual(llvm_new_struct);

    llvm::Value *res =
        builder.CreateGEP(
            llvm::ConstantPointerNull::get(lpt),
            llvm::ArrayRef<llvm::Value*>(
                two_zero_indices
            ));

    llvm::Value *res2 =
        builder.CreatePtrToInt(res, nt->getNativeSizeType());

    setPr(pr, block, type_size, res2);
    return true;
}

bool Generator::getOffsetofType(llvm::BasicBlock *block,
                                Element::Type *type,
                                const char *field_name,
                                int index,
                                ParseResult *pr)
{
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type =
        toLLVMType(type, NULL, false);
    if (!llvm_type) {
        failedDaleToLLVMTypeConversion(type);
        return false;
    }

    llvm::PointerType *lpt =
        llvm::PointerType::getUnqual(llvm_type);

    int myindex;
    if (index != -1) {
        myindex = index;
    } else {
        Element::Struct *structp =
            ctx->getStruct(
                type->struct_name->c_str(),
                type->namespaces
            );

        if (!structp) {
            fprintf(stderr, "Internal error: invalid struct name.\n");
            abort();
        }

        myindex = structp->nameToIndex(field_name);
        if (myindex == -1) {
            fprintf(stderr, "Internal error: invalid struct field"
                    "name.\n");
            abort();
        }
    }

    std::vector<llvm::Value *> indices;
    stl::push_back2(&indices,  llvm_native_zero,
                    getNativeInt(myindex));

    llvm::Value *res =
        builder.CreateGEP(
            llvm::ConstantPointerNull::get(lpt),
            llvm::ArrayRef<llvm::Value *>(indices));

    llvm::Value *res2 =
        builder.CreatePtrToInt(res, nt->getNativeSizeType());

    setPr(pr, block, type_size, res2);
    return true;
}

bool Generator::getSizeofType(llvm::BasicBlock *block,
                              Element::Type *type,
                              ParseResult *pr)
{
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type =
        toLLVMType(type, NULL, false);
    if (!llvm_type) {
        failedDaleToLLVMTypeConversion(type);
        return false;
    }

    llvm::PointerType *lpt =
        llvm::PointerType::getUnqual(llvm_type);

    llvm::Value *res =
        builder.CreateGEP(
            llvm::ConstantPointerNull::get(lpt),
            llvm::ConstantInt::get(nt->getNativeIntType(), 1)
        );

    llvm::Value *res2 =
        builder.CreatePtrToInt(res, nt->getNativeSizeType());

    setPr(pr, block, type_size, res2);
    return true;
}

size_t Generator::getOffsetofTypeImmediate(Element::Type *type,
        const char *field_name,
        int index)
{
    llvm::Type *llvm_return_type =
        toLLVMType(type_size, NULL, false);
    if (!llvm_return_type) {
        failedDaleToLLVMTypeConversion(type_size);
        return 0;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[255];
    sprintf(buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        mod->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    std::vector<Element::Variable*> args;

    llvm::Function *fn = llvm::cast<llvm::Function>(fnc);

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", fn);
    ParseResult mine;
    bool mres = getOffsetofType(block, type, field_name, index, &mine);
    if (!mres) {
        return 0;
    }

    llvm::IRBuilder<> builder(block);
    builder.CreateRet(mine.value);

    void* fptr =
        ee->getPointerToFunction(fn);
    if (!fptr) {
        fprintf(stderr,
                "Internal error: could not get pointer "
                "to function for literal.\n");
        abort();
    }

    size_t (*FPTR)(void) = (size_t (*)(void))fptr;

    size_t res = (size_t) FPTR();

    fn->eraseFromParent();

    return res;
}

size_t Generator::getSizeofTypeImmediate(Element::Type *type)
{
    llvm::Type *llvm_return_type =
        toLLVMType(type_size, NULL, false);
    if (!llvm_return_type) {
        failedDaleToLLVMTypeConversion(type_size);
        return 0;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[255];
    sprintf(buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        mod->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    std::vector<Element::Variable*> args;

    llvm::Function *fn = llvm::cast<llvm::Function>(fnc);

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", fn);
    ParseResult mine;
    bool mres = getSizeofType(block, type, &mine);
    if (!mres) {
        return 0;
    }

    llvm::IRBuilder<> builder(block);
    builder.CreateRet(mine.value);

    void* fptr =
        ee->getPointerToFunction(fn);
    if (!fptr) {
        fprintf(stderr,
                "Internal error: could not get pointer "
                "to function for literal.\n");
        abort();
    }

    size_t (*FPTR)(void) = (size_t (*)(void))fptr;

    size_t res = (size_t) FPTR();

    fn->eraseFromParent();

    return res;
}

bool Generator::parseOffsetof(Element::Function *dfn,
                                      llvm::BasicBlock *block,
                                      Node *n,
                                      bool getAddress,
                                      bool prefixed_with_core,
                                      ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("offsetof", n, 2, 2)) {
        return false;
    }

    symlist *lst = n->list;

    /* First argument must be a struct name. */

    Node *struct_name = (*lst)[1];
    struct_name = parseOptionalMacroCall(struct_name);
    if (!struct_name) {
        return false;
    }

    Element::Type *mytype = parseType(struct_name,
                                      false,
                                      false);
    if (!mytype) {
        return false;
    }

    if (!mytype->struct_name) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, n,
            "struct", "offsetof", "not a struct"
        );
        erep->addError(e);
        return false;
    }

    Element::Struct *str =
        ctx->getStruct(
            mytype->struct_name->c_str(),
            mytype->namespaces
        );

    if (!str) {
        fprintf(stderr, "Internal error: invalid struct name.\n");
        abort();
    }

    /* Second argument is struct field. */

    bool res = getOffsetofType(block, mytype,
                           (*lst)[2]->token->str_value.c_str(), -1,
                           pr);
    return res;
}

bool Generator::parseAlignmentof(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        bool getAddress,
        bool prefixed_with_core,
        ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("alignmentof", n, 1, 1)) {
        return false;
    }

    symlist *lst = n->list;

    Node *mytype = (*lst)[1];
    mytype = parseOptionalMacroCall(mytype);
    if (!mytype) {
        return false;
    }
    Element::Type *type = parseType(mytype, false,
                                    false);
    if (!type) {
        return false;
    }

    bool res = getAlignmentofType(block, type, pr);
    return res;
}

bool Generator::parseSizeof(Element::Function *dfn,
                                    llvm::BasicBlock *block,
                                    Node *n,
                                    bool getAddress,
                                    bool prefixed_with_core,
                                    ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("sizeof", n, 1, -1)) {
        return false;
    }

    symlist *lst = n->list;

    /* Get the type to which it is being cast. */

    Node *thing = (*lst)[1];
    thing = parseOptionalMacroCall(thing);
    if (!thing) {
        return false;
    }

    Element::Type *type = parseType((*lst)[1], false, false);

    if (!type) {
        erep->popLastError();

        int error_count = erep->getErrorTypeCount(ErrorType::Error);

        ParseResult expr_res;
        bool res = 
            parseFunctionBodyInstr(
                dfn, block, (*lst)[1], true, NULL, &expr_res
            );

        if (!res) {
            popErrors(error_count);

            bool res =
                parseFunctionBodyInstr(
                    dfn, block, (*lst)[1], false, NULL, &expr_res
                );
            if (!res) {
                return false;
            }
            type  = expr_res.type;
            block = expr_res.block;

            if (hasRelevantDestructor(&expr_res)) {
                ParseResult temp;
                bool res = destructIfApplicable(&expr_res, NULL, &temp);
                if (!res) {
                    return false;
                }
                block = temp.block;
            }
        } else {
            type =
                (expr_res.type->points_to)
                ? expr_res.type->points_to
                : expr_res.type;
            block = expr_res.block;
            if (hasRelevantDestructor(&expr_res)) {
                ParseResult temp;
                bool res = destructIfApplicable(&expr_res, NULL, &temp);
                if (!res) {
                    return false;
                }
                block = temp.block;
            }
        }
    }

    
    bool res = getSizeofType(block, type, pr);

    return res;
}

bool isFunctionPointerVarArgs(Element::Type *fn_ptr)
{
    if (fn_ptr->points_to->parameter_types->size() == 0) {
        return false;
    }

    Element::Type *back = fn_ptr->points_to->parameter_types->back();

    return (back->base_type == dale::Type::VarArgs) ? 1 : 0;
}

int fnPtrNumberOfRequiredArgs(Element::Type *fn_ptr)
{
    if (fn_ptr->points_to->parameter_types->size() == 0) {
        return 0;
    }

    unsigned int num_of_args = fn_ptr->points_to->parameter_types->size();

    if (isFunctionPointerVarArgs(fn_ptr)) {
        num_of_args -= 1;
    }

    return num_of_args;
}

bool Generator::parseFuncallInternal(
    Element::Function *dfn,
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
        fnPtrNumberOfRequiredArgs(fn_ptr->type);

    if (isFunctionPointerVarArgs(fn_ptr->type)) {
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
    if (extra_call_args) {
        for (std::vector<llvm::Value*>::iterator b =
                    extra_call_args->begin(), e = extra_call_args->end(); b !=
                e; ++b) {
            call_args.push_back((*b));
        }
    }
    std::vector<Element::Type *>::iterator param_iter;

    while (skip--) {
        ++symlist_iter;
    }

    param_iter = fn_ptr->type->points_to->parameter_types->begin();
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
        bool res = parseFunctionBodyInstr(
            dfn, block, (*symlist_iter), getAddress, NULL, &p
        );
        if (!res) {
            return false;
        }

        block = p.block;

        if ((param_iter != fn_ptr->type->points_to->parameter_types->end())
                && (!(p.type->isEqualTo((*param_iter))))
                && ((*param_iter)->base_type != Type::VarArgs)) {

            llvm::Value *new_val = coerceValue(p.value,
                                               p.type,
                                               (*param_iter),
                                               block);

            if (!new_val) {
                std::string twant;
                std::string tgot;
                (*param_iter)->toStringProper(&twant);
                p.type->toStringProper(&tgot);
                char buf[100];
                sprintf(buf, "%d", arg_count);

                Error *e = new Error(
                    ErrorInst::Generator::IncorrectArgType,
                    (*symlist_iter),
                    "function pointer call",
                    twant.c_str(), buf, tgot.c_str()
                );
                erep->addError(e);
                return NULL;
            } else {
                call_args.push_back(new_val);
            }
        } else {
            call_args.push_back(p.value);
        }

        ++symlist_iter;

        if (param_iter != fn_ptr->type->points_to->parameter_types->end()) {
            ++param_iter;
            // Skip the varargs type.
            if (param_iter !=
                    fn_ptr->type->points_to->parameter_types->end()) {
                if ((*param_iter)->base_type == Type::VarArgs) {
                    ++param_iter;
                }
            }
        }
    }

    llvm::IRBuilder<> builder(block);

    llvm::Value *call_res =
        builder.CreateCall(fn, llvm::ArrayRef<llvm::Value*>(call_args));

    setPr(pr, block, fn_ptr->type->points_to->return_type, call_res);

    fn_ptr->block = pr->block;
    if (hasRelevantDestructor(fn_ptr)) {
        ParseResult temp;
        bool res = destructIfApplicable(fn_ptr, NULL, &temp);
        if (!res) {
            return false;
        }
        pr->block = temp.block;
    }

    return true;
}

bool Generator::parseFuncall(Element::Function *dfn,
                                     llvm::BasicBlock *block,
                                     Node *n,
                                     bool getAddress,
                                     bool prefixed_with_core,
                                     ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    /* (funcall <pointer-to-function> <arg1> <arg2> ...) */

    if (!assertArgNums("funcall", n, 1, -1)) {
        return false;
    }

    symlist *lst = n->list;

    ParseResult fn_ptr;
    bool res =
        parseFunctionBodyInstr(
            dfn, block, (*lst)[1], getAddress, NULL, &fn_ptr
        );
    if (!res) {
        return false;
    }

    if (!fn_ptr.type->points_to
            || !fn_ptr.type->points_to->is_function) {
        std::string temp;
        fn_ptr.type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            "funcall", "fn pointer", "1", temp.c_str()
        );
        erep->addError(e);
        return false;
    }

    res = parseFuncallInternal(
               dfn,
               n,
               getAddress,
               &fn_ptr,
               2,
               NULL,
               pr
           );
    return res;
}

bool Generator::parseVaStart(Element::Function *dfn,
                                     llvm::BasicBlock *block,
                                     Node *n,
                                     bool getAddress,
                                     bool prefixed_with_core,
                                     ParseResult *pr)
{
    if (!assertArgNums("va-start", n, 1, 1)) {
        return false;
    }

    symlist *lst = n->list;

    ParseResult pr_valist;
    bool res =
        parseFunctionBodyInstr(dfn, block, (*lst)[1], false, NULL,
                               &pr_valist);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_valist.block);
    llvm::Function *va_start =
        mod->getFunction(llvm::StringRef("llvm.va_start"));
    if (!va_start) {
        fprintf(stderr, "Unable to load va_start.");
        abort();
    }

    std::vector<llvm::Value*> call_args;
    ParseResult to_pchar;
    res = doCast(pr_valist.block,
                pr_valist.value,
                pr_valist.type,
                type_pchar,
                n, 0, &to_pchar);
    if (!res) {
        return false;
    }
    call_args.push_back(to_pchar.value);
    builder.CreateCall(va_start,
                       llvm::ArrayRef<llvm::Value*>(call_args)); 

    setPr(pr, to_pchar.block, type_void, NULL);

    return true;
}

bool Generator::parseVaEnd(Element::Function *dfn,
                                     llvm::BasicBlock *block,
                                     Node *n,
                                     bool getAddress,
                                     bool prefixed_with_core,
                                     ParseResult *pr)
{
    if (!assertArgNums("va-end", n, 1, 1)) {
        return false;
    }

    symlist *lst = n->list;

    ParseResult pr_valist;
    bool res =
        parseFunctionBodyInstr(dfn, block, (*lst)[1], false, NULL,
                               &pr_valist);
    if (!res) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_valist.block);
    llvm::Function *va_end =
        mod->getFunction(llvm::StringRef("llvm.va_end"));
    if (!va_end) {
        fprintf(stderr, "Unable to load va_end.");
        abort();
    }

    std::vector<llvm::Value*> call_args;
    ParseResult to_pchar;
    res = doCast(pr_valist.block,
                 pr_valist.value,
                 pr_valist.type,
                 type_pchar,
                 n, 0, &to_pchar);
    if (!res) {
        return false;
    }
    call_args.push_back(to_pchar.value);
    builder.CreateCall(va_end,
                       llvm::ArrayRef<llvm::Value*>(call_args)); 

    setPr(pr, to_pchar.block, type_void, NULL);
    return true;
}

bool Generator::parseVaArg(Element::Function *dfn,
                            llvm::BasicBlock *block,
                            Node *n,
                            bool getAddress,
                            bool prefixed_with_core,
                            ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("va-arg", n, 2, 2)) {
        return false;
    }

    symlist *lst = n->list;

    /* Get the arglist. */

    ParseResult pr_arglist;
    bool res = parseFunctionBodyInstr(dfn, block, (*lst)[1], false,
                                      NULL, &pr_arglist);
    if (!res) {
        return false;
    }

    /* Get the type to which it is being cast. */

    Element::Type *type = parseType((*lst)[2], false, false);

    if (!type) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_arglist.block);

    /* The va_arg intrinsic is not implemented for x86-64, which
     * is why it's done here.
     */

    llvm::Type *llvm_type =
        toLLVMType(type, NULL, false);
    if (!llvm_type) {
        failedDaleToLLVMTypeConversion(type);
        return NULL;
    }

    if (is_x86_64) {
        /*
            (See the AMD64 ABI draft of 13/01/2010 for more
            information, most of this is verbatim from there, it
            even details the algorithm to use. Own comments are
            further indented.)

            va_list on x86-64 looks like so:

            typedef struct {
                unsigned int gp_offset;
                unsigned int fp_offset;
                void *overflow_arg_area;
                void *reg_save_area;
            } va_list[1];

            reg_save_area: points to the start of the register
            save area.

            overflow_arg_area: points to the address of the first
            argument passed on the stack, if any. When an argument
            is fetched, it should be updated to point to the start
            of the next argument on the stack.

            gp_offset: holds the offset in bytes from
            reg_save_area to the place where the next available
            general purpose argument register is saved. In case
            all argument registers have been exhausted, it is set
            to the value 48 (6 * 8).

            fp_offset: holds the offset in bytes from
            reg_save_area to the place where the next available
            floating point argument register is saved. In case all
            argument registers have been exhausted, it is set to
            the value 304 ((6 * 8) + (16 * 16)).

                So add the struct in common declarations, cast
                va_list to the struct, get all the values out of
                it.

            Algorithm:
                1. Determine whether 'type' may be passed in the
                    registers. If not, go to 7.
                2. Compute 'num_gp' to hold the number of general
                    purpose registers needed to pass 'type' and
                    'num_fp' to hold the number of floating point
                    registers needed.
                3. Verify whether arguments fit into registers. In
                    the event that:

                    gp_offset > (48 - (num_gp * 8))
                        ||
                    fp_offset > (304 - (num_fp * 16))

                    go to 7.
                4. Fetch 'type' from 'reg_save_area' with an
                    offset of 'gp_offset' and/or 'fp_offset'. This
                    may require copying to a temporary location in
                    case the parameter is passed in different
                    register classes or requires an alignment of
                    greater than 8 for general purpose registers
                    and 16 for XMM registers.
                5. Set:

                    gp_offset += num_gp * 8
                    fp_offset += num_fp * 16

                6. Return the fetched type.

                7. Align 'overflow_arg_area' upwards to a 16-byte
                    boundary if alignment needed by 'type' exceeds
                    8-byte boundary.
                8. Fetch 'type' from 'overflow_arg_area'.
                9. Set:

                    overflow_arg_area += sizeof(type)

                10. Align overflow_arg_area upwards to an 8-byte
                    boundary.
                11. Return the fetched type.

        ----

            Only handling INTEGER class at the moment (being the
            normal number types (int, char, uint etc.) and
            pointers. Structs are a bit trickier.

            Cast arglist to a struct of the correct type.

            Get the fields from it as per the specification.

            Each register is 8 bytes, so the assumption for the
            moment is that everything fits in a register. num_gp is
            1, num_fp is 1.

            Get the value - gp_offset from reg_save_area.

            Update gp_offset and fp_offset as per the previous
            part.

            Return the fetched type.

            typedef struct {
                unsigned int gp_offset;
                unsigned int fp_offset;
                void *overflow_arg_area;
                void *reg_save_area;
            } va_list[1];
        */
        /* builder, pr_arglist, type */

        llvm::Value *pvlstr = llvm::cast<llvm::Value>(
                                  builder.CreateBitCast(
                                      pr_arglist.value,
                                      llvm::PointerType::getUnqual(
                                          ctx->getStruct("va-list")->type
                                      )
                                  )
                              );

        std::vector<llvm::Value *> indices_gp_offset;
        stl::push_back2(&indices_gp_offset, llvm_native_zero,
                        llvm_native_zero);

        std::vector<llvm::Value *> indices_overflow_arg_area;
        stl::push_back2(&indices_overflow_arg_area,
                        llvm_native_zero,
                        getNativeInt(2));

        std::vector<llvm::Value *> indices_reg_save_area;
        stl::push_back2(&indices_reg_save_area,
                        llvm_native_zero,
                        getNativeInt(3));

        llvm::Value *ptr_gpo =
            builder.CreateGEP(pvlstr,
                              llvm::ArrayRef<llvm::Value*>(indices_gp_offset));

        llvm::Value *gpo =
            llvm::cast<llvm::Value>(builder.CreateLoad(ptr_gpo));

        llvm::Value *ptr_oaa =
            builder.CreateGEP(pvlstr,
                              llvm::ArrayRef<llvm::Value*>(indices_overflow_arg_area));

        llvm::Value *oaa =
            llvm::cast<llvm::Value>(builder.CreateLoad(ptr_oaa));

        llvm::Value *ptr_rsa =
            builder.CreateGEP(pvlstr,
                              llvm::ArrayRef<llvm::Value*>(indices_reg_save_area));

        llvm::Value *rsa =
            llvm::cast<llvm::Value>(builder.CreateLoad(ptr_rsa));

        /* if gp_offset > (48 - (num_gp * 8))
           for the time being - if gp_offset > 40 */

        llvm::BasicBlock *then_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "then", dfn->llvm_function);
        llvm::BasicBlock *else_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "else", dfn->llvm_function);

        llvm::Value *cond =
            builder.CreateICmpUGT(
                gpo,
                llvm::ConstantInt::get(nt->getNativeUIntType(), 40)
            );

        builder.CreateCondBr(cond, then_block, else_block);

        llvm::IRBuilder<> builder_then(then_block);
        llvm::IRBuilder<> builder_else(else_block);

        /* Then component (after the first 5) */

        /*
                            7. Align 'overflow_arg_area' upwards to a 16-byte
                                boundary if alignment needed by 'type' exceeds
                                8-byte boundary.
                            8. Fetch 'type' from 'overflow_arg_area'.
                            9. Set:

                                overflow_arg_area += sizeof(type)

                            10. Align overflow_arg_area upwards to an 8-byte
                                boundary.
                            11. Return the fetched type.
        */

        std::vector<llvm::Value *> indices_po1;
        indices_po1.push_back(llvm::ConstantInt::get(nt->getNativeUIntType(),
                              0));

        llvm::Value *then_ptr_obj =
            builder_then.CreateGEP(oaa,
                                   llvm::ArrayRef<llvm::Value*>(indices_po1));

        llvm::Value *then_value=
            builder_then.CreateLoad(
                builder_then.CreateBitCast(
                    then_ptr_obj,
                    llvm::PointerType::getUnqual(llvm_type)
                )
            );

        builder_then.CreateStore(
            builder_then.CreateIntToPtr(
                builder_then.CreateAdd(
                    builder_then.CreatePtrToInt(
                        oaa,
                        llvm::Type::getInt64Ty(llvm::getGlobalContext())
                    ),
                    llvm::ConstantInt::get(
                        llvm::Type::getInt64Ty(llvm::getGlobalContext()),
                        8
                    )
                ),
                llvm::PointerType::getUnqual(
                    llvm::Type::getInt8Ty(llvm::getGlobalContext())
                )
            ),
            ptr_oaa
        );

        /* Else component (first 5) */

        std::vector<llvm::Value *> indices_po2;
        indices_po2.push_back(gpo);

        llvm::Value *ptr_obj =
            builder_else.CreateGEP(rsa,
                                   llvm::ArrayRef<llvm::Value*>(indices_po2));

        llvm::Value *else_value=
            builder_else.CreateLoad(
                builder_else.CreateBitCast(
                    ptr_obj,
                    llvm::PointerType::getUnqual(llvm_type)
                )
            );

        builder_else.CreateStore(
            builder_else.CreateAdd(gpo,
                                   llvm::ConstantInt::get(nt->getNativeIntType(),
                                           8)),
            ptr_gpo
        );

        /* End components */

        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "va_done", dfn->llvm_function);

        builder_then.CreateBr(done_block);
        builder_else.CreateBr(done_block);

        llvm::IRBuilder<> builder_done(done_block);

        llvm::PHINode *pn =
            builder_done.CreatePHI(llvm_type, 0);

        pn->addIncoming(then_value, then_block);
        pn->addIncoming(else_value, else_block);

        setPr(pr, done_block, type, llvm::cast<llvm::Value>(pn));

        return true;

    } else {
        llvm::Value *res =
            builder.CreateVAArg(pr_arglist.value, llvm_type);
        setPr(pr, pr_arglist.block, type, res);

        return true;
    }
}

/* If 'implicit' is set to 1, then int->ptr and ptr->int
 * conversions are disallowed. */
bool Generator::doCast(llvm::BasicBlock *block,
                               llvm::Value *value,
                               Element::Type *from_type,
                               Element::Type *to_type,
                               Node *n,
                               int implicit,
                               ParseResult *pr)
{
    return Operation::Cast::execute(ctx, mod, block,
                                    value, from_type, to_type,
                                    n, implicit, pr);
}

bool Generator::parseCast(Element::Function *dfn,
                                  llvm::BasicBlock *block,
                                  Node *n,
                                  bool getAddress,
                                  bool prefixed_with_core,
                                  ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    symlist *lst = n->list;

    if (!assertArgNums("cast", n, 2, 2)) {
        return false;
    }

    /* Get the value that is being cast. */

    ParseResult pr_value;
    bool res = 
        parseFunctionBodyInstr(dfn, block, (*lst)[1], false, NULL,
                               &pr_value);

    if (!res) {
        return false;
    }

    /* Get the type to which it is being cast. (It is allowable to
     * cast to a bitfield type, because there's no other way to
     * set a bitfield value.) */

    Element::Type *type = parseType((*lst)[2], false, true);
    if (!type) {
        return false;
    }

    /* If the type of the value and the target type are the same,
     * return the original value. */

    if (pr_value.type->isEqualTo(type)) {
        pr_value.copyTo(pr);
        return true;
    }

    ParseResult temp;
    res = doCast(pr_value.block,
                pr_value.value,
                pr_value.type,
                type,
                n,
                0,
                &temp);
    if (!res) {
        return false;
    }
    pr_value.block = temp.block;
    if (hasRelevantDestructor(&pr_value)) {
        ParseResult temp2;
        res = destructIfApplicable(&pr_value, NULL, &temp2);
        temp.block = temp2.block;
    }

    setPr(pr, temp.block, temp.type, temp.value);
    return true;
}

bool Generator::parseSref(Element::Function *dfn,
                            llvm::BasicBlock *block,
                            Node *n,
                            bool getAddress,
                            bool prefixed_with_core,
                            ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    symlist *lst = n->list;

    if (!assertArgNums(":", n, 2, 2)) {
        return false;
    }

    getAddress = true;

    int original_error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    ParseResult pr_struct;
    bool res = 
        parseFunctionBodyInstr(dfn, block, (*lst)[1], getAddress,
                               NULL, &pr_struct);

    if (!res) {
        /* If the error message is 'cannot take address of
         * non-lvalue', make getAddress false and go from there. */
        if (erep->getErrorTypeCount(ErrorType::Error) ==
                original_error_count + 1) {
            Error *e = erep->popLastError();
            if (e->instance ==
                    ErrorInst::Generator::CannotTakeAddressOfNonLvalue) {
                res =
                    parseFunctionBodyInstr(
                        dfn, block, (*lst)[1], false, NULL, &pr_struct
                    );
                if (!res) {
                    erep->addError(e);
                    return false;
                }
                getAddress = false;
            } else {
                erep->addError(e);
                return false;
            }
        } else {
            return false;
        }
    }

    /* If getAddress is false, allocate space for the struct and
     * modify pr_struct so that the rest of the code continues to
     * work. */

    if (!getAddress) {
        llvm::IRBuilder<> builder(pr_struct.block);
        llvm::Type *llvm_type =
            toLLVMType(pr_struct.type, NULL, false);
        if (!llvm_type) {
            failedDaleToLLVMTypeConversion(pr_struct.type);
            return false;
        }
        llvm::Value *store =
            builder.CreateAlloca(llvm_type);
        builder.CreateStore(pr_struct.value, store);
        pr_struct.type = tr->getPointerType(pr_struct.type);
        pr_struct.value = store;
    }

    /* Able to assume points_to here, because getAddress was set
     * to true in the preceding pfbi call. */

    int is_const = pr_struct.type->points_to->is_const;

    if (pr_struct.type->points_to->struct_name == NULL) {
        std::string temp;
        pr_struct.type->points_to->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            ":", "a struct", "1", temp.c_str()
        );
        erep->addError(e);
        return false;
    }

    if (ctx->getEnum(pr_struct.type->points_to->struct_name->c_str())) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            ":", "a struct", "1", "an enum"
        );
        erep->addError(e);
        return false;
    }

    Node *ref = (*lst)[2];
    ref = parseOptionalMacroCall(ref);
    if (!ref) {
        return false;
    }
    if (!assertArgIsAtom(":", ref, "2")) {
        return false;
    }
    if (!assertAtomIsSymbol(":", ref, "2")) {
        return false;
    }

    Token *t = ref->token;

    Element::Struct *structp =
        ctx->getStruct(
            pr_struct.type->points_to->struct_name->c_str(),
            pr_struct.type->points_to->namespaces
        );

    if (!structp) {
        Error *e = new Error(
            ErrorInst::Generator::NotInScope,
            ((*lst)[1]),
            pr_struct.type->points_to->struct_name->c_str()
        );
        erep->addError(e);
        return false;
    }

    int index = structp->nameToIndex(t->str_value.c_str());

    if (index == -1) {
        Error *e = new Error(
            ErrorInst::Generator::FieldDoesNotExistInStruct,
            ((*lst)[2]),
            t->str_value.c_str(),
            pr_struct.type->points_to->struct_name->c_str()
        );
        erep->addError(e);
        return false;
    }

    Element::Type *eltype = structp->indexToType(index);
    if (is_const) {
        eltype = tr->getConstType(eltype);
    }

    std::vector<llvm::Value *> indices;
    stl::push_back2(&indices, llvm_native_zero,
                    getNativeInt(index));

    llvm::IRBuilder<> builder(pr_struct.block);
    llvm::Value *vres =
        builder.CreateGEP(pr_struct.value,
                          llvm::ArrayRef<llvm::Value*>(indices));

    /* This has changed - sref will always return a pointer,
     * regardless of getAddress (it is as if getAddress was always
     * enabled). */

    setPr(pr, pr_struct.block, tr->getPointerType(eltype), vres);

    if (hasRelevantDestructor(&pr_struct)) {
        ParseResult temp;
        bool res = destructIfApplicable(&pr_struct, NULL, &temp);
        if (!res) {
            return false;
        }
        pr->block = temp.block;
    }

    return true;
}

bool Generator::parseAref(Element::Function *dfn,
                          llvm::BasicBlock *block,
                          Node *n,
                          bool getAddress,
                          bool prefixed_with_core,
                          ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("$", n, 2, 2)) {
        return false;
    }

    symlist *lst = n->list;
    ParseResult pr_array;
    bool res = parseFunctionBodyInstr(dfn, block, (*lst)[1], true,
                                      NULL, &pr_array);
    if (!res) {
        return false;
    }

    if (!(pr_array.type->points_to)) {
        std::string temp;
        pr_array.type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            "$", "a pointer or array", "1", temp.c_str()
        );
        erep->addError(e);
        return false;
    }

    ParseResult pr_index;
    res = 
        parseFunctionBodyInstr(dfn, pr_array.block, (*lst)[2], false,
                               NULL, &pr_index);
    if (!res) {
        return false;
    }

    /* Attempt to cast pr_index to a size type, if it is not such
     * a type already. */

    if (pr_index.type->base_type != Type::Size) {
        ParseResult newt;
        bool res = 
            doCast(pr_index.block, pr_index.value, pr_index.type,
                   type_size, (*lst)[2], IMPLICIT, &newt);
        if (!res) {
            std::string temp;
            pr_index.type->toStringProper(&temp);
            Error *e = new Error(
                ErrorInst::Generator::IncorrectArgType,
                ((*lst)[2]),
                "$", "int", "2", temp.c_str()
            );
            erep->addError(e);
            return false;
        }
        newt.copyTo(&pr_index);
    }

    llvm::IRBuilder<> builder(pr_index.block);

    llvm::Value *proper_ptr;
    llvm::Value *vres;

    if (pr_array.type->points_to->points_to) {
        proper_ptr = builder.CreateLoad(pr_array.value);
        std::vector<llvm::Value *> indices;
        indices.push_back(llvm::cast<llvm::Value>(pr_index.value));
        vres = builder.Insert(
                  llvm::GetElementPtrInst::Create(
                      proper_ptr,
                      llvm::ArrayRef<llvm::Value*>(indices)
                  ),
                  "asdf"
              );
    } else {
        std::vector<llvm::Value *> indices;
        stl::push_back2(&indices, llvm_native_zero,
                        llvm::cast<llvm::Value>(pr_index.value));
        vres = builder.Insert(
                  llvm::GetElementPtrInst::Create(
                      pr_array.value,
                      llvm::ArrayRef<llvm::Value*>(indices)
                  ),
                  "asdf"
              );
    }

    pr->block = pr_index.block;

    /* pr_array returns a pointer - that's what you want to
     * return, here. */
    if (pr_array.type->is_array) {
        pr->type = tr->getPointerType(pr_array.type->array_type);
    } else {
        if (pr_array.type->points_to->points_to) {
            pr->type = pr_array.type->points_to;
        } else if (pr_array.type->points_to->array_type) {
            pr->type = tr->getPointerType(
                pr_array.type->points_to->array_type
            );
        } else {
            std::string typestr;
            pr_array.type->toStringProper(&typestr);
            Error *e = new Error(
                ErrorInst::Generator::CanOnlyIndexIntoPointersAndArrays,
                n,
                typestr.c_str()
            );
            erep->addError(e);
            return false;
        }
    }
    pr->value = vres;

    if (hasRelevantDestructor(&pr_array)) {
        pr_array.block = pr_index.block;
        ParseResult temp;
        bool res = destructIfApplicable(&pr_array, NULL, &temp);
        if (!res) {
            return false;
        }
        pr_index.block = temp.block;
    }

    if (hasRelevantDestructor(&pr_index)) {
        ParseResult temp;
        bool res = destructIfApplicable(&pr_index, NULL, &temp);
        if (!res) {
            return false;
        }
        pr->block = temp.block;
    }

    return true;
}

bool Generator::parseIf(Element::Function *dfn,
                        llvm::BasicBlock *block,
                        Node *n,
                        bool getAddress,
                        bool prefixed_with_core,
                        ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("if", n, 3, 3)) {
        return false;
    }

    symlist *lst = n->list;

    ParseResult pr_cond;
    bool res =
        parseFunctionBodyInstr(dfn, block, (*lst)[1], getAddress,
                               NULL, &pr_cond);
    if (!res) {
        return false;
    }

    if (pr_cond.type->base_type != Type::Bool) {
        std::string temp;
        pr->type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            "if", "bool", "1", temp.c_str()
        );
        erep->addError(e);
        return false;
    }

    llvm::IRBuilder<> builder4(pr_cond.block);

    llvm::BasicBlock *then_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "then", dfn->llvm_function);
    llvm::BasicBlock *else_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "else", dfn->llvm_function);

    builder4.CreateCondBr(pr_cond.value, then_block, else_block);

    if (hasRelevantDestructor(&pr_cond)) {
        ParseResult temp;
        bool res = destructIfApplicable(&pr_cond, &builder4, &temp);
        if (!res) {
            return false;
        }
    }

    ctx->activateAnonymousNamespace();
    ParseResult pr_then;
    res =
        parseFunctionBodyInstr(dfn, then_block, (*lst)[2], getAddress,
                               NULL, &pr_then);
    scopeClose(dfn, then_block, NULL);
    ctx->deactivateAnonymousNamespace();
    if (!res) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    ParseResult pr_else;
    res =
        parseFunctionBodyInstr(dfn, else_block, (*lst)[3], getAddress,
                               NULL, &pr_else);
    scopeClose(dfn, else_block, NULL);
    ctx->deactivateAnonymousNamespace();
    if (!res) {
        return false;
    }

    /* If the last instruction in both of these blocks is already
     * a terminating instruction, or the parseResult said treat as
     * terminator, don't add a done block and don't add a phi
     * node. */
    
    llvm::Instruction *then_instr =
        (pr_then.block->size() > 0)
        ? &(pr_then.block->back())
        : NULL;

    llvm::Instruction *else_instr =
        (pr_else.block->size() > 0)
        ? &(pr_else.block->back())
        : NULL;

    int then_terminates = 
        (then_instr && then_instr->isTerminator())
        || pr_then.treat_as_terminator;

    int else_terminates = 
        (else_instr && else_instr->isTerminator())
        || pr_else.treat_as_terminator;
 
    if (then_terminates && else_terminates) {
        /* Nothing else to do here - note that this block should
         * not be used - if this is an if that returns a value,
         * then the correct block gets returned (the done block
         * down the bottom), but if it's terminating, then that's not
         * the case. */

        /* todo: this used to return pr_cond, which can't have been
         * right, but creating a new block may lead to other problems.
         * yep - because if this is used, there are problems, since it
         * has no predecessors. Let alone that it really should not be
         * used. */
        pr_cond.copyTo(pr);
        return true;
    }

    /* If the last instruction in one of these blocks is a terminator
     * or should be treated as such, a phi node is unnecessary. The
     * result of the if statement will be the last value from the
     * non-terminating block. */

    if (then_terminates && !else_terminates) {
        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "done_then_no_else", dfn->llvm_function);

        llvm::IRBuilder<> builder2(pr_else.block);
        builder2.CreateBr(done_block);

        setPr(pr, done_block, pr_else.type,
              llvm::cast<llvm::Value>(pr_else.value));
        return true;
    }

    if (else_terminates && !then_terminates) {
        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "done_else_no_then", dfn->llvm_function);

        llvm::IRBuilder<> builder2(pr_then.block);
        builder2.CreateBr(done_block);

        setPr(pr, done_block, pr_then.type,
              llvm::cast<llvm::Value>(pr_then.value));
        return true;
    }

    /* If neither branch terminates, then the values of both
     * branches must be of the same type. */

    if (!pr_then.type->isEqualTo(pr_else.type)) {
        std::string sthen;
        std::string selse;
        pr_then.type->toStringProper(&sthen);
        pr_else.type->toStringProper(&selse);
        Error *e = new Error(
            ErrorInst::Generator::IfBranchesHaveDifferentTypes,
            n,
            sthen.c_str(), selse.c_str()
        );
        erep->addError(e);
        return false;
    }

    llvm::BasicBlock *done_block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "done_phi", dfn->llvm_function);

    llvm::IRBuilder<> builder(pr_then.block);
    builder.CreateBr(done_block);
    llvm::IRBuilder<> builder2(pr_else.block);
    builder2.CreateBr(done_block);

    llvm::IRBuilder<> builder3(done_block);

    llvm::Type *llvm_then_type =
        toLLVMType(pr_then.type, NULL, false);
    if (!llvm_then_type) {
        failedDaleToLLVMTypeConversion(pr_then.type);
        return false;
    }
    llvm::PHINode *pn =
        builder3.CreatePHI(llvm_then_type, 0);

    pn->addIncoming(pr_then.value, pr_then.block);
    pn->addIncoming(pr_else.value, pr_else.block);

    setPr(pr, done_block, pr_then.type,
          llvm::cast<llvm::Value>(pn));

    /* There's no need to re-copy the value here, since it's
     * coming straight out of then or else. */
    pr->freshly_copied = 1;

    return true;
}

bool Generator::parseInFunctionDefine(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        bool getAddress,
        bool
        prefixed_with_core,
        ParseResult *pr)
{
    assert(n->list && "must receive a list!");

    if (!assertArgNums("def", n, 2, 2)) {
        return false;
    }

    symlist *lst = n->list;
    Node *nname = (*lst)[1];

    if (!assertArgIsAtom("def", nname, "1")) {
        return false;
    }
    if (!assertAtomIsSymbol("def", nname, "1")) {
        return false;
    }

    Token *t = nname->token;
    char name[255];
    strncpy(name, t->str_value.c_str(), 255);

    Node *ndef = (*lst)[2];

    if (!assertArgIsList("def", ndef, "2")) {
        return false;
    }

    Node *nvar = NULL;

    if ((ndef->list->size() < (int) 1)
            || (!(nvar = (*(ndef->list))[0]))
            || (!(nvar->is_token))
            || (!(nvar->token->type == TokenType::String))) {
        if (nvar->token->str_value.compare("var")
                && nvar->token->str_value.compare("struct")) {
            Error *e = new Error(
                ErrorInst::Generator::OnlyVarPermitted,
                ndef
            );
            erep->addError(e);
            return false;
        }
    }

    if (!(nvar->token->str_value.compare("struct"))) {
        parseStructDefinition(name, ndef);
        setPr(pr, block, type_int,
              llvm::ConstantInt::get(nt->getNativeIntType(), 0));
        return true;
    }

    symlist *newlist = ndef->list;
    nvar = (*newlist)[0];

    /* Parse linkage. */

    int linkage = parseLinkage((*newlist)[1]);
    if (!linkage) {
        return false;
    }

    if ((linkage != Linkage::Auto)
            && (linkage != Linkage::Intern)
            && (newlist->size() > 3)) {
        Error *e = new Error(
            ErrorInst::Generator::HasBothExternAndInitialiser,
            ndef
        );
        erep->addError(e);
        return false;
    }

    /* Check if the type is a single token string equal to "\". If
     * it is, then the type is implied based on the result of
     * parsing the later expression. */

    setPr(pr, block, type_int, 
          llvm::ConstantInt::get(nt->getNativeIntType(), 0));
    pr->do_not_destruct       = 1;
    pr->do_not_copy_with_setf = 1;

    Element::Type *type;

    if ((*newlist)[2]->is_token &&
            !(*newlist)[2]->token->str_value.compare("\\")) {
        if (newlist->size() != 4) {
            Error *e = new Error(
                ErrorInst::Generator::MustHaveInitialiserForImpliedType,
                ndef
            );
            erep->addError(e);
            return false;
        }

        ParseResult p;
        bool res =
            parseFunctionBodyInstr(
                dfn, block, (*newlist)[3], getAddress, NULL, &p
            );
        if (!res) {
            return false;
        }
        type  = p.type;
        block = p.block;

        llvm::IRBuilder<> builder(block);
        llvm::Type *et = toLLVMType(type, (*newlist)[2], false);
        if (!et) {
            failedDaleToLLVMTypeConversion(type);
            return false;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(et)
                               );
        Element::Variable *var2 = new Element::Variable();
        var2->name.append(name);
        var2->type = type;
        var2->value = new_ptr;
        var2->linkage = Linkage::Auto;
        int avres = ctx->ns()->addVariable(name, var2);

        if (!avres) {
            Error *e = new Error(
                ErrorInst::Generator::RedefinitionOfVariable,
                n,
                name
            );
            erep->addError(e);
            return false;
        }

        /* If the constant int 0 is returned, and this isn't an
         * integer type (or bool), then skip this part (assume
         * that the variable has been initialised by the user).
         * This is to save pointless copies/destructs, while still
         * allowing the variable to be fully initialised once the
         * define is complete. */

        if (!(type->isIntegerType()) && (type->base_type != Type::Bool)) {
            if (llvm::ConstantInt *temp =
                        llvm::dyn_cast<llvm::ConstantInt>(p.value)) {
                if (temp->getValue().getLimitedValue() == 0) {
                    pr->block = p.block;
                    return true;
                }
            }
        }

        if (!assertTypeEquality("def", n, p.type, type, 1)) {
            return NULL;
        }

        std::vector<Element::Type *> call_arg_types;
        call_arg_types.push_back(tr->getPointerType(type));
        call_arg_types.push_back(tr->getPointerType(type));
        Element::Function *or_setf =
            ctx->getFunction("setf-copy", &call_arg_types,
                             NULL, 0);
        if (or_setf && type->isEqualTo(p.type)) {
            std::vector<llvm::Value *> call_args2;
            call_args2.push_back(new_ptr);
            llvm::Value *new_ptr2 = 
                llvm::cast<llvm::Value>(
                    builder.CreateAlloca(toLLVMType(type, NULL, false))
                );
            builder.CreateStore(p.value, new_ptr2);
            call_args2.push_back(new_ptr2);
            builder.CreateCall(
                or_setf->llvm_function,
                llvm::ArrayRef<llvm::Value*>(call_args2));
        } else {
            call_arg_types.pop_back();
            call_arg_types.push_back(p.type);
            Element::Function *or_setf2 =
                ctx->getFunction("setf-copy", &call_arg_types,
                                 NULL, 0);
            if (or_setf2) {
                std::vector<llvm::Value *> call_args2;
                call_args2.push_back(new_ptr);
                call_args2.push_back(p.value);
                builder.CreateCall(
                    or_setf->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args2));
            } else {
                builder.CreateStore(p.value, new_ptr);
            }
        }
        ParseResult temp;
        bool mres = destructIfApplicable(&p, NULL, &temp);
        if (!mres) {
            return false;
        }

        pr->block = temp.block;
        return true;
    } else {
        /* Parse the type. */
        type = parseType((*newlist)[2], false, false);
        if (!type) {
            return false;
        }
        /* If it's a struct, check if it's must-init. */
        if (type->struct_name) {
            Element::Struct *mine =
                ctx->getStruct(
                    type->struct_name->c_str(),
                    type->namespaces
                );
            if (mine->must_init && (newlist->size() == 3)) {
                Error *e = new Error(
                    ErrorInst::Generator::MustHaveInitialiserForType,
                    ndef
                );
                erep->addError(e);
                return false;
            }
        }

        bool is_zero_sized =
            (type->array_type && (type->array_size == 0));

        /* Add an alloca instruction for this variable. */

        llvm::IRBuilder<> builder(block);
        llvm::Type *et = toLLVMType(type, (*newlist)[2], false);
        if (!et) {
            failedDaleToLLVMTypeConversion(type);
            return false;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(et)
                               );
        Element::Variable *var2 = new Element::Variable();
        var2->name.append(name);
        var2->type = type;
        var2->value = new_ptr;
        var2->linkage = linkage;
        int avres = ctx->ns()->addVariable(name, var2);
        if (!avres) {
            Error *e = new Error(
                ErrorInst::Generator::RedefinitionOfVariable,
                n,
                name
            );
            erep->addError(e);
            return false;
        }

        if (newlist->size() == 3) {
            if (type->is_const) {
                Error *e = new Error(
                    ErrorInst::Generator::MustHaveInitialiserForConstType,
                    ndef
                );
                erep->addError(e);
                return false;
            }

            /* Finished - no value for this define. If the type is
             * const, though, it's an error. */

            setPr(pr, block, type_int,
                  llvm::ConstantInt::get(nt->getNativeIntType(), 0));

            return true;
        }

        ParseResult p;
        bool res = 
            parseFunctionBodyInstr(
                dfn, block, (*newlist)[3], getAddress, type, &p
            );
        if (!res) {
            return false;
        }

        /* If the constant int 0 is returned, and this isn't an
         * integer type, then skip this part (assume that the
         * variable has been initialised by the user). This is to
         * save pointless copies/destructs, while still allowing
         * the variable to be fully initialised once the define is
         * complete. */

        if (!(type->isIntegerType()) && (type->base_type != Type::Bool)) {
            if (llvm::ConstantInt *temp =
                        llvm::dyn_cast<llvm::ConstantInt>(p.value)) {
                if (temp->getValue().getLimitedValue() == 0) {
                    pr->block = p.block;
                    return true;
                }
            }
        }

        /* Handle arrays that were given a length of 0. */
        if (is_zero_sized) {
            type = p.type;
            var2->type = type;
            et = toLLVMType(type, (*newlist)[2], false);
            new_ptr = llvm::cast<llvm::Value>(
                          builder.CreateAlloca(et)
                      );
            var2->value = new_ptr;
        }

        llvm::IRBuilder<> builder2(p.block);

        std::vector<Element::Type *> call_arg_types;
        call_arg_types.push_back(tr->getPointerType(type));
        call_arg_types.push_back(tr->getPointerType(type));
        Element::Function *or_setf =
            ctx->getFunction("setf-copy", &call_arg_types,
                             NULL, 0);
        if (or_setf && type->isEqualTo(p.type)) {
            std::vector<llvm::Value *> call_args2;
            call_args2.push_back(new_ptr);
            llvm::Value *new_ptr2 = 
                llvm::cast<llvm::Value>(
                    builder2.CreateAlloca(toLLVMType(type, NULL, false))
                );
            builder2.CreateStore(p.value, new_ptr2);
            call_args2.push_back(new_ptr2);
            builder2.CreateCall(
                or_setf->llvm_function,
                llvm::ArrayRef<llvm::Value*>(call_args2));
        } else {
            call_arg_types.clear();
            call_arg_types.push_back(tr->getPointerType(type));
            call_arg_types.push_back(p.type);
            Element::Function *or_setf2 =
                ctx->getFunction("setf-copy", &call_arg_types,
                                 NULL, 0);
            if (or_setf2) {
                std::vector<llvm::Value *> call_args2;
                call_args2.push_back(new_ptr);
                call_args2.push_back(p.value);
                builder2.CreateCall(
                    or_setf2->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args2));
            } else {
                if (!assertTypeEquality("def", n, p.type, type, 1)) {
                    return false;
                }
                builder2.CreateStore(p.value, new_ptr);
            }
        }
        ParseResult temp;
        bool mres = destructIfApplicable(&p, NULL, &temp);
        if (!mres) {
            return false;
        }

        pr->block = temp.block;
        return true;
    }
}

int Generator::mySizeToRealSize(int n)
{
    return nt->internalSizeToRealSize(n);
}

static int anoncount = 0;

bool Generator::parseFunctionBodyInstr(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        bool getAddress,
        Element::Type *wanted_type,
        ParseResult *pr)
{
    bool res =
        parseFunctionBodyInstrInternal(dfn, block, n,
                                       getAddress, wanted_type,
                                       pr);

    if (!res) {
        return false;
    }
    if (dfn->is_setf_fn) {
        return true;
    }
    /* todo: if there's never a use case for a separate
     * parseresult, then fix this function accordingly. */
    copyWithSetfIfApplicable(dfn, pr, pr);

    return true;
}

bool Generator::parseFunctionBodyInstrInternal(
    Element::Function *dfn,
    llvm::BasicBlock *block,
    Node *n,
    bool getAddress,
    Element::Type *wanted_type,
    ParseResult *pr)
{
    if (DALE_DEBUG) {
        printf("Called parseFunctionBodyInstruction: ");
        n->print();
        printf("\n");
    }

    global_block = block;

    if (n->is_token) {
        /* Single value. */
        Token *t = n->token;

        /* Check if we are expecting an enum. */

        Element::Enum *myenum2;
        if (wanted_type
                && (wanted_type->struct_name)
                && (myenum2 =
                        ctx->getEnum(wanted_type->struct_name->c_str()))) {

            Element::Struct *myenumstruct2 =
                ctx->getStruct(wanted_type->struct_name->c_str());

            if (!myenumstruct2) {
                fprintf(stderr,
                        "Internal error: no struct associated "
                        "with enum.\n");
                abort();
            }

            int original_error_count =
                erep->getErrorTypeCount(ErrorType::Error);

            /* Will fail here where the token is not a valid
             * literal, so in that case just continue onwards
             * (token could be a var name). */

            bool res =
                parseEnumLiteral(block, n,
                                 myenum2,
                                 wanted_type,
                                 myenumstruct2,
                                 getAddress,
                                 pr);

            if (res) {
                return res;
            } else {
                popErrors(original_error_count);
                goto tryvar;
            }
        } else if (t->type == TokenType::Int) {
            if (wanted_type
                    && wanted_type->isIntegerType()) {
                int mysize =
                    mySizeToRealSize(wanted_type->getIntegerSize());
                setPr(pr,
                           block,
                           tr->getBasicType(wanted_type->base_type),
                           getConstantInt(
                               llvm::IntegerType::get(
                                   llvm::getGlobalContext(),
                                   mysize
                               ),
                               t->str_value.c_str()
                           )
                       );
                return true;
            } else {
                setPr(pr,
                           block,
                           type_int,
                           getConstantInt(
                               nt->getNativeIntType(),
                               t->str_value.c_str()
                           )
                       );
                return true;
            }
        } else if (t->type == TokenType::FloatingPoint) {
            if (wanted_type
                    && wanted_type->base_type == Type::Float) {
                setPr(pr,
                           block,
                           type_float,
                           llvm::ConstantFP::get(
                               llvm::Type::getFloatTy(llvm::getGlobalContext()),
                               llvm::StringRef(t->str_value.c_str())
                           )
                       );
                return true;
            } else if (wanted_type
                       && wanted_type->base_type == Type::Double) {
                setPr(pr, 
                           block,
                           type_double,
                           llvm::ConstantFP::get(
                               llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                               llvm::StringRef(t->str_value.c_str())
                           )
                       );
                return true;
            } else if (wanted_type
                       && wanted_type->base_type == Type::LongDouble) {
                setPr(pr, 
                           block,
                           type_longdouble,
                           llvm::ConstantFP::get(
                               nt->getNativeLongDoubleType(),
                               llvm::StringRef(t->str_value.c_str())
                           )
                       );
                return true;
            } else {
                setPr(pr, 
                           block,
                           type_float,
                           llvm::ConstantFP::get(
                               llvm::Type::getFloatTy(llvm::getGlobalContext()),
                               llvm::StringRef(t->str_value.c_str())
                           )
                       );
                return true;
            }
        } else if (t->type == TokenType::String) {
tryvar:
            /* Special cases - boolean values. */
            int is_true  = !t->str_value.compare("true");
            int is_false = !t->str_value.compare("false");

            if (is_true || is_false) {
                setPr(pr, 
                           block,
                           type_bool,
                           llvm::ConstantInt::get(
                               llvm::Type::getInt1Ty(llvm::getGlobalContext()),
                               is_true
                           )
                       );
                return true;
            }

            /* Special case - characters. */
            if ((t->str_value.size() >= 3)
                    && (t->str_value.at(0) == '#')
                    && (t->str_value.at(1) == '\\')) {
                const char *temp = t->str_value.c_str();
                temp += 2;
                char c;

                if (!strcmp(temp, "NULL")) {
                    c = '\0';
                } else if (!strcmp(temp, "TAB")) {
                    c = '\t';
                } else if (!strcmp(temp, "SPACE")) {
                    c = ' ';
                } else if (!strcmp(temp, "NEWLINE")) {
                    c = '\n';
                } else if (!strcmp(temp, "CARRIAGE")) {
                    c = '\r';
                } else if (!strcmp(temp, "EOF")) {
                    c = EOF;
                } else {
                    if (strlen(temp) != 1) {
                        Error *e = new Error(
                            ErrorInst::Generator::InvalidChar,
                            n,
                            temp
                        );
                        erep->addError(e);
                        return false;
                    }
                    c = t->str_value.at(2);
                }

                setPr(pr,
                           block,
                           type_char,
                           llvm::ConstantInt::get(nt->getNativeCharType(), c)
                       );
                return true;
            }

            /* Plain string - has to be variable. */
            Element::Variable *var =
                ctx->getVariable(t->str_value.c_str());

            if (!var) {
                Error *e = new Error(
                    ErrorInst::Generator::VariableNotInScope,
                    n,
                    t->str_value.c_str()
                );
                erep->addError(e);
                return false;
            }

            llvm::IRBuilder<> builder(block);

            if (getAddress) {
                setPr(pr,
                           block,
                           tr->getPointerType(var->type),
                           var->value
                       );
                return true;
            } else {
                if (var->type->is_array) {
                    /* If the variable is an array, return a pointer of
                    * the array's type. */
                    llvm::Value *p_to_array =
                        builder.CreateGEP(
                            var->value,
                            llvm::ArrayRef<llvm::Value*>(two_zero_indices)
                        );

                    setPr(pr,
                               block,
                               tr->getPointerType(var->type->array_type),
                               p_to_array
                           );
                    return true;
                }

                /* Return the dereferenced variable. */
                setPr(pr, 
                           block,
                           var->type,
                           llvm::cast<llvm::Value>(
                               builder.CreateLoad(var->value)
                           )
                       );
                return true;
            }
        } else if (t->type == TokenType::StringLiteral) {

            /* Add the variable to the module */

            int size = 0;
            llvm::Constant *init = parseLiteral1(type_pchar, n, &size);
            if (!init) {
                return false;
            }
            Element::Type *temp = tr->getArrayType(type_char, size);

            llvm::Type *llvm_type =
                toLLVMType(temp, NULL, false);
            if (!llvm_type) {
                failedDaleToLLVMTypeConversion(temp);
                return false;
            }

            /* Have to check for existing variables with this
             * name, due to modules. */

            std::string varname;
            llvm::GlobalVariable *var;
            do {
                varname.clear();
                getUnusedVarname(&varname);
            } while (
                (var = mod->getGlobalVariable(varname.c_str(),
                                              llvm_type))
            );

            var =
                llvm::cast<llvm::GlobalVariable>(
                    mod->getOrInsertGlobal(varname.c_str(),
                                           llvm_type)
                );

            var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));
            var->setInitializer(init);
            var->setConstant(true);

            Element::Variable *var2 = new Element::Variable();
            var2->name.append(varname.c_str());
            var2->internal_name.append(varname);
            var2->type = temp;
            var2->value = llvm::cast<llvm::Value>(var);
            var2->linkage = Linkage::Intern;
            int avres = ctx->ns()->addVariable(varname.c_str(), var2);

            if (!avres) {
                Error *e = new Error(
                    ErrorInst::Generator::RedefinitionOfVariable,
                    n,
                    varname.c_str()
                );
                erep->addError(e);
                return false;
            }

            llvm::IRBuilder<> builder(block);

            llvm::Value *charpointer =
                builder.CreateGEP(
                    llvm::cast<llvm::Value>(var2->value),
                    llvm::ArrayRef<llvm::Value*>(two_zero_indices));

            setPr(pr, 
                       block,
                       type_pchar,
                       charpointer
                   );
            return true;
        } else {
            Error *e = new Error(
                ErrorInst::Generator::UnableToParseForm,
                n
            );
            erep->addError(e);
            return false;
        }
    }

    symlist *lst = n->list;

    if (lst->size() == 0) {
        Error *e = new Error(
            ErrorInst::Generator::NoEmptyLists,
            n
        );
        erep->addError(e);
        return false;
    }

    Node *first = (*lst)[0];

    if (!first->is_token) {
        first = parseOptionalMacroCall(first);
        if (!first) {
            return false;
        }
    }

    /* If the first node is a token, and it equals "fn", then
     * create an anonymous function and return a pointer to it. */

    if (first->is_token and !first->token->str_value.compare("fn")) {
        int preindex = ctx->lv_index;

        std::vector<NSNode *> active_ns_nodes = ctx->active_ns_nodes;
        std::vector<NSNode *> used_ns_nodes   = ctx->used_ns_nodes;
        ctx->popUntilNamespace(prefunction_ns);

        char buf[255];
        sprintf(buf, "_anon_%d", anoncount++);
        Element::Function *myanonfn = NULL;
        int error_count = erep->getErrorTypeCount(ErrorType::Error);

        parseFunction(buf, n, &myanonfn, Linkage::Intern, 1);

        int diff = erep->getErrorTypeCount(ErrorType::Error)
                   - error_count;

        if (diff) {
            ctx->active_ns_nodes = active_ns_nodes;
            ctx->used_ns_nodes = used_ns_nodes;
            return false;
        }

        Element::Type *fntype = new Element::Type();
        fntype->is_function = 1;
        fntype->return_type = myanonfn->return_type;

        std::vector<Element::Type *> *parameter_types =
            new std::vector<Element::Type *>;

        std::vector<Element::Variable *>::iterator iter;

        iter = myanonfn->parameter_types->begin();

        while (iter != myanonfn->parameter_types->end()) {
            parameter_types->push_back((*iter)->type);
            ++iter;
        }

        fntype->parameter_types = parameter_types;

        setPr(pr,
            block,
            tr->getPointerType(fntype),
            llvm::cast<llvm::Value>(myanonfn->llvm_function)
        );

        std::vector<Element::Variable *> myvars;
        ctx->ns()->getVarsAfterIndex(preindex, &myvars);
        for (std::vector<Element::Variable *>::iterator
                b = myvars.begin(),
                e = myvars.end();
                b != e;
                ++b) {
            (*b)->index = 0;
        }

        ctx->active_ns_nodes = active_ns_nodes;
        ctx->used_ns_nodes = used_ns_nodes;

        return true;
    }

    /* If wanted_type is present and is a struct, then use
     * parseStructLiteral, if the first list element is a list. */

    if (wanted_type
            && (wanted_type->struct_name)
            && (!first->is_token)) {

        Element::Struct *str =
            ctx->getStruct(wanted_type->struct_name->c_str(),
                           wanted_type->namespaces);

        if (!str) {
            fprintf(stderr,
                    "Internal error: cannot load struct '%s'.\n",
                    wanted_type->struct_name->c_str());
            abort();
        }

        bool res =
            parseStructLiteral(dfn, block, n,
                               wanted_type->struct_name->c_str(),
                               str,
                               wanted_type,
                               getAddress,
                               pr);
        return res;
    }

    if (!first->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        erep->addError(e);
        return false;
    }

    Token *t = first->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            n
        );
        erep->addError(e);
        return false;
    }

    /* If the first element matches an enum name, then make an
     * enum literal (a struct literal) from the remainder of the
     * form. */

    Element::Enum *myenum =
        ctx->getEnum(t->str_value.c_str());

    if (myenum) {
        if (lst->size() != 2) {
            goto past_en_parse;
        }
        Element::Struct *myenumstruct =
            ctx->getStruct(t->str_value.c_str());

        if (!myenumstruct) {
            fprintf(stderr,
                    "Internal error: no struct associated "
                    "with enum.\n");
            abort();
        }

        Element::Type *myenumtype =
            parseType((*lst)[0], false, false);

        if (!myenumtype) {
            fprintf(stderr,
                    "Internal error: no type associated "
                    "with enum.\n");
            abort();
        }

        int original_error_count =
            erep->getErrorTypeCount(ErrorType::Error);

        bool res = parseEnumLiteral(block, (*lst)[1],
                                             myenum,
                                             myenumtype,
                                             myenumstruct,
                                             getAddress,
                                             pr);
        if (!res) {
            popErrors(original_error_count);
            goto past_en_parse;
        }
        return true;
    }
past_en_parse:

    /* If the first element matches a struct name, then make a
     * struct literal from the remainder of the form. */

    Element::Struct *mystruct =
        ctx->getStruct(t->str_value.c_str());

    if (mystruct) {
        if (lst->size() != 2) {
            goto past_sl_parse;
        }

        Node *struct_name = (*lst)[0];
        Element::Struct *str =
            ctx->getStruct(t->str_value.c_str());
        if (!str) {
            fprintf(stderr,
                    "Internal error: cannot load struct '%s'.\n",
                    struct_name->token->str_value.c_str());
            abort();
        }

        Element::Type *structtype =
            parseType((*lst)[0], false, false);

        if (!structtype) {
            fprintf(stderr,
                    "Internal error: struct ('%s') type does "
                    "not exist.\n",
                    struct_name->token->str_value.c_str());
            abort();
        }

        int original_error_count =
            erep->getErrorTypeCount(ErrorType::Error);

        bool res = parseStructLiteral(dfn, block, (*lst)[1],
                                               "asdf",
                                               str,
                                               structtype,
                                               getAddress,
                                               pr);
        if (!res) {
            popErrors(original_error_count);
            goto past_sl_parse;
        }
        return true;
    }
past_sl_parse:

    /* If the first element is 'array', and an array type has been
     * requested, handle that specially. */

    if (wanted_type
            && wanted_type->is_array
            && (!strcmp(t->str_value.c_str(), "array"))) {
        /* go to parseArrayLiteral */
        int size;
        bool res = parseArrayLiteral(
                                dfn, block, n,
                                "array literal",
                                wanted_type,
                                getAddress,
                                &size,
                                pr
                            );
        return res;
    }

    /* Check that a macro/function exists with the relevant name.
       This can be checked by passing NULL in place of the types.
       If the form begins with 'core', though, skip this part. If
       any errors occur here, then pop them from the reporter and
       keep going - only if the rest of the function fails, should
       the errors be restored. */

    int error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    Error *backup_error = NULL;

    int prefixed_with_core = !(t->str_value.compare("core"));

    if (!prefixed_with_core) {
        Element::Function *fn_exists =
            ctx->getFunction(t->str_value.c_str(), NULL, NULL, 0);
        Element::Function *mac_exists =
            ctx->getFunction(t->str_value.c_str(), NULL, NULL, 1);

        if (fn_exists || mac_exists) {
            /* A function (or macro) with this name exists. Call
             * parseFunctionCall: if it returns a PR, then great.
             * If it returns no PR, but sets macro_to_call, then
             * pass off to parseMacroCall. If it returns no PR,
             * then pop the errors and continue, but only if there
             * is one error, and it's related to an overloaded
             * function not being present. */

            Element::Function *macro_to_call_real;
            Element::Function **macro_to_call = &macro_to_call_real;
            *macro_to_call = NULL;

            nesting += 4;
            bool res = parseFunctionCall(dfn, block, n,
                                   t->str_value.c_str(), getAddress,
                                   false, macro_to_call, pr);
            nesting -= 4;
            if (res) {
                return true;
            }

            if (*macro_to_call) {
                Node *mac_node =
                    parseMacroCall(n, t->str_value.c_str(),
                                   *macro_to_call);
                if (!mac_node) {
                    return false;
                }
                bool res =
                    parseFunctionBodyInstr(
                        dfn, block, mac_node, getAddress, wanted_type, pr
                    );

                delete mac_node;

                return res;
            }

            int error_count2 =
                erep->getErrorTypeCount(ErrorType::Error);

            if (error_count2 != (error_count + 1)) {
                return false;
            }
            backup_error = erep->popLastError();
            if (backup_error->getType() != ErrorType::Error) {
                erep->addError(backup_error);
                return false;
            }
            if ((backup_error->instance !=
                    ErrorInst::Generator::OverloadedFunctionOrMacroNotInScope)
                    && (backup_error->instance !=
                        ErrorInst::Generator::OverloadedFunctionOrMacroNotInScopeWithClosest)) {
                erep->addError(backup_error);
                return false;
            }
        }
    }

    if (prefixed_with_core) {
        std::vector<Node *> *temp = new std::vector<Node *>;
        temp->insert(temp->begin(),
                     lst->begin() + 1,
                     lst->end());
        lst = temp;
        n = new Node(temp);
        first = (*lst)[0];

        if (!first->is_token) {
            first = parseOptionalMacroCall(first);
            if (!first) {
                return false;
            }
        }
        if (!first->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::FirstListElementMustBeSymbol,
                n
            );
            erep->addError(e);
            return false;
        }

        t = first->token;
        if (t->type != TokenType::String) {
            Error *e = new Error(
                ErrorInst::Generator::FirstListElementMustBeSymbol,
                n
            );
            erep->addError(e);
            return false;
        }
    }

    /* Core forms (at least at this point). */

    bool (* core_fn2)(Generator *gen,
                      Element::Function *fn,
                      llvm::BasicBlock *block,
                      Node *node,
                      bool get_address,
                      bool prefixed_with_core,
                      ParseResult *pr);

    bool (dale::Generator::* core_fn)(
        Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        bool getAddress,
        bool prefixed_with_core,
        ParseResult *pr);

    core_fn2 =
        (eq("goto"))    ? &dale::Form::Goto::execute
                        : NULL;
    
    if (core_fn2) {
        return core_fn2(this, dfn, block, n,
                        getAddress, prefixed_with_core, pr);
    }
       
    core_fn =
          (eq("label"))   ? &dale::Generator::parseLabel
        : (eq("return"))  ? &dale::Generator::parseReturn
        : (eq("setf"))    ? &dale::Generator::parseSetf

        : (eq("@"))       ? &dale::Generator::parseDereference
        : (eq(":"))       ? &dale::Generator::parseSref
        : (eq("#"))       ? &dale::Generator::parseAddressOf
        : (eq("$"))       ? &dale::Generator::parseAref

        : (eq("get-dnodes")) ? &dale::Generator::parseGetDNodes

        : (eq("p="))      ? &dale::Generator::parsePtrEquals
        : (eq("p+"))      ? &dale::Generator::parsePtrAdd
        : (eq("p-"))      ? &dale::Generator::parsePtrSubtract
        : (eq("p<"))      ? &dale::Generator::parsePtrLessThan
        : (eq("p>"))      ? &dale::Generator::parsePtrMoreThan

        : (eq("def"))     ? &dale::Generator::parseInFunctionDefine
        : (eq("if"))      ? &dale::Generator::parseIf
        : (eq("null"))    ? &dale::Generator::parseNull
        : (eq("nullptr")) ? &dale::Generator::parseNullPtr
        : (eq("do"))      ? &dale::Generator::parseDo
        : (eq("cast"))    ? &dale::Generator::parseCast
        : (eq("va-arg"))  ? &dale::Generator::parseVaArg
        : (eq("va-start")) ? &dale::Generator::parseVaStart
        : (eq("va-end"))   ? &dale::Generator::parseVaEnd
        : (eq("sizeof"))  ? &dale::Generator::parseSizeof
        : (eq("offsetof")) ? &dale::Generator::parseOffsetof
        : (eq("alignmentof"))  ? &dale::Generator::parseAlignmentof
        : (eq("funcall"))  ? &dale::Generator::parseFuncall
        : (eq("using-namespace")) ? &dale::Generator::parseUsingNamespace
        : (eq("new-scope")) ?   &dale::Generator::parseNewScope
        : (eq("array-of")) ? &dale::Generator::parseArrayOf
        : NULL;

    if (core_fn) {
        return ((this)->*(core_fn))(dfn, block, n,
                                    getAddress, prefixed_with_core, pr);
    }

    /* Not core form - look for core macro. */

    Node* (dale::Generator::* core_mac)(Node *n);

    core_mac =   (eq("setv"))   ? &dale::Generator::parseSetv
               : (eq("@$"))     ? &dale::Generator::parseArrayDeref
               : (eq(":@"))     ? &dale::Generator::parseDerefStruct
               : (eq("@:"))     ? &dale::Generator::parseStructDeref
               : (eq("@:@"))    ? &dale::Generator::parseDerefStructDeref
               : NULL;

    if (core_mac) {
        /* Going to assume similarly here, re the error messages. */
        Node *new_node = ((this)->*(core_mac))(n);
        if (!new_node) {
            return false;
        }
        return parseFunctionBodyInstr(dfn, block, new_node,
                                      getAddress, wanted_type, pr);
    }

    /* Not core form/macro, nor function. If the string token is
     * 'destroy', then treat this as a no-op (because it's
     * annoying to have to check, in macros, whether destroy
     * happens to be implemented over a particular type). If it is
     * not, call pfbi on the first element. If it is a function
     * pointer, then go to funcall. If it is a struct, see if it
     * is a function object and go from there. Don't do any of
     * this if backup_error is set. */

    if (!(t->str_value.compare("destroy"))) {
        setPr(pr, block, type_void, NULL);
        return true;
    }

    if (backup_error) {
        erep->addError(backup_error);
        return false;
    }

    int last_error_count =
        erep->getErrorTypeCount(ErrorType::Error);
    ParseResult try_fnp;
    bool res = parseFunctionBodyInstr(
                               dfn, block, (*lst)[0], getAddress,
                               wanted_type, &try_fnp
                           );
    if (!res) {
        /* If this fails, and there is one extra error, and the
         * error is 'variable not in scope', then change it to
         * 'not in scope' (it could be intended as either a
         * variable, a macro or a fn). */
        int new_error_count =
            erep->getErrorTypeCount(ErrorType::Error);
        if (new_error_count == (last_error_count + 1)) {
            Error *e = erep->popLastError();
            if (e->instance ==
                    ErrorInst::Generator::VariableNotInScope) {
                e->instance = ErrorInst::Generator::NotInScope;
            }
            erep->addError(e);
        }
        return false;
    }
    block = try_fnp.block;
    if (try_fnp.type->points_to
            && try_fnp.type->points_to->is_function) {
        Token *funcall_str_tok = new Token(TokenType::String, 0,0,0,0);
        funcall_str_tok->str_value.clear();
        funcall_str_tok->str_value.append("funcall");
        Node *funcall_str_node = new Node(funcall_str_tok);
        funcall_str_node->filename = erep->current_filename;
        lst->insert(lst->begin(), funcall_str_node);
        bool res =
            parseFuncall(dfn,
                         block,
                         n,
                         getAddress,
                         false,
                         pr);
        return res;
    }
    res = parseFunctionBodyInstr(
                  dfn, try_fnp.block, (*lst)[0], true, wanted_type,
                  &try_fnp
              );
    if (!res) {
        return false;
    }
    if (try_fnp.type->points_to
            && try_fnp.type->points_to->struct_name) {
        /* Struct must implement 'apply' to be considered a
         * function object. */
        Element::Type *try_fnp_inner_type = try_fnp.type->points_to;
        Element::Struct *mystruct =
            ctx->getStruct(
                try_fnp_inner_type->struct_name->c_str(),
                try_fnp_inner_type->namespaces
            );
        if (mystruct) {
            Element::Type *apply =
                mystruct->nameToType("apply");
            if (apply
                    && apply->points_to
                    && apply->points_to->is_function) {
                /* The first argument of this function must be a
                 * pointer to this particular struct type. */
                Element::Type *applyfn = apply->points_to;
                if (!(applyfn->parameter_types->size())) {
                    Error *e = new Error(
                        ErrorInst::Generator::ApplyMustTakePointerToStructAsFirstArgument,
                        (*lst)[0]
                    );
                    erep->addError(e);
                    return false;
                }
                if (!(applyfn->parameter_types->at(0)->isEqualTo(
                            try_fnp.type))) {
                    Error *e = new Error(
                        ErrorInst::Generator::ApplyMustTakePointerToStructAsFirstArgument,
                        (*lst)[0]
                    );
                    erep->addError(e);
                    return false;
                }
                /* Get the function pointer value. */
                std::vector<llvm::Value *> indices;
                stl::push_back2(&indices,
                                llvm_native_zero,
                                getNativeInt(
                                    mystruct->nameToIndex("apply")));

                llvm::IRBuilder<> builder(block);
                llvm::Value *res =
                    builder.CreateGEP(
                        try_fnp.value,
                        llvm::ArrayRef<llvm::Value*>(indices)
                    );

                ParseResult supertemp;
                supertemp.type  = apply;
                supertemp.block = block;
                llvm::Value *pvalue =
                    llvm::cast<llvm::Value>(builder.CreateLoad(res));
                supertemp.value = pvalue;

                /* So a pointer to the struct is your first
                 * argument. Skip 1 element of the list when
                 * passing off (e.g. (adder 1)). */

                std::vector<llvm::Value*> extra_args;
                extra_args.push_back(try_fnp.value);
                return parseFuncallInternal(
                           dfn,
                           n,
                           getAddress,
                           &supertemp,
                           1,
                           &extra_args,
                           pr
                       );
            }
        }
    }

    Error *e = new Error(
        ErrorInst::Generator::NotInScope,
        n,
        t->str_value.c_str()
    );
    erep->addError(e);
    return false;
}

void Generator::setPdnode()
{
    if (!type_dnode) {
        Element::Type *st = tr->getStructType("DNode");
        Element::Type *tt = tr->getPointerType(st);

        llvm::Type *dnode =
            toLLVMType(st, NULL, false);
        if (!dnode) {
            fprintf(stderr, "Unable to fetch DNode type.\n");
            abort();
        }

        llvm::Type *pointer_to_dnode =
            toLLVMType(tt, NULL, false);
        if (!pointer_to_dnode) {
            fprintf(stderr, "Unable to fetch pointer to DNode type.\n");
            abort();
        }

        type_dnode = st;
        type_pdnode = tt;
        intptr_t temp = (intptr_t) pointer_to_dnode;
        llvm_type_pdnode = (llvm::Type *) temp;
        intptr_t temp1 = (intptr_t) dnode;
        llvm_type_dnode = (llvm::Type *) temp1;
    }
}

void Generator::setPoolfree()
{
    if (!pool_free_fptr) {
        pool_free_fptr =
            (void (*)(MContext *mcp))
                ee->getPointerToFunction(
                    ctx->getFunction("pool-free", NULL, 0)->llvm_function
                );
    }
}

DNode *callmacro(int arg_count, void *gen, void *mac, DNode **dnodes,
                 MContext **mc_ptr)
{
    ffi_type **args =
        (ffi_type **) malloc(arg_count * sizeof(ffi_type *));
    void **vals =
        (void **)     malloc(arg_count * sizeof(void *));
    PoolNode *pn =
        (PoolNode *)  malloc(sizeof(PoolNode));
    MContext *mc =
        (MContext *)  malloc(sizeof(MContext));
    *mc_ptr = mc;

    memset(pn, 0, sizeof(PoolNode));
    memset(mc, 0, sizeof(MContext));
    args[0] = &ffi_type_pointer;
    vals[0] = (void*) &mc;

    int actual_arg_count = arg_count - 1;
    mc->arg_count = actual_arg_count;
    mc->pool_node = pn;
    mc->generator = gen;

    int i;
    for (i = 1; i < arg_count; i++) {
        args[i] = &ffi_type_pointer;
    }
    for (i = 1; i < arg_count; i++) {
        vals[i] = (void *) &(dnodes[i - 1]);
    }

    ffi_cif cif;
    ffi_status res2 =
        ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_count,
                     &ffi_type_pointer, args);

    if (res2 != FFI_OK) {
        fprintf(stderr, "prep_cif failed, cannot run macro.\n");
        abort();
    }

    DNode *ret_node = NULL;
    //fprintf(stderr, "Ret node 1: %p\n", ret_node);
    ffi_call(&cif, (void (*)(void)) mac, (void *) &ret_node, vals);
    //fprintf(stderr, "Ret node 2: %p\n", ret_node);
    free(args);
    free(vals);

    return ret_node;
}

Node *Generator::parseMacroCall(Node *n,
                                const char *name,
                                Element::Function *macro_to_call)
{
    if (DALE_DEBUG) {
        fprintf(stderr, "Calling macro '%s'\n", name);
    }

    assert(n->list && "must receive a list!");

    symlist *lst = n->list;

    Node *nmc_name = (*lst)[0];

    if (!nmc_name->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        erep->addError(e);
        return NULL;
    }

    Token *t = nmc_name->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            n
        );
        erep->addError(e);
        return NULL;
    }

    /* Have to expand this to handle overloading itself
     * (macro_to_call is provided by PFBI, where applicable). */

    Element::Function *mc =
        macro_to_call
        ? macro_to_call
        : ctx->getFunction(t->str_value.c_str(), NULL, NULL, 1);

    if (!mc) {
        Error *e = new Error(
            ErrorInst::Generator::MacroNotInScope,
            n,
            t->str_value.c_str()
        );
        erep->addError(e);
        return NULL;
    }

    /* Used to be -1 for lst->size() here, but isn't anymore,
     * because of the implicit MContext variable that is passed
     * to each macro. */

    if (mc->isVarArgs()) {
        if ((lst->size()) < mc->numberOfRequiredArgs()) {
            char buf1[100];
            char buf2[100];
            sprintf(buf1, "%d", (int) (lst->size()));
            sprintf(buf2, "%d", (mc->numberOfRequiredArgs() - 1));
            Error *e = new Error(
                ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
                n,
                t->str_value.c_str(), buf2, buf1
            );
            erep->addError(e);
            return NULL;
        }
    } else {
        if ((lst->size()) != mc->numberOfRequiredArgs()) {
            char buf1[100];
            char buf2[100];
            sprintf(buf1, "%d", (int) (lst->size()));
            sprintf(buf2, "%d", (mc->numberOfRequiredArgs() - 1));
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                n,
                t->str_value.c_str(), buf2, buf1
            );
            erep->addError(e);
            return NULL;
        }
    }

    if (DALE_DEBUG) {
        fprintf(stderr, "Macro flag 1\n");
    }

    std::vector<Element::Variable *>::iterator var_iter;
    var_iter = mc->parameter_types->begin();
    // Skip implicit MContext arg. */
    ++var_iter;

    std::vector<DNode *> dnodes_to_free;

    setPdnode();

    DNode *myargs[256];
    int myargs_count = 0;

    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;

    while (node_iter != lst->end()) {
        Node *ni = (*node_iter);
        addMacroPosition(ni, n);

        if ((*var_iter)->type->base_type == Type::VarArgs) {
            /* Into varargs - always pointers to DNodes. */
            DNode *new_dnode = IntNodeToDNode((*node_iter));
            myargs[myargs_count++] = new_dnode;
            dnodes_to_free.push_back(new_dnode);
            ++node_iter;
            continue;
        }

        /* todo: Macros can have various parameter types, but the actual
         * arguments are always pointers to DNode. */
        DNode *new_dnode = IntNodeToDNode((*node_iter));
        myargs[myargs_count++] = new_dnode;
        dnodes_to_free.push_back(new_dnode);
        ++var_iter;
        ++node_iter;
    }

    void *callmacro_fptr = (void*) &callmacro;

    void *actualmacro_fptr =
        ee->getPointerToFunction(mc->llvm_function);

    /* Cast it to the correct type. */

    DNode* (*FP)(int arg_count, void *gen, void *mac_fn, DNode
    **dnodes, MContext **mcp) =
        (DNode* (*)(int, void*, void*, DNode**, MContext**))callmacro_fptr;

    /* Get the returned dnode. */

    MContext *mcontext;
    DNode *mc_result_dnode = FP(myargs_count + 1,
                                (void *) this,
                                (char *) actualmacro_fptr,
                                myargs,
                                &mcontext);

    /* Convert it to an int node. */

    //fprintf(stderr, "MC result dnode: %s: %p\n", name, mc_result_dnode);
    Node *mc_result_node =
        (mc_result_dnode) ? DNodeToIntNode(mc_result_dnode)
                          : NULL;

    /* Free the pool node. */

    pool_free_fptr(mcontext);
    free(mcontext);

    /* Add the macro position information to the nodes. */

    if (mc_result_node) {
        addMacroPosition(mc_result_node, n);
    }

    /* Finished - return the macro result node. */

    return mc_result_node;
}

void Generator::addMacroPosition(Node *n, Node *mac_node)
{
    if (!(n->macro_begin.getLineNumber())) {
        n->macro_begin.setLineAndColumn(
            mac_node->getBeginPos()->getLineNumber(),
            mac_node->getBeginPos()->getColumnNumber()
        );
        n->macro_end.setLineAndColumn(
            mac_node->getEndPos()->getLineNumber(),
            mac_node->getEndPos()->getColumnNumber()
        );
    }

    if (n->is_list) {
        std::vector<Node *>::iterator iter = n->list->begin();

        while (iter != n->list->end()) {
            addMacroPosition((*iter), mac_node);
            ++iter;
        }
    }

    return;
}

Node *Generator::parseOptionalMacroCall(Node *n)
{
    if (n->is_token) {
        return n;
    }

    if (!n->is_list) {
        return n;
    }

    symlist *lst = n->list;
    if (lst->size() == 0) {
        return n;
    }

    Node *mac_name = (*lst)[0];

    if (!mac_name->is_token) {
        return n;
    }

    Token *t = mac_name->token;

    /* Core macros. */

    Node* (dale::Generator::* core_mac)(Node *n);

    core_mac =   (eq("setv"))   ? &dale::Generator::parseSetv
               : (eq("@$"))     ? &dale::Generator::parseArrayDeref
               : (eq(":@"))     ? &dale::Generator::parseDerefStruct
               : (eq("@:"))     ? &dale::Generator::parseStructDeref
               : (eq("@:@"))    ? &dale::Generator::parseDerefStructDeref
               : NULL;

    if (core_mac) {
        Node *new_node = ((this)->*(core_mac))(n);
        if (!new_node) {
            return NULL;
        }

        return new_node;
    }

    Element::Function *ffn =
        ctx->getFunction(t->str_value.c_str(), NULL, 1);
    if (!ffn) {
        return n;
    }

    /* Create a temporary function for evaluating the arguments. */

    llvm::Type *llvm_return_type =
        toLLVMType(type_int, NULL, false);
    if (!llvm_return_type) {
        failedDaleToLLVMTypeConversion(type_int);
        return NULL;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[255];
    sprintf(buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        mod->getOrInsertFunction(new_name.c_str(), ft);
    if (!fnc) {
        fprintf(stderr, "Internal error: unable to add "
                "function ('%s') to module.\n",
                new_name.c_str());
        abort();
    }

    llvm::Function *fn =
        llvm::dyn_cast<llvm::Function>(fnc);
    if (!fn) {
        fprintf(stderr, "Internal error: unable to convert "
                "function constant to function "
                "for function '%s'.\n",
                new_name.c_str());
        abort();
    }

    std::vector<Element::Variable *> vars;

    Element::Function *dfn =
        new Element::Function(type_int,
                              &vars,
                              fn,
                              0,
                              new std::string(new_name),
                              0);
    dfn->linkage = Linkage::Intern;
    if (!dfn) {
        fprintf(stderr, "Internal error: unable to create new "
                "function (!) '%s'.\n",
                new_name.c_str());
        abort();
    }

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "entry",
                                 fn);

    /* Iterate over the arguments and collect the types. Make
     * backups of the existing state first. */

    std::vector<Element::Type *> types;

    int error_count = erep->getErrorTypeCount(ErrorType::Error);

    global_functions.push_back(dfn);
    global_function = dfn;

    global_blocks.push_back(block);
    global_block = block;

    ctx->activateAnonymousNamespace();

    for (std::vector<Node *>::iterator b = lst->begin() + 1,
            e = lst->end();
            b != e;
            ++b) {
        ParseResult mine;
        bool res =
            parseFunctionBodyInstr(dfn, block, *b, false, NULL,
                                   &mine);
        if (res) {
            /* Add the type. */
            types.push_back(mine.type);
            block = mine.block;
        }
        else {
            /* Add a (p DNode) to types. */
            types.push_back(type_pdnode);
        }
    }
    popErrors(error_count);

    ctx->deactivateAnonymousNamespace();

    global_functions.pop_back();
    if (global_functions.size()) {
        global_function = global_functions.back();
    } else {
        global_function = NULL;
    }

    global_blocks.pop_back();
    if (global_blocks.size()) {
        global_block = global_blocks.back();
    } else {
        global_block = NULL;
    }

    /* Remove the temporary function. */
    fn->eraseFromParent();

    /* Call getFunction with the new set of parameter types. */

    ffn = ctx->getFunction(t->str_value.c_str(), &types, 1);
    if (!ffn) {
        // No macro matching these type parameters.
        return n;
    }

    Node *mac_node = parseMacroCall(n, t->str_value.c_str(),
                                    ffn);

    if (!mac_node) {
        return NULL;
    }

    /* If a macro node was got, and it's a list containing two
     * elements, and the first element is 'do', then just return
     * the second element. */

    if ((!mac_node->is_token)
            && (mac_node->list->size() == 2)
            && (mac_node->list->at(0)->is_token)
            && (mac_node->list->at(0)
                ->token->str_value.compare("do") == 0)) {
        return parseOptionalMacroCall(mac_node->list->at(1));
    } else {
        return parseOptionalMacroCall(mac_node);
    }
}

bool Generator::parseEnumLiteral(llvm::BasicBlock *block,
        Node *n,
        Element::Enum *myenum,
        Element::Type *myenumtype,
        Element::Struct
        *myenumstructtype,
        bool getAddress,
        ParseResult *pr)
{
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, n,
            "atom", "enum literal", "list"
        );
        erep->addError(e);
        return false;
    }

    int num = myenum->nameToNumber(n->token->str_value.c_str());
    if (num == ENUM_NOTFOUND) {
        Error *e = new Error(
            ErrorInst::Generator::EnumValueDoesNotExist,
            n,
            n->token->str_value.c_str()
        );
        erep->addError(e);
        return false;
    }

    llvm::IRBuilder<> builder(block);

    llvm::Value *sp = llvm::cast<llvm::Value>(
                          builder.CreateAlloca(myenumstructtype->type)
                      );

    llvm::Value *res =
        builder.CreateGEP(sp,
                          llvm::ArrayRef<llvm::Value*>(two_zero_indices));

    llvm::Type *llvm_type =
        toLLVMType(myenumstructtype->element_types.at(0),
                       NULL, false);
    if (!llvm_type) {
        failedDaleToLLVMTypeConversion(
            myenumstructtype->element_types.at(0)
        );
        return false;
    }

    builder.CreateStore(llvm::ConstantInt::get(
                            llvm_type, num),
                        res);

    if (getAddress) {
        setPr(pr, block, tr->getPointerType(myenumtype), sp);
        return true;
    } else {
        llvm::Value *final_value =
            builder.CreateLoad(sp);

        setPr(pr, block, myenumtype, final_value);
        return true;
    }
}

bool Generator::parseArrayLiteral(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        const char *name,
        Element::Type *array_type,
        bool getAddress,
        int *size,
        ParseResult *pr)
{
    if (DALE_DEBUG) {
        fprintf(stderr, "Parsing array literal (%s).\n", name);
    }

    Node *array_list = n;

    if (!array_list->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, array_list,
            "list", "array initialisers", "atom"
        );
        erep->addError(e);
        return false;
    }

    symlist *lst = array_list->list;

    std::vector<Node *>::iterator iter = lst->begin();
    ++iter;

    std::vector<ParseResult *> elements;

    while (iter != lst->end()) {
        ParseResult *el = new ParseResult();
        bool res =
            parseFunctionBodyInstr(
                dfn,
                block,
                (*iter),
                false,
                array_type->array_type,
                el
            );

        if (!res) {
            return false;
        }
        if (!el->type->isEqualTo(array_type->array_type)) {
            std::string exptype;
            std::string gottype;
            array_type->array_type->toStringProper(&exptype);
            el->type->toStringProper(&gottype);

            Error *e = new Error(
                ErrorInst::Generator::IncorrectType,
                (*iter),
                exptype.c_str(), gottype.c_str()
            );
            erep->addError(e);
            return false;
        }
        elements.push_back(el);
        block = el->block;

        ++iter;
    }

    if ((array_type->array_size != 0)
            && (array_type->array_size != (int) elements.size())) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectNumberOfArrayElements,
            n,
            elements.size(), array_type->array_size
        );
        erep->addError(e);
        return NULL;
    }

    *size = (int) elements.size();
    array_type = tr->getArrayType(array_type->array_type, *size);

    llvm::Type *llvm_array_type =
        toLLVMType(array_type, n, false);
    if (!llvm_array_type) {
        failedDaleToLLVMTypeConversion(array_type);
        return NULL;
    }

    llvm::IRBuilder<> builder(block);

    llvm::Value *llvm_array = builder.CreateAlloca(llvm_array_type);
    std::vector<llvm::Value *> indices;
    indices.push_back(llvm_native_zero);

    for (int i = 0; i < (int) elements.size(); ++i) {
        indices.push_back(llvm::ConstantInt::get(nt->getNativeIntType(), i));

        llvm::Value *res = builder.Insert(
                               llvm::GetElementPtrInst::Create(
                                   llvm_array,
                                   llvm::ArrayRef<llvm::Value*>(indices)
                               ),
                               "asdf"
                           );

        builder.CreateStore(elements[i]->value, res);

        indices.pop_back();
        delete elements[i];
    }

    setPr(pr, block, array_type, llvm_array);

    if (getAddress) {
        pr->type = tr->getPointerType(array_type);
    } else {
        /* Add a load instruction */
        llvm::Value *pvalue =
            llvm::cast<llvm::Value>(builder.CreateLoad(llvm_array));
        pr->value = pvalue;
    }

    return true;
}

bool Generator::parseStructLiteral(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        const char *struct_name,
        Element::Struct *str,
        Element::Type *structtype,
        bool getAddress,
        ParseResult *pr)
{
    Node *struct_list = n;

    if (!struct_list->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, struct_list,
            "list", "struct initialisers", "atom"
        );
        erep->addError(e);
        return false;
    }

    symlist *slst = struct_list->list;

    std::vector<Node *>::iterator siter = slst->begin();

    llvm::IRBuilder<> builder(block);

    llvm::Value *sp = llvm::cast<llvm::Value>(
                          builder.CreateAlloca(str->type)
                      );

    while (siter != slst->end()) {
        Node *sel = (*siter);
        if (!sel->is_list) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement, sel,
                "list", "struct initialiser", "atom"
            );
            erep->addError(e);
            return false;
        }
        symlist *sellst = sel->list;
        if (sellst->size() != 2) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement, sel,
                "list", "struct initialiser", "atom"
            );
            erep->addError(e);
            return false;
        }
        Node *name      = (*sellst)[0];
        Node *namevalue = (*sellst)[1];

        if (!name->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement, sel,
                "atom", "struct field name", "list"
            );
            erep->addError(e);
            return false;
        }

        Element::Type *nametype =
            str->nameToType(name->token->str_value.c_str());

        if (!nametype) {
            Error *e = new Error(
                ErrorInst::Generator::FieldDoesNotExistInStruct,
                name, name->token->str_value.c_str(),
                struct_name
            );
            erep->addError(e);
            return false;
        }

        int index = str->nameToIndex(name->token->str_value.c_str());

        std::vector<llvm::Value *> indices;
        stl::push_back2(&indices, llvm_native_zero,
                        getNativeInt(index));

        llvm::Value *res =
            builder.CreateGEP(sp,
                              llvm::ArrayRef<llvm::Value*>(indices));

        ParseResult newvalue;
        bool mres = 
            parseFunctionBodyInstr(dfn, block, namevalue, false, NULL,
                                   &newvalue);

        if (!mres) {
            return false;
        }
        if (!newvalue.type->isEqualTo(nametype)) {
            if ((nametype->isIntegerType()
                    && newvalue.type->isIntegerType())
                    || (nametype->isFloatingPointType()
                        && newvalue.type->isFloatingPointType())) {
                ParseResult casttemp;
                bool res =
                    doCast(newvalue.block,
                           newvalue.value,
                           newvalue.type,
                           nametype,
                           sel,
                           0,
                           &casttemp
                          );
                if (!res) {
                    return false;
                }
                casttemp.copyTo(&newvalue);
            } else {
                std::string expstr;
                std::string gotstr;
                nametype->toStringProper(&expstr);
                newvalue.type->toStringProper(&gotstr);
                Error *e = new Error(
                    ErrorInst::Generator::IncorrectType,
                    name,
                    expstr.c_str(), gotstr.c_str()
                );
                erep->addError(e);
                return false;
            }
        }

        block = newvalue.block;
        builder.SetInsertPoint(block);

        builder.CreateStore(newvalue.value,
                            res);

        ++siter;
    }

    if (getAddress) {
        setPr(pr, block, tr->getPointerType(structtype), sp);
    } else {
        llvm::Value *final_value = builder.CreateLoad(sp);
        setPr(pr, block, structtype, final_value);
    }
    return true;
}

bool Generator::parseFunctionCall(Element::Function *dfn,
        llvm::BasicBlock *block,
        Node *n,
        const char *name,
        bool getAddress,
        bool prefixed_with_core,
        Element::Function **macro_to_call,
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
    std::vector<Element::Type *> call_arg_types;

    std::vector<llvm::Value *> call_args_newer;
    std::vector<Element::Type *> call_arg_types_newer;

    if (!strcmp(name, "setf")) {
        /* Add a bool argument and type to the front of the
         * function call. */
        call_arg_types.push_back(type_bool);
        call_args.push_back(llvm_bool_false);
    }

    symlist_iter = lst->begin();
    /* Skip the function name. */
    ++symlist_iter;

    /* The processing below is only required when the function/macro
     * name is overloaded. For now, short-circuit for macros that are
     * not overloaded, because that will give the greatest benefits.
     * */

    if (!ctx->isOverloadedFunction(t->str_value.c_str())) {
        std::map<std::string, std::vector<Element::Function *> *>::iterator
            iter;
        Element::Function *fn = NULL;
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
            setPdnode();
            /* If the third argument is either non-existent, or a (p
             * DNode) (because typed arguments must appear before the
             * first (p DNode) argument), then short-circuit, so long
             * as the argument count is ok. */
            std::vector<Element::Variable*>::iterator
                b = (fn->parameter_types->begin() + 1);
            if ((b == fn->parameter_types->end())
                    || (*b)->type->isEqualTo(type_pdnode)) {
                bool use = false;
                if (fn->isVarArgs()) {
                    use = ((fn->numberOfRequiredArgs() - 1)
                            <= (lst->size() - 1));
                } else {
                    use = ((fn->numberOfRequiredArgs() - 1)
                            == (lst->size() - 1));
                }
                if (use) {
                    *macro_to_call = fn;
                    return NULL;
                }
            }
        }
    }

    std::vector<Error*> errors;

    /* Record the number of blocks and the instruction index in the
     * current block. If the underlying Element::Function to call
     * is a function, then there's no problem with using the
     * modifications caused by the repeated PFBI calls below. If
     * it's a macro, however, anything that occurred needs to be
     * 'rolled back'. Have to do the same thing for the context. */

    int current_block_count = dfn->llvm_function->size();
    int current_instr_index = block->size();
    int current_dgcount = dfn->defgotos->size();
    std::map<std::string, Element::Label *> labels = *(dfn->labels);
    llvm::BasicBlock *original_block = block;
    ContextSavePoint *csp = new ContextSavePoint(ctx);

    while (symlist_iter != lst->end()) {
        int error_count =
            erep->getErrorTypeCount(ErrorType::Error);

        ParseResult p;
        bool res = 
            parseFunctionBodyInstr(dfn, block, (*symlist_iter),
                                   getAddress, NULL,
                                   &p);

        int diff = erep->getErrorTypeCount(ErrorType::Error)
                   - error_count;

        if (!res || diff) {
            /* May be a macro call (could be an unparseable
             * argument). Pop and store errors for the time being
             * and treat this argument as a (p DNode). */

            if (diff) {
                errors.insert(errors.end(),
                              erep->errors->begin() + error_count,
                              erep->errors->end());
                erep->errors->erase(erep->errors->begin() + error_count,
                                    erep->errors->end());
            }

            call_args.push_back(NULL);
            call_arg_types.push_back(type_pdnode);
            ++symlist_iter;
            continue;
        }

        block = p.block;
        if (p.type->is_array) {
            llvm::IRBuilder<> builder(block);
            llvm::Type *llvm_type =
                toLLVMType(p.type, NULL, false);
            if (!llvm_type) {
                failedDaleToLLVMTypeConversion(p.type);
                return false;
            }
            llvm::Value *newptr =
                builder.CreateAlloca(llvm_type);
            builder.CreateStore(p.value, newptr);

            llvm::Value *p_to_array =
                builder.CreateGEP(
                    newptr,
                    llvm::ArrayRef<llvm::Value*>(two_zero_indices));
            call_arg_types.push_back(
                tr->getPointerType(p.type->array_type)
            );
            call_args.push_back(p_to_array);
        } else {
            call_args.push_back(p.value);
            call_arg_types.push_back(p.type);
        }

        ++symlist_iter;
    }

    /* Now have all the argument types. Get the function out of
     * the context. */

    Element::Function *closest_fn = NULL;

    Element::Function *fn =
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

        int dg_to_pop_back = dfn->defgotos->size() - current_dgcount;
        while (dg_to_pop_back--) {
            dfn->defgotos->pop_back();
        }
        *dfn->labels = labels;

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

            std::vector<Element::Variable *> *myarg_types =
                fn->parameter_types;
            std::vector<Element::Variable *>::iterator miter =
                myarg_types->begin();

            std::vector<llvm::Value *>::iterator citer =
                call_args.begin();
            std::vector<Element::Type *>::iterator caiter =
                call_arg_types.begin();

            /* Create strings describing the types, for use in a
             * possible error message. */

            std::string expected_args;
            std::string provided_args;
            while (miter != myarg_types->end()) {
                (*miter)->type->toStringProper(&expected_args);
                expected_args.append(" ");
                ++miter;
            }
            if (expected_args.size() == 0) {
                expected_args.append("void");
            } else {
                expected_args.erase(expected_args.size() - 1, 1);
            }
            while (caiter != call_arg_types.end()) {
                (*caiter)->toStringProper(&provided_args);
                provided_args.append(" ");
                ++caiter;
            }
            if (provided_args.size() == 0) {
                provided_args.append("void");
            } else {
                provided_args.erase(provided_args.size() - 1, 1);
            }
            miter = myarg_types->begin();
            caiter = call_arg_types.begin();

            if (call_args.size() < fn->numberOfRequiredArgs()) {
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
                    && call_args.size() != fn->numberOfRequiredArgs()) {
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

            while (miter != myarg_types->end()
                    && citer != call_args.end()
                    && caiter != call_arg_types.end()) {
                if ((*miter)->type->isEqualTo((*caiter))) {
                    call_args_newer.push_back((*citer));
                    call_arg_types_newer.push_back((*caiter));
                    ++miter;
                    ++citer;
                    ++caiter;
                    continue;
                }
                if (!(*miter)->type->isIntegerType()
                        and (*miter)->type->base_type != Type::Bool) {
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
                        and (*caiter)->base_type != Type::Bool) {
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
                bool res = doCast(block,
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
                call_args_newer.push_back(mytemp.value);
                call_arg_types_newer.push_back(mytemp.type);

                ++miter;
                ++citer;
                ++caiter;
            }

            call_args = call_args_newer;
            call_arg_types = call_arg_types_newer;
        } else if (ctx->existsNonExternCFunction(t->str_value.c_str())) {
            /* Return a no-op ParseResult if the function name is
             * 'destroy', because it's tedious to have to check in
             * generic code whether a particular value can be
             * destroyed or not. */
            if (!t->str_value.compare("destroy")) {
                setPr(pr, block, type_void, NULL);
                return true;
            }

            std::vector<Element::Type *>::iterator titer =
                call_arg_types.begin();

            std::string args;
            while (titer != call_arg_types.end()) {
                (*titer)->toStringProper(&args);
                ++titer;
                if (titer != call_arg_types.end()) {
                    args.append(" ");
                }
            }

            if (closest_fn) {
                std::string expected;
                std::vector<Element::Variable *>::iterator viter;
                viter = closest_fn->parameter_types->begin();
                if (closest_fn->is_macro) {
                    ++viter;
                }
                while (viter != closest_fn->parameter_types->end()) {
                    (*viter)->type->toStringProper(&expected);
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
        int n = fn->numberOfRequiredArgs();

        std::vector<llvm::Value *>::iterator call_args_iter
        = call_args.begin();
        std::vector<Element::Type *>::iterator call_arg_types_iter
        = call_arg_types.begin();

        while (n--) {
            ++call_args_iter;
            ++call_arg_types_iter;
        }
        while (call_args_iter != call_args.end()) {
            if ((*call_arg_types_iter)->base_type == Type::Float) {
                (*call_args_iter) =
                    builder.CreateFPExt(
                        (*call_args_iter),
                        llvm::Type::getDoubleTy(llvm::getGlobalContext())
                    );
                (*call_arg_types_iter) =
                    type_double;
            } else if ((*call_arg_types_iter)->isIntegerType()) {
                int real_size =
                    mySizeToRealSize(
                        (*call_arg_types_iter)->getIntegerSize()
                    );

                if (real_size < nt->getNativeIntSize()) {
                    if ((*call_arg_types_iter)->isSignedIntegerType()) {
                        /* Target integer is signed - use sext. */
                        (*call_args_iter) =
                            builder.CreateSExt((*call_args_iter),
                                               toLLVMType(type_int,
                                                              NULL, false));
                        (*call_arg_types_iter) = type_int;
                    } else {
                        /* Target integer is not signed - use zext. */
                        (*call_args_iter) =
                            builder.CreateZExt((*call_args_iter),
                                               toLLVMType(type_uint,
                                                              NULL, false));
                        (*call_arg_types_iter) = type_uint;
                    }
                }
            }
            ++call_args_iter;
            ++call_arg_types_iter;
        }
    }

    llvm::Value *call_res = builder.CreateCall(
                                fn->llvm_function,
                                llvm::ArrayRef<llvm::Value*>(call_args));

    setPr(pr, block, fn->return_type, call_res);

    /* If the return type of the function is one that should be
     * copied with an overridden setf, that will occur in the
     * function, so prevent the value from being re-copied here
     * (because no corresponding destructor call will occur). */

    pr->do_not_copy_with_setf = 1;

    return true;
}

/* The node given here is the whole function node - so the actual
 * function body parts begin at element (skip). */
int Generator::parseFunctionBody(Element::Function *dfn,
                                 llvm::Function *fn,
                                 Node *n,
                                 int skip,
                                 int is_anonymous)
{
    assert(n->list && "parseFunctionBody must receive a list!");

    symlist *lst = n->list;

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", fn);

    global_blocks.push_back(block);
    global_block = block;

    llvm::BasicBlock *next  = block;
    llvm::IRBuilder<> builder(block);

    /* Add 'define' calls for the args to the function. */

    std::vector<Element::Variable *>::iterator fn_args_iter;
    fn_args_iter = dfn->parameter_types->begin();

    /* Add the variables to the context, once confirmed that it is
     * not just a declaration. */

    int mcount = 0;

    setPdnode();

    while (fn_args_iter != dfn->parameter_types->end()) {

        if ((*fn_args_iter)->type->base_type == Type::VarArgs) {
            break;
        }
        int avres;
        Element::Variable *myvart = (*fn_args_iter);
        Element::Variable *myvar = new Element::Variable();
        myvar->type          = myvart->type;
        myvar->name          = myvart->name;
        myvar->internal_name = myvart->internal_name;
        myvar->value         = myvart->value;
        myvar->has_initialiser = myvart->has_initialiser;
        myvar->once_tag = myvart->once_tag;
        myvar->index = myvart->index;
        myvar->linkage = Linkage::Auto;

        if (mcount >= 1 && dfn->is_macro) {
            /* Macro arguments, past the first, always have a type of
             * (p DNode), notwithstanding that the type in the
             * Element::Function can be anything (to support
             * overloading). */
            std::string mtype;
            myvar->type->toStringProper(&mtype);
            myvar->type = type_pdnode;
        }
        avres = ctx->ns()->addVariable(
                    myvar->name.c_str(), myvar
                );
        if (!avres) {
            Error *e = new Error(
                ErrorInst::Generator::RedefinitionOfVariable,
                n,
                myvar->name.c_str()
            );
            erep->addError(e);
            return 0;
        }

        llvm::Value *original_value = myvar->value;

        /* Make CreateAlloca instructions for each argument. */
        llvm::Type *llvm_type =
            toLLVMType(myvar->type,
                           NULL,
                           false);
        if (!llvm_type) {
            failedDaleToLLVMTypeConversion(myvar->type);
            return 0;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(llvm_type)
                               );

        myvar->value = new_ptr;

        builder.CreateStore(original_value, new_ptr);

        ++fn_args_iter;
        ++mcount;
    }

    std::vector<Node *>::iterator iter;
    iter = lst->begin();

    llvm::Value *last_value = NULL;
    Element::Type *last_type = NULL;
    Node *last_position = NULL;

    /* Skip the fn token, the linkage, the return type and the
     * arg list. */

    int count = 0;
    int size  = (int) lst->size();

    while (skip--) {
        ++count;
        ++iter;
    }

    while (iter != lst->end()) {
        Element::Type *wanted_type = NULL;
        if ((count + 1) == size) {
            wanted_type = dfn->return_type;
        }
        ParseResult p;
        bool res =
            parseFunctionBodyInstr(dfn, next, (*iter), false,
            wanted_type, &p);
        if (!res) {
            /* Add an option to stop on first error, which would
             * break here. */
        } else {
            next = p.block;
            if ((count + 1) == size) {
                /* Set the value of the last instruction for possible
                 * implicit return later on. */
                last_value = p.value;
                /* Set the last type so as to ensure that the final
                 * return value is valid. */
                last_type = p.type;
                Token *x = new Token(
                    0,
                    (*iter)->getBeginPos()->getLineNumber(),
                    (*iter)->getBeginPos()->getColumnNumber(),
                    (*iter)->getEndPos()->getLineNumber(),
                    (*iter)->getEndPos()->getColumnNumber()
                );
                last_position = new Node(x);
                last_position->filename = (*iter)->filename;
            } else {
                ParseResult temp;
                bool res = destructIfApplicable(&p, NULL, &temp);
                if (!res) {
                    return 0;
                }
                next = temp.block;
            }
        }
        ++iter;
        ++count;
    }

    int res = 1;
    int bcount;
    int bmax;

    if (dfn->defgotos->size() > 0) {
        /* Have got deferred gotos - try to resolve. */
        std::vector<DeferredGoto *>::iterator dgiter =
            dfn->defgotos->begin();

        while (dgiter != dfn->defgotos->end()) {
            std::string *ln     = (*dgiter)->label_name;
            Element::Label *ell = dfn->getLabel(ln->c_str());
            if (!ell) {
                Error *e = new Error(
                    ErrorInst::Generator::LabelNotInScope,
                    n,
                    ln->c_str()
                );
                erep->addError(e);

                delete ln;
                delete (*dgiter);

                ++dgiter;

                continue;
            }
            llvm::BasicBlock *b = ell->block;

            /* Load an llvm::IRBuilder and point it to the correct
             * spot. */
            llvm::BasicBlock *block_marker = (*dgiter)->block_marker;
            llvm::Instruction *marker      = (*dgiter)->marker;
            llvm::IRBuilder<> builder(block_marker);
            builder.SetInsertPoint(block_marker);

            if (!((*dgiter)->marker)) {
                if (block_marker->size() == 0) {
                    builder.SetInsertPoint(block_marker);
                } else {
                    llvm::Instruction *fnp =
                        block_marker->getFirstNonPHI();
                    
                    /* Get the instruction after the first non-PHI
                     * node, and set that as the insertion point.
                     * If such an instruction doesn't exist, then
                     * the previous SetInsertPoint call will do
                     * the trick. */

                    llvm::BasicBlock::iterator bi, be;

                    for (bi = block_marker->begin(),
                            be = block_marker->end();
                            bi != be;
                            ++bi) {
                        llvm::Instruction *i = bi;
                        if (i == fnp) {
                            ++bi;
                            if (bi != be) {
                                builder.SetInsertPoint(bi);
                                break;
                            } else {
                                break;
                            }
                        }
                    }
                }
            } else {
                llvm::BasicBlock::iterator bi, be;

                for (bi = block_marker->begin(),
                        be = block_marker->end();
                        bi != be;
                        ++bi) {
                    llvm::Instruction *i = bi;
                    if (i == marker) {
                        ++bi;
                        if (bi != be) {
                            builder.SetInsertPoint(bi);
                            break;
                        } else {
                            break;
                        }
                    }
                }
            }

            /* Get the goto's namespace. */
            Namespace *goto_ns = (*dgiter)->ns;

            /* Create a vector of variables to destruct. This will
             * be the vector of all variables in the goto_namespace
             * and upwards, until either null (top of function) or
             * the other context is hit. Variables in the other
             * context are excluded. */
            std::vector<Element::Variable *> variables;
            Namespace *current_ns = goto_ns;
            while (current_ns != ell->ns) {
                current_ns->getVariables(&variables);
                current_ns = current_ns->parent_namespace;
                if (!current_ns) {
                    break;
                }
            }

            if (current_ns != ell->ns) {
                Namespace *ell_ns = ell->ns;
                /* Didn't hit the label's namespace on the way
                 * upwards. If the label's namespace, or any namespace
                 * in which the label's namespace is located, has a
                 * variable declaration with an index smaller than
                 * that of the label, then the goto is going to
                 * cross that variable declaration, in which case
                 * you want to bail. (This is because the
                 * declaration may result in a destructor being
                 * called on scope close, and the variable may not be
                 * initialised when the goto is reached.) */
                std::vector<Element::Variable *> vars_before;
                std::vector<Element::Variable *> real_vars_before;
                while (ell_ns) {
                    vars_before.clear();
                    ell_ns->getVarsBeforeIndex(ell->index, &vars_before);
                    if (vars_before.size() > 0) {
                        for
                        (std::vector<Element::Variable*>::iterator
                                vvb = vars_before.begin(),
                                vve = vars_before.end();
                                vvb != vve;
                                ++vvb) {
                            if ((*vvb)->index >= (*dgiter)->index) {
                                real_vars_before.push_back((*vvb));
                            }
                        }
                        if (real_vars_before.size() > 0) {
                            Error *e = new Error(
                                ErrorInst::Generator::GotoWillCrossDeclaration,
                                (*dgiter)->node
                            );
                            erep->addError(e);
                            res = 0;
                            goto finish;
                        }
                    }
                    ell_ns = ell_ns->parent_namespace;
                }
            }

            /* Add the destructors for the collected variables. */
            for (std::vector<Element::Variable *>::iterator
                    vb = variables.begin(),
                    ve = variables.end();
                    vb != ve;
                    ++vb) {
                Element::Variable *v = (*vb);
                ParseResult pr_variable;
                ParseResult temp;
                setPr(&pr_variable, NULL, v->type, v->value);
                bool res = destructIfApplicable(&pr_variable, &builder,
                                                &temp);
                if (!res) {
                    return 0;
                }
            }

            builder.CreateBr(b);
            delete ln;
            delete (*dgiter);

            ++dgiter;
        }
    }

    /* Iterate over the blocks in the function. If the block ends
     * in a terminator, all is well. If it doesn't: if the block
     * is the last block in the function, create a return
     * instruction containing the last value evaluated, otherwise
     * create a branch instruction that moves to the next block.
     */

    bcount = 1;
    bmax   = fn->size();

    for (llvm::Function::iterator i = fn->begin(), e = fn->end();
            i != e; ++i) {
        if ((i->size() == 0) || !(i->back().isTerminator())) {
            llvm::IRBuilder<> builder(i);

            /* The underlying error here will have been
             * reported earlier, if there is no
             * last_value. */
            if (bcount == bmax) {
                if (last_value) {
                    Element::Type *got_type = last_type;

                    if (!dfn->return_type->isEqualTo(got_type)) {
                        std::string gotstr;
                        got_type->toStringProper(&gotstr);
                        std::string expstr;
                        dfn->return_type->toStringProper(&expstr);
                        Error *e = new Error(
                            ErrorInst::Generator::IncorrectReturnType,
                            last_position,
                            expstr.c_str(), gotstr.c_str()
                        );
                        erep->addError(e);
                        res = 0;
                        goto finish;
                    }
                    if (dfn->return_type->base_type == Type::Void) {
                        scopeClose(dfn, i, NULL);
                        builder.CreateRetVoid();
                    } else {
                        ParseResult x;
                        setPr(&x, i, last_type, last_value);
                        scopeClose(dfn, x.block, NULL);
                        builder.CreateRet(x.value);
                    }
                } else {
                    scopeClose(dfn, i, NULL);
                    builder.CreateRetVoid();
                }
            } else {
                /* Get the next block and create a branch to it. */
                ++i;
                builder.CreateBr(i);
                --i;
            }
        }
        ++bcount;
    }

    /* Iterate over the blocks in the function. Delete all
     * instructions that occur after the first terminating
     * instruction. */

    for (llvm::Function::iterator i = fn->begin(), e = fn->end();
            i != e; ++i) {
        llvm::BasicBlock::iterator bi;
        llvm::BasicBlock::iterator be;

        for (bi = i->begin(), be = i->end(); bi != be; ++bi) {
            if ((*bi).isTerminator()) {
                ++bi;
                if (bi == be) {
                    break;
                }
                int count = 0;
                while (bi != be) {
                    count++;
                    ++bi;
                }
                while (count--) {
                    i->getInstList().pop_back();
                }
                break;
            }
        }
    }


finish:

    /* Clear deferred gotos and labels. For anonymous functions,
     * these are saved and restored in parseFunctionBodyInstr. */

    dfn->defgotos->clear();
    dfn->labels->clear();

    global_blocks.pop_back();
    if (global_blocks.size()) {
        global_block = global_blocks.back();
    } else {
        global_block = NULL;
    }

    return res;
}

void Generator::parseArgument(Element::Variable *var, Node *top,
                              bool allow_anon_structs,
                              bool allow_bitfields)
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
            var->type = type_void;
            return;
        } else if (!strcmp(t->str_value.c_str(), "...")) {
            var->type = type_varargs;
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

    /* parseType returns a newly allocated type - it is assumed
     * that Element::Variable has not initialised its type and
     * that memory is not going to leak here. */
    Element::Type *type = parseType((*lst)[1], allow_anon_structs,
                                    allow_bitfields);
    var->type = type;

    return;
}

int Generator::parseInteger(Node *n)
{
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "symbol", "integer", "list"
        );
        erep->addError(e);
        return -1;
    }
    if (n->token->type != TokenType::Int) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "integer", "literal", n->token->tokenType()
        );
        erep->addError(e);
        return -1;
    }

    char *endptr;
    unsigned long addnum =
        strtoul(n->token->str_value.c_str(), &endptr, DECIMAL_RADIX);

    if (STRTOUL_FAILED(addnum, n->token->str_value.c_str(), endptr)) {
        Error *e = new Error(
            ErrorInst::Generator::UnableToParseInteger,
            n,
            n->token->str_value.c_str()
        );
        erep->addError(e);
        return -1;
    }

    return addnum;
}

void mysplitString(std::string *str, std::vector<std::string> *lst, char c)
{
    int index = 0;
    int len = str->length();

    while (index < len) {
        int found = str->find(c, index);
        if (found == -1) {
            found = str->length();
        }
        std::string temp(str->substr(index, found - index));
        lst->push_back(temp);
        index = found + 1;
    }
}

Element::Type *Generator::parseType(Node *top,
                                    bool allow_anon_structs,
                                    bool allow_bitfields)
{
    if (!top) {
        return NULL;
    }

    if (top->is_token) {
        Token *t = top->token;

        if (t->type != TokenType::String) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectSingleParameterType,
                top,
                "symbol", t->tokenType()
            );
            erep->addError(e);
            return NULL;
        }

        const char *typs = t->str_value.c_str();

        Element::Type *mt =
              (!strcmp(typs, "int" ))        ? type_int
            : (!strcmp(typs, "void"))        ? type_void
            : (!strcmp(typs, "char"))        ? type_char
            : (!strcmp(typs, "bool"))        ? type_bool
            : (!strcmp(typs, "uint" ))       ? type_uint
            : (!strcmp(typs, "int8"))        ? type_int8
            : (!strcmp(typs, "uint8"))       ? type_uint8
            : (!strcmp(typs, "int16"))       ? type_int16
            : (!strcmp(typs, "uint16"))      ? type_uint16
            : (!strcmp(typs, "int32"))       ? type_int32
            : (!strcmp(typs, "uint32"))      ? type_uint32
            : (!strcmp(typs, "int64"))       ? type_int64
            : (!strcmp(typs, "uint64"))      ? type_uint64
            : (!strcmp(typs, "int128"))      ? type_int128
            : (!strcmp(typs, "uint128"))     ? type_uint128
            : (!strcmp(typs, "intptr"))      ? type_intptr
            : (!strcmp(typs, "size"))        ? type_size
            : (!strcmp(typs, "ptrdiff"))     ? type_ptrdiff
            : (!strcmp(typs, "float"))       ? type_float
            : (!strcmp(typs, "double"))      ? type_double
            : (!strcmp(typs, "long-double")) ? type_longdouble
                                             : NULL;

        if (mt) {
            if (!is_x86_64
                    && (mt->base_type == Type::Int128
                        || mt->base_type == Type::UInt128)) {
                Error *e = new Error(
                    ErrorInst::Generator::TypeNotSupported,
                    top,
                    typs
                );
                erep->addError(e);
                return NULL;
            }
            return mt;
        }

        /* Not a simple type - check if it is a struct. */

        Element::Struct *temp_struct;

        if ((temp_struct = ctx->getStruct(typs))) {
            std::string fqsn;
            bool b = ctx->setFullyQualifiedStructName(typs, &fqsn);
            if (!b) {
                fprintf(stderr, "Internal error: unable to set struct "
                                "name (%s).\n", typs);
                abort();
            }
            return tr->getStructType(fqsn);
        }

        Error *err = new Error(
            ErrorInst::Generator::TypeNotInScope,
            top,
            typs
        );
        erep->addError(err);
        return NULL;
    }

    // If here, node is a list node. First, try for a macro call.

    Node *newtop = parseOptionalMacroCall(top);

    if (newtop != top) {
        /* New node - re-call parseType. */
        return parseType(newtop, allow_anon_structs,
                         allow_bitfields);
    }

    symlist *lst = top->list;

    Node *n = (*lst)[0];

    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        erep->addError(e);
        return NULL;
    }

    Token *t = n->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            n
        );
        erep->addError(e);
        return NULL;
    }

    // If the first element is 'do', then skip that element.

    std::vector<Node*> templist;
    if (!(t->str_value.compare("do"))) {
        templist.assign(lst->begin() + 1, lst->end());
        lst = &templist;
        if (lst->size() == 1) {
            return parseType(lst->at(0), allow_anon_structs,
                             allow_bitfields);
        }
    }

    /* If list is a two-element list, where the first element is
     * 'struct', then this is an anonymous struct. If
     * allow_anon_structs is enabled, then construct a list that
     * can in turn be used to create that struct, call
     * parseStructDefinition, and then use that new struct name
     * for the value of this element. */

    if (allow_anon_structs
            && lst->size() == 2
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("struct"))) {
        Token *li = new Token(TokenType::String,0,0,0,0);
        li->str_value.append("extern");
        lst->insert((lst->begin() + 1), new Node(li));
        char buf[255];
        sprintf(buf, "__as%d", anonstructcount++);
        int error_count =
            erep->getErrorTypeCount(ErrorType::Error);

        parseStructDefinition(buf, new Node(lst));

        int error_post_count =
            erep->getErrorTypeCount(ErrorType::Error);
        if (error_count != error_post_count) {
            return NULL;
        }

        Token *name = new Token(TokenType::String,0,0,0,0);
        name->str_value.append(buf);
        Element::Type *myst = parseType(new Node(name), false,
                                        false);
        if (!myst) {
            fprintf(stderr, "Unable to retrieve anonymous struct.\n");
            abort();
        }
        return myst;
    }

    /* If list is a three-element list, where the first element is
     * 'bf', then this is a bitfield type. Only return such a type
     * if allow_bitfields is enabled. */

    if (allow_bitfields
            && lst->size() == 3
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("bf"))) {
        Element::Type *bf_type =
            parseType(lst->at(1), false, false);
        if (!(bf_type->isIntegerType())) {
            Error *e = new Error(
                ErrorInst::Generator::BitfieldMustHaveIntegerType,
                top
            );
            erep->addError(e);
            return NULL;
        }
        int size = parseInteger(lst->at(2));
        if (size == -1) {
            return NULL;
        }
        return tr->getBitfieldType(bf_type, size);
    }

    if (!strcmp(t->str_value.c_str(), "const")) {
        if (lst->size() != 2) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                top,
                "const", "1"
            );
            char buf[100];
            sprintf(buf, "%d", (int) lst->size() - 1);
            e->addArgString(buf);
            erep->addError(e);
            return NULL;
        }

        Node *newnum = parseOptionalMacroCall((*lst)[1]);
        if (!newnum) {
            return NULL;
        }

        Element::Type *const_type =
            parseType((*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (const_type == NULL) {
            return NULL;
        }

        return tr->getConstType(const_type);
    }

    if (!strcmp(t->str_value.c_str(), "array-of")) {
        if (lst->size() != 3) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                top,
                "array-of", "2"
            );
            char buf[100];
            sprintf(buf, "%d", (int) lst->size() - 1);
            e->addArgString(buf);
            erep->addError(e);
            return NULL;
        }

        Node *newnum = parseOptionalMacroCall((*lst)[1]);
        if (!newnum) {
            return NULL;
        }

        int size = parseInteger(newnum);
        if (size == -1) {
            return NULL;
        }

        Element::Type *array_type =
            parseType((*lst)[2], allow_anon_structs,
                      allow_bitfields);

        if (array_type == NULL) {
            return NULL;
        }

        Element::Type *type = tr->getArrayType(array_type, size);

        return type;
    }

    if (!strcmp(t->str_value.c_str(), "p")) {
        if (!assertArgNums("p", top, 1, 1)) {
            return NULL;
        }

        Element::Type *points_to_type =
            parseType((*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (points_to_type == NULL) {
            return NULL;
        }

        return tr->getPointerType(points_to_type);
    }

    if (!strcmp(t->str_value.c_str(), "fn")) {
        if (!assertArgNums("fn", top, 2, 2)) {
            return NULL;
        }

        Element::Type *ret_type =
            parseType((*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (ret_type == NULL) {
            return NULL;
        }
        if (ret_type->is_array) {
            Error *e = new Error(
                ErrorInst::Generator::ReturnTypesCannotBeArrays,
                n
            );
            erep->addError(e);
            return NULL;
        }

        Node *params = (*lst)[2];

        if (!params->is_list) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                n,
                "list", "fn parameters", "symbol"
            );
            erep->addError(e);
            return NULL;
        }

        symlist *plst = params->list;

        Element::Variable *var;

        std::vector<Element::Type *> *parameter_types =
            new std::vector<Element::Type *>;

        std::vector<Node *>::iterator node_iter;
        node_iter = plst->begin();

        while (node_iter != plst->end()) {
            var = new Element::Variable();
            var->type = NULL;

            parseArgument(var, (*node_iter),
                          allow_anon_structs,
                          allow_bitfields);

            if (var->type == NULL) {
                delete var;
                return NULL;
            }

            if (var->type->base_type == Type::Void) {
                delete var;
                if (plst->size() != 1) {
                    Error *e = new Error(
                        ErrorInst::Generator::VoidMustBeTheOnlyParameter,
                        params
                    );
                    erep->addError(e);
                    return NULL;
                }
                break;
            }

            /* Have to check that none come after this. */
            if (var->type->base_type == Type::VarArgs) {
                if ((plst->end() - node_iter) != 1) {
                    delete var;
                    Error *e = new Error(
                        ErrorInst::Generator::VarArgsMustBeLastParameter,
                        params
                    );
                    erep->addError(e);
                    return NULL;
                }
                parameter_types->push_back(var->type);
                break;
            }

            if (var->type->is_function) {
                delete var;
                Error *e = new Error(
                    ErrorInst::Generator::NonPointerFunctionParameter,
                    (*node_iter)
                );
                erep->addError(e);
                return NULL;
            }

            parameter_types->push_back(var->type);

            ++node_iter;
        }

        Element::Type *type = new Element::Type();
        type->is_function     = 1;
        type->return_type     = ret_type;
        type->parameter_types = parameter_types;
        return type;
    }

    Error *e = new Error(
        ErrorInst::Generator::InvalidType,
        top
    );
    erep->addError(e);

    return NULL;
}

llvm::Type *Generator::toLLVMType(Element::Type *type,
                                      Node *n,
                                      bool
                                      allow_non_first_class,
                                      bool
                                      externally_defined)
{
    return ctx->toLLVMType(type, n, allow_non_first_class,
                               externally_defined);
}

llvm::Type *Generator::toLLVMType(Element::Type *type,
        Node *n)
{
    return ctx->toLLVMType(type, n);
}

llvm::Value *Generator::coerceValue(llvm::Value *from_value,
                                    Element::Type *from_type,
                                    Element::Type *to_type,
                                    llvm::BasicBlock *block)
{
    int fa = from_type->is_array;
    int fb = (fa) ? from_type->array_type->base_type : 0;
    Element::Type *fp = from_type->points_to;
    Element::Type *tp = to_type->points_to;

    if (fb == Type::Char && fa && !fp) {
        if (tp && tp->base_type == Type::Char && !tp->points_to) {
            llvm::IRBuilder<> builder(block);

            llvm::Value *charpointer =
                builder.CreateGEP(
                    llvm::cast<llvm::Value>(from_value),
                    llvm::ArrayRef<llvm::Value*>(two_zero_indices));

            return charpointer;
        }
    }

    return NULL;
}

Node *Generator::DNodeToIntNode(DNode *dnode)
{
    Node *tempnode = new Node();
    tempnode->list_begin.line_number    = dnode->begin_line;
    tempnode->list_begin.column_number  = dnode->begin_column;
    tempnode->list_end.line_number      = dnode->end_line;
    tempnode->list_end.column_number    = dnode->end_column;
    tempnode->macro_begin.line_number   = dnode->macro_begin_line;
    tempnode->macro_begin.column_number = dnode->macro_begin_column;
    tempnode->macro_end.line_number     = dnode->macro_end_line;
    tempnode->macro_end.column_number   = dnode->macro_end_column;
    tempnode->filename = dnode->filename;

    if (!dnode->is_list) {
        Token *token = new Token(TokenType::Null,
                                 dnode->begin_line,
                                 dnode->begin_column,
                                 dnode->end_line,
                                 dnode->end_column);

        if (!dnode->token_str) {
            Error *e = new Error(
                ErrorInst::Generator::DNodeHasNoString,
                tempnode
            );
            erep->addError(e);
            delete token;
            return NULL;
        }
        if (strlen(dnode->token_str) == 0) {
            Error *e = new Error(
                ErrorInst::Generator::DNodeHasNoString,
                tempnode
            );
            erep->addError(e);
            delete token;
            return NULL;
        }

        char c = (dnode->token_str)[0];
        char d = (dnode->token_str)[1];
        int len = strlen(dnode->token_str);

        if ((((len > 1) && (c == '-') && (isdigit(d))))
                || (isdigit(c))) {
            /* Is an integer (or a float). */

            token->str_value.append(dnode->token_str);
            Node *n = new Node(token);
            n->macro_begin.line_number   = dnode->macro_begin_line;
            n->macro_begin.column_number = dnode->macro_begin_column;
            n->macro_end.line_number     = dnode->macro_end_line;
            n->macro_end.column_number   = dnode->macro_end_column;

            if (strchr(dnode->token_str, '.')) {
                if (!is_simple_float(dnode->token_str)) {
                    Error *e = new Error(
                        ErrorInst::Lexer::InvalidFloatingPointNumber,
                        n
                    );

                    erep->addError(e);
                    return NULL;
                } else {
                    token->type = TokenType::FloatingPoint;
                }
            } else {
                if (!is_simple_int(dnode->token_str)) {
                    Error *e = new Error(
                        ErrorInst::Lexer::InvalidInteger,
                        n
                    );
                    erep->addError(e);
                    return NULL;
                } else {
                    token->type = TokenType::Int;
                }
            }
            n->filename = dnode->filename;
            return n;
        }

        if ((c == '"') &&
                (dnode->token_str)[strlen(dnode->token_str)-1] == '"') {
            /* Is a string literal. */

            char str[256];
            str[0] = '\0';

            strncpy(str, (dnode->token_str) + 1, (strlen(dnode->token_str)-2));
            str[strlen(dnode->token_str)-2] = '\0';

            token->type = TokenType::StringLiteral;
            token->str_value.append(str);

            Node *n = new Node(token);
            n->macro_begin.line_number   = dnode->macro_begin_line;
            n->macro_begin.column_number = dnode->macro_begin_column;
            n->macro_end.line_number     = dnode->macro_end_line;
            n->macro_end.column_number   = dnode->macro_end_column;
            n->filename = dnode->filename;
            return n;
        }

        /* If neither - is plain string. */

        token->type = TokenType::String;
        token->str_value.append(dnode->token_str);

        Node *mynode = new Node(token);
        mynode->macro_begin.line_number   = dnode->macro_begin_line;
        mynode->macro_begin.column_number = dnode->macro_begin_column;
        mynode->macro_end.line_number     = dnode->macro_end_line;
        mynode->macro_end.column_number   = dnode->macro_end_column;
        mynode->filename = dnode->filename;
        return mynode;
    }

    /* DNode is list. */
    if (dnode->is_list) {
        std::vector<Node *> *list = new std::vector<Node *>;

        DNode *current_node = dnode->list_node;

        while (current_node) {
            Node *new_node = DNodeToIntNode(current_node);
            list->push_back(new_node);
            current_node = current_node->next_node;
        }

        Node *final_node = new Node(list);
        final_node->filename = dnode->filename;
        final_node->list_begin.line_number    = dnode->begin_line;
        final_node->list_begin.column_number  = dnode->begin_column;
        final_node->list_end.line_number      = dnode->end_line;
        final_node->list_end.column_number    = dnode->end_column;
        final_node->macro_begin.line_number   = dnode->macro_begin_line;
        final_node->macro_begin.column_number = dnode->macro_begin_column;
        final_node->macro_end.line_number     = dnode->macro_end_line;
        final_node->macro_end.column_number   = dnode->macro_end_column;
        return final_node;
    }

    Error *e = new Error(
        ErrorInst::Generator::DNodeIsNeitherTokenNorList,
        tempnode
    );
    erep->addError(e);

    return NULL;
}

DNode *Generator::IntNodeToDNode(Node *node)
{
    /* If node is token - allocate single dnode and return it. */

    if (node->is_token) {
        Token *t     = node->token;
        DNode *dnode = (DNode*)malloc(sizeof(*dnode));

        std::string ttostr;
        t->valueToString(&ttostr);

        char *sv = (char*)malloc(ttostr.length() + 1);

        strncpy(sv, ttostr.c_str(), ttostr.length()+1);

        dnode->is_list   = 0;
        dnode->token_str = sv;
        dnode->list_node = NULL;
        dnode->next_node = NULL;

        dnode->begin_line   = node->getBeginPos()->getLineNumber();
        dnode->begin_column = node->getBeginPos()->getColumnNumber();
        dnode->end_line     = node->getEndPos()->getLineNumber();
        dnode->end_column   = node->getEndPos()->getColumnNumber();
        if (node->macro_begin.getLineNumber()) {
            dnode->macro_begin_line = node->macro_begin.getLineNumber();
            dnode->macro_begin_column =
                node->macro_begin.getColumnNumber();
            dnode->macro_end_line = node->macro_end.getLineNumber();
            dnode->macro_end_column =
                node->macro_end.getColumnNumber();
        }
        dnode->filename = node->filename;

        return dnode;
    }

    /* If node is list - for each element, call self, link the
     * nodes together. */

    if (node->is_list) {
        DNode *top_node = (DNode*)malloc(sizeof(*top_node));
        top_node->is_list   = 1;
        top_node->token_str = NULL;
        top_node->next_node = NULL;

        DNode *current_dnode = NULL;

        symlist *lst = node->list;

        std::vector<Node *>::iterator node_iter;
        node_iter = lst->begin();

        while (node_iter != lst->end()) {
            DNode *temp_node = IntNodeToDNode((*node_iter));

            if (!current_dnode) {
                top_node->list_node = temp_node;
                current_dnode = temp_node;
            } else {
                current_dnode->next_node = temp_node;
                current_dnode            = temp_node;
            }

            ++node_iter;
        }

        top_node->begin_line   = node->getBeginPos()->getLineNumber();
        top_node->begin_column = node->getBeginPos()->getColumnNumber();
        top_node->end_line     = node->getEndPos()->getLineNumber();
        top_node->end_column   = node->getEndPos()->getColumnNumber();

        if (node->macro_begin.getLineNumber()) {
            top_node->macro_begin_line = node->macro_begin.getLineNumber();
            top_node->macro_begin_column =
                node->macro_begin.getColumnNumber();
            top_node->macro_end_line = node->macro_end.getLineNumber();
            top_node->macro_end_column =
                node->macro_end.getColumnNumber();
        }
        top_node->filename = node->filename;

        return top_node;
    }

    Error *e = new Error(
        ErrorInst::Generator::NodeIsNeitherTokenNorList,
        node
    );
    erep->addError(e);

    return NULL;
}

void Generator::deleteDNode(DNode *dnode)
{
    if (!dnode) {
        return;
    }

    /* If node is token - free the token string and free the dnode. */

    if (!dnode->is_list) {
        free(dnode->token_str);

        if (dnode->next_node) {
            deleteDNode(dnode->next_node);
        }

        free(dnode);

        return;
    }

    /* If node is list - for each element, call self, link the
     * nodes together. */

    if (dnode->is_list) {
        deleteDNode(dnode->list_node);

        if (dnode->next_node) {
            deleteDNode(dnode->next_node);
        }

        free(dnode);

        return;
    }

    Node *tempnode = new Node();
    tempnode->list_begin = new Position(dnode->begin_line,
                                        dnode->begin_column);
    tempnode->list_end = new Position(dnode->end_line,
                                      dnode->end_column);

    Error *e = new Error(
        ErrorInst::Generator::DNodeIsNeitherTokenNorList,
        tempnode
    );
    erep->addError(e);

    return;
}

Node *Generator::typeToIntNode(Element::Type *type)
{
    if (type->is_array) {
        return NULL;
    }

    if (type->points_to) {
        Node *n = typeToIntNode(type->points_to);

        std::vector <Node*> *nodes = new std::vector <Node*>;

        Token *t1 = new Token(TokenType::String, 0,0,0,0);
        t1->str_value.clear();
        t1->str_value.append("p");
        Node *n1 = new Node(t1);

        nodes->push_back(n1);
        nodes->push_back(n);

        Node *nh = new Node(nodes);
        return nh;
    }

    const char *btstr = dale::Element::baseTypeToString(type->base_type);
    if (strcmp(btstr, "[unknown]")) {
        Token *t = new Token(TokenType::String, 0,0,0,0);
        t->str_value.clear();
        t->str_value.append(btstr);
        Node *n = new Node(t);
        return n;
    }

    if (type->struct_name != NULL) {
        Token *t = new Token(TokenType::String, 0,0,0,0);
        t->str_value.clear();
        t->str_value.append(*(type->struct_name));
        Node *n = new Node(t);
        return n;
    }

    return NULL;
}

std::map<std::string, llvm::GlobalVariable*> string_cache;

llvm::Value *Generator::IntNodeToStaticDNode(Node *node,
        llvm::Value
        *next_node)
{
    if (!node) {
        fprintf(stderr, "Internal error: null node passed to "
                "IntNodeToStaticNode.\n");
        abort();
    }

    /* If it's one node, add the dnode. */
    std::string varname;
    getUnusedVarname(&varname);

    /* Add the variable to the module. */

    setPdnode();

    llvm::Type *llvm_type = llvm_type_dnode;
    llvm::Type *llvm_r_type = llvm_type_pdnode;

    llvm::GlobalVariable *var =
        llvm::cast<llvm::GlobalVariable>(
            mod->getOrInsertGlobal(varname.c_str(), llvm_type)
        );

    var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    std::vector<llvm::Constant *> constants;
    llvm::Constant *first =
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->is_list)
        );
    constants.push_back(first);

    if (!node->is_list) {
        Token *t = node->token;
        size_t pos = 0;
        while ((pos = t->str_value.find("\\n", pos)) != std::string::npos) {
            t->str_value.replace(pos, 2, "\n");
        }
        if (t->type == TokenType::StringLiteral) {
            t->str_value.insert(0, "\"");
            t->str_value.push_back('"');
        }

        /* If there is an entry in the cache for this string, and
         * the global variable in the cache belongs to the current
         * module, then use that global variable. */

        llvm::GlobalVariable *svar2 = NULL;

        std::map<std::string, llvm::GlobalVariable*>::iterator f
        = string_cache.find(t->str_value);
        if (f != string_cache.end()) {
            llvm::GlobalVariable *temp = f->second;
            if (temp->getParent() == mod) {
                svar2 = temp;
            }
        }

        if (!svar2) {
            llvm::Constant *arr =
                llvm::ConstantArray::get(llvm::getGlobalContext(),
                                         t->str_value.c_str(),
                                         true);
            std::string varname2;
            getUnusedVarname(&varname2);

            Element::Type *archar = 
                tr->getArrayType(type_char,
                                 t->str_value.size() + 1);

            svar2 =
                llvm::cast<llvm::GlobalVariable>(
                    mod->getOrInsertGlobal(varname2.c_str(),
                                           toLLVMType(archar, NULL, false))
                );

            svar2->setInitializer(arr);
            svar2->setConstant(true);
            svar2->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

            string_cache.insert(std::pair<std::string,
                                llvm::GlobalVariable*>(
                                    t->str_value,
                                    svar2
                                ));
        }

        llvm::Value *temps[2];
        temps[0] = llvm::ConstantInt::get(nt->getNativeIntType(), 0);
        temps[1] = llvm::ConstantInt::get(nt->getNativeIntType(), 0);

        llvm::Constant *pce =
            llvm::ConstantExpr::getGetElementPtr(
                llvm::cast<llvm::Constant>(svar2),
                temps,
                2
            );

        constants.push_back(pce);
    } else {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(
                        toLLVMType(type_pchar, NULL, false)
                    )
                )
            )
        );
    }

    if (node->is_list) {
        std::vector<Node *> *list = node->list;
        std::vector<Node *>::reverse_iterator list_iter = list->rbegin();
        llvm::Value *sub_next_node = NULL;

        while (list_iter != list->rend()) {
            llvm::Value *temp_value =
                IntNodeToStaticDNode((*list_iter), sub_next_node);
            sub_next_node = temp_value;
            ++list_iter;
        }

        constants.push_back(
            llvm::cast<llvm::Constant>(
                sub_next_node
            )
        );
    } else {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(
                        llvm_r_type
                    )
                )
            )
        );
    }

    if (next_node) {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                next_node
            )
        );
    } else {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(
                        llvm_r_type
                    )
                )
            )
        );
    }

    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->getBeginPos()->line_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->getBeginPos()->column_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->getEndPos()->line_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->getEndPos()->column_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->macro_begin.line_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->macro_begin.column_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->macro_end.line_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantInt::get(nt->getNativeIntType(),
                                   node->macro_end.column_number)
        )
    );
    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantPointerNull::get(
                llvm::cast<llvm::PointerType>(
                    toLLVMType(type_pchar, NULL, false)
                )
            )
        )
    );
    llvm::StructType *st =
        llvm::cast<llvm::StructType>(llvm_type);
    llvm::Constant *init =
        llvm::ConstantStruct::get(
            st,
            constants
        );
    var->setInitializer(init);

    var->setConstant(true);

    return llvm::cast<llvm::Value>(var);
}
}
