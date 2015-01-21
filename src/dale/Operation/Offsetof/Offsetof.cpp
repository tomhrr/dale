#include "Offsetof.h"

namespace dale
{
namespace Operation
{
namespace Offsetof
{
int
nameToIndex(Context *ctx,
            Type *type,
            const char *field_name)
{
    Struct *structp =
        ctx->getStruct(
            type->struct_name.c_str(),
            &(type->namespaces)
        );

    if (!structp) {
        fprintf(stderr, "Internal error: invalid struct name.\n");
        abort();
    }

    int index = structp->memberToIndex(field_name);
    if (index == -1) {
        fprintf(stderr, "Internal error: invalid struct field"
                "name.\n");
        abort();
    }

    return index;
}

bool 
execute(Context *ctx,
        llvm::BasicBlock *block,
        Type *type,
        const char *field_name,
        ParseResult *pr)
{
    return executeByIndex(ctx, block, type, 
                          nameToIndex(ctx, type, field_name), pr);
}

bool 
executeByIndex(Context *ctx,
               llvm::BasicBlock *block,
               Type *type,
               int index,
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

    std::vector<llvm::Value *> indices;
    stl::push_back2(&indices,  ctx->nt->getLLVMZero(),
                               ctx->nt->getNativeInt(index));
                    
    llvm::Value *res =
        builder.CreateGEP(
            llvm::ConstantPointerNull::get(lpt),
            llvm::ArrayRef<llvm::Value *>(indices));

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
    Type *type,
    const char *field_name)
{
    return getByIndex(unit, type, nameToIndex(unit->ctx, type, field_name));
}

size_t
getByIndex(Unit *unit,
           Type *type,
           int index)
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
    sprintf(buf, "_function_offsetof_%d", function_count++);
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
    bool mres = executeByIndex(ctx, block, type, index, &mine);
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
