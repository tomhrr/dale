#include "CommonDecl.h"

#include "../BasicTypes/BasicTypes.h"
#include <setjmp.h>
#include <float.h>

#define BT_SI(t)  BasicTypes::addSignedInt(ctx, mod, &current_once_tag, t);
#define BT_UI(t)  BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, t);
#define BT_FP(t)  BasicTypes::addFloatingPoint(ctx, mod, &current_once_tag, t);

#define AV_INT(n, v) addVariable(unit, n, type_int, nt->getNativeInt(v));

namespace dale
{
namespace CommonDecl
{
llvm::FunctionType*
getFunctionType(llvm::Type *t,
                std::vector<llvm::Type*> &v,
                bool b) 
{
    llvm::ArrayRef<llvm::Type*> temp(v);
    return llvm::FunctionType::get(t, temp, b);
}

bool
addVariable(Unit *unit,
            const char *name,
            Element::Type *type,
            llvm::Constant *init)
{
    Context *ctx      = unit->ctx;
    llvm::Module *mod = unit->module;

    Element::Variable *var = new Element::Variable();
    var->name.append(name);
    var->type = type;
    var->internal_name.append(name);
    var->linkage = Linkage::Extern;
    int avres = ctx->ns()->addVariable(name, var);
    if (!avres) {
        fprintf(stderr, "Unable to add %s.\n", name);
        abort();
    }

    llvm::Type *rdttype =
        ctx->toLLVMType(type, NULL, false, false);
    if (!rdttype) {
        return false;
    }

    llvm::GlobalVariable *lvar =
        llvm::cast<llvm::GlobalVariable>(
            mod->getOrInsertGlobal(name,
                                   rdttype)
        );

    lvar->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_Weak));
    lvar->setInitializer(init);
    var->value = lvar;

    return true;
}

void 
addVarargsFunctions(Unit *unit) 
{
    llvm::Module *mod = unit->module;
    Context *ctx = unit->ctx;
    Element::Type *type_pchar =
        ctx->tr->getPointerType(ctx->tr->getBasicType(Type::Char)); 

    std::vector<llvm::Type*> va_start_args;
    va_start_args.push_back(ctx->toLLVMType(type_pchar, NULL, false));

    llvm::FunctionType *va_start_ft =
        getFunctionType(
            ctx->toLLVMType(ctx->tr->getBasicType(Type::Void), NULL, true),
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

void 
addBasicTypes(Unit *unit, bool is_x86_64)
{
    Context *ctx      = unit->ctx;
    llvm::Module *mod = unit->module;
    std::string current_once_tag = unit->once_tag;
    TypeRegister *tr = ctx->tr;

    BT_SI(tr->getBasicType(Type::Int));
    BT_SI(tr->getBasicType(Type::Char));
    BT_SI(tr->getBasicType(Type::Int8));
    BT_SI(tr->getBasicType(Type::Int16));
    BT_SI(tr->getBasicType(Type::Int32));
    BT_SI(tr->getBasicType(Type::Int64));

    BT_UI(tr->getBasicType(Type::UInt));
    BT_UI(tr->getBasicType(Type::IntPtr));
    BT_UI(tr->getBasicType(Type::PtrDiff));
    BT_UI(tr->getBasicType(Type::Size));
    BT_UI(tr->getBasicType(Type::UInt8));
    BT_UI(tr->getBasicType(Type::UInt16));
    BT_UI(tr->getBasicType(Type::UInt32));
    BT_UI(tr->getBasicType(Type::UInt64));

    /* i128 (actually any integer type with a size of more than 64
     * bits) does not work properly in some respects on x86-32
     * (see http://llvm.org/bugs/show_bug.cgi?id=2660). Rather
     * than allowing the hobbled support to be had, disable it
     * completely. */

    if (is_x86_64) {
        BT_SI(tr->getBasicType(Type::Int128));
        BT_UI(tr->getBasicType(Type::UInt128));
    }

    BT_FP(tr->getBasicType(Type::Float));
    BT_FP(tr->getBasicType(Type::Double));
    BT_FP(tr->getBasicType(Type::LongDouble));
}

void 
addVarargsTypes(Unit *unit, bool is_x86_64)
{
    Parser *prsr = unit->parser;

    const char *definition =
        (is_x86_64)
            ? "(def va-list "
                "(struct extern ((a uint32) "
                                "(b uint32) "
                                "(c (p char)) "
                                "(d (p char)))))"
            : "(def va-list "
                "(struct extern ((a uint32))))";

    prsr->lxr->pushText(definition);
}

void 
addStandardVariables(Unit *unit)
{
    Context *ctx = unit->ctx;
    NativeTypes *nt = ctx->nt;
    
    Element::Type *type_int        = ctx->tr->getBasicType(Type::Int);
    Element::Type *type_float      = ctx->tr->getBasicType(Type::Float);
    Element::Type *type_double     = ctx->tr->getBasicType(Type::Double);
    Element::Type *type_longdouble = ctx->tr->getBasicType(Type::LongDouble);

    AV_INT("JMP_BUF_SIZE",     sizeof (jmp_buf));
    AV_INT("FPOS_T",           sizeof (fpos_t));
    AV_INT("TIME_T",           sizeof (time_t));
    AV_INT("CLOCK_T",          sizeof (clock_t));
    AV_INT("SIZEOF_LONG",      sizeof (long));
    AV_INT("SIZEOF_LONG_LONG", sizeof (long long));

    AV_INT("FLT_RADIX",     FLT_RADIX);
    AV_INT("FLT_MANT_DIG",  FLT_MANT_DIG);
    AV_INT("FLT_DIG",       FLT_DIG);
    AV_INT("FLT_ROUNDS",    FLT_ROUNDS);
    AV_INT("FLT_MIN_EXP",   FLT_MIN_EXP);
    AV_INT("FLT_MAX_EXP",   FLT_MAX_EXP);
    AV_INT("LDBL_MANT_DIG", LDBL_MANT_DIG);
    AV_INT("LDBL_DIG",      LDBL_DIG);
    AV_INT("DBL_MANT_DIG",  DBL_MANT_DIG);
    AV_INT("DBL_DIG",       DBL_DIG);
    AV_INT("DBL_MIN_EXP",   DBL_MIN_EXP);
    AV_INT("DBL_MAX_EXP",   DBL_MAX_EXP);
    AV_INT("LDBL_MIN_EXP",  LDBL_MIN_EXP);
    AV_INT("LDBL_MAX_EXP",  LDBL_MAX_EXP);
    AV_INT("L_tmpnam",      L_tmpnam);
    AV_INT("TMP_MAX",       TMP_MAX);
    AV_INT("FILENAME_MAX",  FILENAME_MAX);
    AV_INT("FOPEN_MAX",     FOPEN_MAX);
    AV_INT("RAND_MAX",      RAND_MAX);
    AV_INT("EXIT_FAILURE",  EXIT_FAILURE);
    AV_INT("EXIT_SUCCESS",  EXIT_SUCCESS);

    addVariable(unit, "FLT_EPSILON",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(
                                      llvm::getGlobalContext()),
                                      FLT_EPSILON));
    addVariable(unit, "FLT_MIN",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(
                                      llvm::getGlobalContext()),
                                      FLT_MIN));
    addVariable(unit, "FLT_MAX",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(
                                      llvm::getGlobalContext()),
                                      FLT_MAX));

    addVariable(unit, "DBL_EPSILON",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(
                                      llvm::getGlobalContext()),
                                      DBL_EPSILON));
    addVariable(unit, "DBL_MIN",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(
                                      llvm::getGlobalContext()),
                                      DBL_MIN));
    addVariable(unit, "DBL_MAX",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(
                                      llvm::getGlobalContext()),
                                      DBL_MAX));

    addVariable(unit, "LDBL_EPSILON",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(
                                      llvm::getGlobalContext()),
                                      LDBL_EPSILON));
    addVariable(unit, "LDBL_MIN",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(
                                      llvm::getGlobalContext()),
                                      LDBL_MIN));
    addVariable(unit, "LDBL_MAX",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(
                                      llvm::getGlobalContext()),
                                      LDBL_MAX));

    addVariable(unit, "HUGE_VAL",
                type_double,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getDoubleTy(llvm::getGlobalContext())
                ));

    addVariable(unit, "HUGE_VALF",
                type_float,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getFloatTy(llvm::getGlobalContext())
                ));

    addVariable(unit, "HUGE_VALL",
                type_longdouble,
                llvm::ConstantFP::getInfinity(
                    llvm::Type::getX86_FP80Ty(llvm::getGlobalContext())
                ));

    return;
}
}
}
