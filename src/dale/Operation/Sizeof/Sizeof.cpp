#include "Sizeof.h"

namespace dale
{
namespace Operation
{
namespace Sizeof
{
bool 
execute(Context *ctx,
        llvm::BasicBlock *block,
        Type *type,
        ParseResult *pr)
{
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type =
        ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return false;
    }

    llvm::PointerType *lpt =
        llvm::PointerType::getUnqual(llvm_type);

    llvm::Value *res =
        builder.CreateGEP(
            llvm::ConstantPointerNull::get(lpt),
            ctx->nt->getNativeInt(1)
        );

    llvm::Value *res2 =
        builder.CreatePtrToInt(res, ctx->nt->getNativeSizeType());

    pr->set(block, ctx->tr->type_size, res2);
    return true;
}

llvm::FunctionType *
getFunctionType(llvm::Type *t,
                std::vector<llvm::Type*> &v,
                bool b) {
    llvm::ArrayRef<llvm::Type*> temp(v);
    return llvm::FunctionType::get(t, temp, b);
}

static int function_count = 0;

size_t 
get(Unit *unit,
    Type *type)
{
    Context *ctx = unit->ctx;

    llvm::Type *llvm_return_type =
        ctx->toLLVMType(ctx->tr->type_size, NULL, false);
    if (!llvm_return_type) {
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
    sprintf(buf, "_function_sizeof_%d", function_count++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (unit->module->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        unit->module->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    std::vector<Variable*> args;

    llvm::Function *fn = llvm::cast<llvm::Function>(fnc);

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", fn);
    ParseResult mine;
    bool mres = execute(ctx, block, type, &mine);
    if (!mres) {
        return 0;
    }

    llvm::IRBuilder<> builder(block);
    builder.CreateRet(mine.value);

    void* fptr =
        unit->ee->getPointerToFunction(fn);
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
}
}
}
