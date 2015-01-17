#include "Alignmentof.h"

namespace dale
{
namespace Operation
{
namespace Alignmentof
{
static int alignment_structs = 0;

bool 
execute(Context *ctx,
        llvm::BasicBlock *block,
        Type *type,
        ParseResult *pr)
{
    std::vector<llvm::Type*> elements_llvm;
    elements_llvm.push_back(
        ctx->toLLVMType(ctx->tr->type_char, NULL, false));

    llvm::Type *llvm_type =
        ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return false;
    }
    elements_llvm.push_back(llvm_type);

    llvm::StructType *llvm_new_struct =
        llvm::StructType::get(llvm::getGlobalContext(),
                              elements_llvm,
                              false);

    char buf[100];
    sprintf(buf, "_alignment_struct_%d", alignment_structs++);
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

    std::vector<llvm::Value *> two_zero_indices;
    stl::push_back2(&two_zero_indices,
                    ctx->nt->getLLVMZero(), ctx->nt->getLLVMZero());

    llvm::Value *res =
        builder.CreateGEP(
            llvm::ConstantPointerNull::get(lpt),
            llvm::ArrayRef<llvm::Value*>(
                two_zero_indices
            ));

    llvm::Value *res2 =
        builder.CreatePtrToInt(res, ctx->nt->getNativeSizeType());

    pr->set(block, ctx->tr->type_size, res2);
    return true;
}
}
}
}
