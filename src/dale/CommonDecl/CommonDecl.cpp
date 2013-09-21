#include "CommonDecl.h"

#include "../BasicTypes/BasicTypes.h"
#include <setjmp.h>
#include <float.h>

#define BT_SI(t) BasicTypes::addSignedInt(ctx, mod, &current_once_tag, t);
#define BT_UI(t) BasicTypes::addUnsignedInt(ctx, mod, &current_once_tag, t);
#define BT_FP(t) BasicTypes::addFloatingPoint(ctx, mod, &current_once_tag, t);

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

    /* Add jmp_buf size constant. */

    addVariable(unit, "JMP_BUF_SIZE",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(jmp_buf)));

    /* Sizeof fpos_t. */

    addVariable(unit, "FPOS_T",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(fpos_t)));

    /* Sizeof time_t. */

    addVariable(unit, "TIME_T",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(time_t)));

    /* Sizeof clock_t. */

    addVariable(unit, "CLOCK_T",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(clock_t)));

    /* Sizeof long. */

    addVariable(unit, "SIZEOF_LONG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(long)));

    /* Sizeof long long. */

    addVariable(unit, "SIZEOF_LONG_LONG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       sizeof(long long)));

    /* Add float.h constants. */

    addVariable(unit, "FLT_RADIX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_RADIX));
    addVariable(unit, "FLT_MANT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_MANT_DIG));
    addVariable(unit, "FLT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_DIG));
    addVariable(unit, "FLT_EPSILON",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(llvm::getGlobalContext()),
                                      FLT_EPSILON));
    addVariable(unit, "FLT_ROUNDS",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_ROUNDS));
    addVariable(unit, "FLT_MIN_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_MIN_EXP));
    addVariable(unit, "FLT_MAX_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FLT_MAX_EXP));
    addVariable(unit, "FLT_MIN",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(llvm::getGlobalContext()),
                                      FLT_MIN));
    addVariable(unit, "FLT_MAX",
                type_float,
                llvm::ConstantFP::get(llvm::Type::getFloatTy(llvm::getGlobalContext()),
                                      FLT_MAX));

    addVariable(unit, "DBL_MANT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_MANT_DIG));
    addVariable(unit, "DBL_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_DIG));
    addVariable(unit, "DBL_EPSILON",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                                      DBL_EPSILON));
    addVariable(unit, "DBL_MIN_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_MIN_EXP));
    addVariable(unit, "DBL_MAX_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       DBL_MAX_EXP));
    addVariable(unit, "DBL_MIN",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                                      DBL_MIN));
    addVariable(unit, "DBL_MAX",
                type_double,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                                      DBL_MAX));

    addVariable(unit, "LDBL_MANT_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_MANT_DIG));
    addVariable(unit, "LDBL_DIG",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_DIG));
    addVariable(unit, "LDBL_EPSILON",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(llvm::getGlobalContext()),
                                      LDBL_EPSILON));
    addVariable(unit, "LDBL_MIN_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_MIN_EXP));
    addVariable(unit, "LDBL_MAX_EXP",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       LDBL_MAX_EXP));
    addVariable(unit, "LDBL_MIN",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(llvm::getGlobalContext()),
                                      LDBL_MIN));
    addVariable(unit, "LDBL_MAX",
                type_longdouble,
                llvm::ConstantFP::get(llvm::Type::getX86_FP80Ty(llvm::getGlobalContext()),
                                      LDBL_MAX));

    /* Add floating-point infinity constants for float, double and
     * long-double. */

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

    /* Other misc. constants. */

    addVariable(unit, "L_tmpnam",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       L_tmpnam));
    addVariable(unit, "TMP_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       TMP_MAX));
    addVariable(unit, "FILENAME_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FILENAME_MAX));
    addVariable(unit, "FOPEN_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       FOPEN_MAX));
    addVariable(unit, "RAND_MAX",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       RAND_MAX));
    addVariable(unit, "EXIT_FAILURE",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       EXIT_FAILURE));
    addVariable(unit, "EXIT_SUCCESS",
                type_int,
                llvm::ConstantInt::get(nt->getNativeIntType(),
                                       EXIT_SUCCESS));

    return;
}
}
}
