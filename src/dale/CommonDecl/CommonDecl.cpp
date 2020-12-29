#include "CommonDecl.h"

#include <float.h>
#include <setjmp.h>

#include <cstdio>
#include <string>
#include <vector>

#include "../BasicTypes/BasicTypes.h"
#include "../llvmUtils/llvmUtils.h"

#define BT_SI(t) \
    BasicTypes::addSignedInt(ctx, mod, &current_once_tag, t);
#define BT_UI(t) \
    BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, t);
#define BT_FP(t) \
    BasicTypes::addFloatingPoint(ctx, mod, &current_once_tag, t);

#define AV_INT(n, v) \
    addVariable(unit, n, type_int, nt->getNativeInt(v));
#define CFP_FLOAT(v) \
    llvm::ConstantFP::get(llvm::Type::getFloatTy(*getContext()), v)
#define CFP_DBL(v) \
    llvm::ConstantFP::get(llvm::Type::getDoubleTy(*getContext()), v)
#define CFP_FP80(v) \
    llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(*getContext()), v)
#define CFP_INF(v) \
    llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(*getContext()), v)

namespace dale {
namespace CommonDecl {
bool addVariable(Unit *unit, const char *name, Type *type,
                 llvm::Constant *init) {
    Context *ctx = unit->ctx;
    llvm::Module *mod = unit->module;

    Variable *var = new Variable();
    var->name.append(name);
    var->type = type;
    var->symbol.append(name);
    var->linkage = Linkage::Extern;
    bool res = ctx->addVariable(name, var);
    assert(res);
    _unused(res);

    llvm::Type *var_type = ctx->toLLVMType(type, NULL, false, false);
    if (!var_type) {
        return false;
    }

    llvm::GlobalVariable *llvm_var = llvm::cast<llvm::GlobalVariable>(
        mod->getOrInsertGlobal(name, var_type));

    llvm_var->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_Weak));
    llvm_var->setInitializer(init);
    var->value = llvm_var;

    return true;
}

void addVarargsFunctions(Unit *unit) {
    llvm::Module *mod = unit->module;
    Context *ctx = unit->ctx;
    Type *type_pchar = ctx->tr->type_pchar;

    std::vector<llvm::Type *> va_start_args;
    va_start_args.push_back(ctx->toLLVMType(type_pchar, NULL, false));

    llvm::FunctionType *va_start_ft =
        getFunctionType(ctx->toLLVMType(ctx->tr->type_void, NULL, true),
                        va_start_args, false);

    llvm::Function *va_start_fn = llvm::Function::Create(
        va_start_ft, ctx->toLLVMLinkage(Linkage::Extern_C),
        "llvm.va_start", mod);

    va_start_fn->setCallingConv(llvm::CallingConv::C);

    llvm::Function *va_end_fn = llvm::Function::Create(
        va_start_ft, ctx->toLLVMLinkage(Linkage::Extern_C),
        "llvm.va_end", mod);

    va_end_fn->setCallingConv(llvm::CallingConv::C);

    return;
}

void addBasicTypes(Unit *unit, bool is_x86_64) {
    Context *ctx = unit->ctx;
    llvm::Module *mod = unit->module;
    std::string current_once_tag = unit->once_tag;
    TypeRegister *tr = ctx->tr;

    BT_SI(tr->type_int);
    BT_SI(tr->type_char);
    BT_SI(tr->type_int8);
    BT_SI(tr->type_int16);
    BT_SI(tr->type_int32);
    BT_SI(tr->type_int64);

    BT_UI(tr->type_uint);
    BT_UI(tr->type_intptr);
    BT_UI(tr->type_ptrdiff);
    BT_UI(tr->type_size);
    BT_UI(tr->type_uint8);
    BT_UI(tr->type_uint16);
    BT_UI(tr->type_uint32);
    BT_UI(tr->type_uint64);

    /* i128 (actually any integer type with a size of more than 64
     * bits) does not work properly in some respects on x86-32 (see
     * http://llvm.org/bugs/show_bug.cgi?id=2660).  Rather than
     * allowing the hobbled support to be had, disable it completely.
     * */

    if (is_x86_64) {
        BT_SI(tr->type_int128);
        BT_UI(tr->type_uint128);
    }

    BT_FP(tr->type_float);
    BT_FP(tr->type_double);
    BT_FP(tr->type_longdouble);
}

void addVarargsTypes(Unit *unit, bool is_x86_64) {
    Parser *prsr = unit->parser;

    const char *definition = (is_x86_64)
                                 ? "(def va-list "
                                   "(struct extern ((a uint32) "
                                   "(b uint32) "
                                   "(c (p char)) "
                                   "(d (p char)))))"
                                 : "(def va-list "
                                   "(struct extern ((a uint32))))";

    prsr->getLexer()->pushText(definition);
}

void addStandardVariables(Unit *unit) {
    Context *ctx = unit->ctx;
    NativeTypes *nt = ctx->nt;

    Type *type_int = ctx->tr->type_int;
    Type *type_float = ctx->tr->type_float;
    Type *type_double = ctx->tr->type_double;
    Type *type_ldbl = ctx->tr->type_longdouble;

    AV_INT("JMP_BUF_SIZE", sizeof(jmp_buf));
    AV_INT("FPOS_T", sizeof(fpos_t));
    AV_INT("TIME_T", sizeof(time_t));
    AV_INT("CLOCK_T", sizeof(clock_t));
    AV_INT("SIZEOF_SHORT", sizeof(short));          // NOLINT
    AV_INT("SIZEOF_LONG", sizeof(long));            // NOLINT
    AV_INT("SIZEOF_LONG_LONG", sizeof(long long));  // NOLINT

    AV_INT("FLT_RADIX", FLT_RADIX);
    AV_INT("FLT_MANT_DIG", FLT_MANT_DIG);
    AV_INT("FLT_DIG", FLT_DIG);
    AV_INT("FLT_ROUNDS", FLT_ROUNDS);
    AV_INT("FLT_MIN_EXP", FLT_MIN_EXP);
    AV_INT("FLT_MAX_EXP", FLT_MAX_EXP);
    AV_INT("LDBL_MANT_DIG", LDBL_MANT_DIG);
    AV_INT("LDBL_DIG", LDBL_DIG);
    AV_INT("DBL_MANT_DIG", DBL_MANT_DIG);
    AV_INT("DBL_DIG", DBL_DIG);
    AV_INT("DBL_MIN_EXP", DBL_MIN_EXP);
    AV_INT("DBL_MAX_EXP", DBL_MAX_EXP);
    AV_INT("LDBL_MIN_EXP", LDBL_MIN_EXP);
    AV_INT("LDBL_MAX_EXP", LDBL_MAX_EXP);
    AV_INT("L_tmpnam", L_tmpnam);
    AV_INT("TMP_MAX", TMP_MAX);
    AV_INT("FILENAME_MAX", FILENAME_MAX);
    AV_INT("FOPEN_MAX", FOPEN_MAX);
    AV_INT("RAND_MAX", RAND_MAX);
    AV_INT("EXIT_FAILURE", EXIT_FAILURE);
    AV_INT("EXIT_SUCCESS", EXIT_SUCCESS);

    addVariable(unit, "FLT_EPSILON", type_float,
                CFP_FLOAT(FLT_EPSILON));
    addVariable(unit, "FLT_MIN", type_float, CFP_FLOAT(FLT_MIN));
    addVariable(unit, "FLT_MAX", type_float, CFP_FLOAT(FLT_MAX));
    addVariable(unit, "DBL_EPSILON", type_double, CFP_DBL(DBL_EPSILON));
    addVariable(unit, "DBL_MIN", type_double, CFP_DBL(DBL_MIN));
    addVariable(unit, "DBL_MAX", type_double, CFP_DBL(DBL_MAX));
    addVariable(unit, "LDBL_EPSILON", type_ldbl,
                CFP_FP80(LDBL_EPSILON));
    addVariable(unit, "LDBL_MIN", type_ldbl, CFP_FP80(LDBL_MIN));
    addVariable(unit, "LDBL_MAX", type_ldbl, CFP_FP80(LDBL_MAX));

    addVariable(unit, "HUGE_VAL", type_double,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getDoubleTy(*getContext())));

    addVariable(unit, "HUGE_VALF", type_float,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getFloatTy(*getContext())));

    addVariable(unit, "HUGE_VALL", type_ldbl,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getX86_FP80Ty(*getContext())));

    return;
}
}
}
