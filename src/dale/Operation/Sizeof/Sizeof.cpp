#include "Sizeof.h"

namespace dale
{
namespace Operation
{
bool 
Sizeof(Context *ctx, llvm::BasicBlock *block, Type *type,
       ParseResult *pr)
{
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    assert(llvm_type);

    llvm::PointerType *lpt =
        llvm::PointerType::getUnqual(llvm_type);

    llvm::Value *pointer =
        builder.CreateGEP(llvm::ConstantPointerNull::get(lpt),
                          ctx->nt->getNativeInt(1));

    llvm::Value *int_pointer =
        builder.CreatePtrToInt(pointer, ctx->nt->getNativeSizeType());

    pr->set(block, ctx->tr->type_size, int_pointer);
    return true;
}

size_t 
SizeofGet(Unit *unit,
          Type *type)
{
    Context *ctx = unit->ctx;

    llvm::Type *llvm_return_type =
        ctx->toLLVMType(ctx->tr->type_size, NULL, false);
    if (!llvm_return_type) {
        return -1;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(llvm_return_type, mc_args, false);

    std::string new_name;
    unit->getUnusedFunctionName(&new_name);

    llvm::Constant *llvm_fnc =
        unit->module->getOrInsertFunction(new_name.c_str(), ft);
    llvm::Function *llvm_fn = llvm::cast<llvm::Function>(llvm_fnc);
    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", llvm_fn);
    ParseResult sizeof_pr;
    bool res = Sizeof(ctx, block, type, &sizeof_pr);
    if (!res) {
        return -1;
    }

    llvm::IRBuilder<> builder(block);
    builder.CreateRet(sizeof_pr.value);

    void* fptr = unit->ee->getPointerToFunction(llvm_fn);
    assert(fptr);

    size_t (*FPTR)(void) = (size_t (*)(void))fptr;
    size_t size = (size_t) FPTR();

    llvm_fn->eraseFromParent();

    return size;
}
}
}
