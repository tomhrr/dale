#include "Offsetof.h"

#include <cstdio>

namespace dale
{
namespace Operation
{
static int
nameToIndex(Context *ctx,
            Type *type,
            const char *member_name)
{
    Struct *st = ctx->getStruct(type);
    assert(st);

    int index = st->nameToIndex(member_name);
    assert(index != -1);

    return index;
}

bool
Offsetof(Context *ctx,
         llvm::BasicBlock *block,
         Type *type,
         const char *member_name,
         ParseResult *pr)
{
    return OffsetofByIndex(ctx, block, type,
                           nameToIndex(ctx, type, member_name), pr);
}

bool
OffsetofByIndex(Context *ctx,
                llvm::BasicBlock *block,
                Type *type,
                int index,
                ParseResult *pr)
{
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return false;
    }

    llvm::PointerType *llvm_pointer_type
        = llvm::PointerType::getUnqual(llvm_type);

    std::vector<llvm::Value *> indices;
    STL::push_back2(&indices,  ctx->nt->getLLVMZero(),
                               ctx->nt->getNativeInt(index));

    llvm::Value *pointer_to_offset =
        builder.CreateGEP(
            llvm::ConstantPointerNull::get(llvm_pointer_type),
            llvm::ArrayRef<llvm::Value *>(indices)
        );

    llvm::Value *int_pointer_to_offset =
        builder.CreatePtrToInt(pointer_to_offset,
                               ctx->nt->getNativeSizeType());

    pr->set(block, ctx->tr->type_size, int_pointer_to_offset);
    return true;
}

static int function_count = 0;

size_t
OffsetofGetByIndex(Unit *unit,
                   Type *type,
                   int index)
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

    char buf[32];
    sprintf(buf, "_oo%d", function_count++);
    std::string new_name;
    ctx->ns()->nameToSymbol(buf, &new_name);

    assert(!unit->module->getFunction(llvm::StringRef(new_name.c_str())));

    llvm::Constant *llvm_fnc =
        unit->module->getOrInsertFunction(new_name.c_str(), ft);
    llvm::Function *llvm_fn = llvm::cast<llvm::Function>(llvm_fnc);
    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", llvm_fn);
    ParseResult by_index_pr;
    bool res = OffsetofByIndex(ctx, block, type, index, &by_index_pr);
    if (!res) {
        return -1;
    }

    llvm::IRBuilder<> builder(block);
    builder.CreateRet(by_index_pr.getValue(ctx));

    void* fptr = unit->ee->getPointerToFunction(llvm_fn);
    assert(fptr);

    size_t (*FPTR)() = (size_t (*)())fptr;
    size_t offset = (size_t) FPTR();

    llvm_fn->eraseFromParent();

    return offset;
}
}
}
