#include "Cast.h"

#include "../../ErrorType/ErrorType.h"

namespace dale
{
namespace Operation
{
bool
Cast(Context *ctx, llvm::BasicBlock *block, llvm::Value *value,
     Type *from_type, Type *to_type, Node *n, bool implicit, ParseResult *pr)
{
    llvm::IRBuilder<> builder(block);
    llvm::Value *res = NULL;
    std::string *struct_name;

    std::vector<llvm::Value *> two_zero_indices;
    STL::push_back2(&two_zero_indices,
                    ctx->nt->getLLVMZero(), ctx->nt->getLLVMZero());

    llvm::Type *llvm_from_type =
        ctx->toLLVMType(from_type, NULL, false);
    if (!llvm_from_type) {
        return false;
    }
    llvm::Type *llvm_to_type =
        ctx->toLLVMType(to_type, NULL, false);
    if (!llvm_to_type) {
        return false;
    }

    if (from_type->isFloatingPointType()
            && to_type->isFloatingPointType()) {
        int a = from_type->getFloatingPointRelativeSize();
        int b = to_type->getFloatingPointRelativeSize();
        if (a < b) {
            res = builder.CreateFPExt(value, llvm_to_type);
        } else {
            res = builder.CreateFPTrunc(value, llvm_to_type);
        }
    } else if (from_type->isFloatingPointType()
               && to_type->isIntegerType()) {
        if (to_type->isSignedIntegerType()) {
            res = builder.CreateFPToSI(value, llvm_to_type);
        } else {
            res = builder.CreateFPToUI(value, llvm_to_type);
        }
    } else if (from_type->isIntegerType()
               && to_type->isFloatingPointType()) {
        if (from_type->isSignedIntegerType()) {
            res = builder.CreateSIToFP(value, llvm_to_type);
        } else {
            res = builder.CreateUIToFP(value, llvm_to_type);
        }
    } else if ((from_type->isIntegerType() && to_type->isIntegerType())
               || (from_type->base_type == BaseType::Bool
                   && to_type->isIntegerType())
               || (from_type->isIntegerType()
                   && to_type->base_type == BaseType::Bool)) {
        int pr_size =
            ctx->nt->internalSizeToRealSize(from_type->getIntegerSize());
        int ta_size =
            ctx->nt->internalSizeToRealSize(to_type->getIntegerSize());

        if (pr_size <= ta_size) {
            if (to_type->isSignedIntegerType()) {
                res = builder.CreateSExt(value, llvm_to_type);
            } else {
                res = builder.CreateZExt(value, llvm_to_type);
            }
        } else {
            res = builder.CreateTrunc(value, llvm_to_type);
        }
    } else if (!implicit && from_type->isIntegerType()
               && !(from_type->points_to) && to_type->points_to) {
        res = builder.CreateIntToPtr(value, llvm_to_type);
    } else if (!implicit && from_type->points_to && !(to_type->points_to)
               && to_type->isIntegerType()) {
        res = builder.CreatePtrToInt(value, llvm_to_type);
    } else if (from_type->points_to && to_type->points_to) {
        res = builder.CreateBitCast(value, llvm_to_type);
    } else if ((struct_name = &(from_type->struct_name))
               && struct_name->size()
               && (to_type->isIntegerType())
               && (ctx->getEnum(struct_name->c_str()))) {
        llvm::Value *pointer = 
            llvm::cast<llvm::Value>(
                builder.CreateAlloca(llvm_from_type)
            );
        builder.CreateStore(value, pointer);

        llvm::Value *pointer_value =
            builder.CreateGEP(pointer,
                              llvm::ArrayRef<llvm::Value*>(two_zero_indices));

        Struct *st = ctx->getStruct(from_type->struct_name.c_str());
        Type *intermediate_type = st->member_types.at(0);
        llvm::Value *int_pointer =
            builder.CreateBitCast(
                pointer_value,
                ctx->toLLVMType(
                    ctx->tr->getPointerType(intermediate_type),
                    NULL, false
                )
            );

        llvm::Value *new_int =
            builder.CreateLoad(int_pointer);

        ParseResult cast_pr;
        bool cast_res = Cast(ctx, block, new_int, intermediate_type, 
                             to_type, n, implicit, &cast_pr);
        if (!cast_res) {
            return false;
        }

        block = cast_pr.block;
        res   = cast_pr.value;
    } else if ((struct_name = &(to_type->struct_name))
               && struct_name->size()
               && (from_type->isIntegerType())
               && (ctx->getEnum(struct_name->c_str()))) {

        Struct *st = ctx->getStruct(to_type->struct_name.c_str());
        Type *intermediate_type = st->member_types.at(0);

        ParseResult cast_pr;
        bool cast_res = Cast(ctx, block, value, from_type, intermediate_type,
                             n, implicit, &cast_pr);
        if (!cast_res) {
            return false;
        }
        block = cast_pr.block;
        value = cast_pr.value;

        llvm::Value *pointer = 
            llvm::cast<llvm::Value>(
                builder.CreateAlloca(
                    ctx->toLLVMType(intermediate_type, NULL, false)
                )
            );
        builder.CreateStore(value, pointer);
        llvm::Value *int_pointer =
            builder.CreateBitCast(
                pointer, 
                ctx->toLLVMType(ctx->tr->getPointerType(to_type),
                                NULL, false)
            );

        llvm::Value *new_int =
            builder.CreateLoad(int_pointer);

        res = new_int;
    } else if (to_type->is_array) {
        llvm::Value *pointer =
            llvm::cast<llvm::Value>(
                builder.CreateAlloca(llvm_from_type)
            );
        builder.CreateStore(value, pointer);
        llvm::Value *int_pointer =
            builder.CreateBitCast(
                pointer, 
                ctx->toLLVMType(ctx->tr->getPointerType(to_type),
                                NULL, false)
            );

        llvm::Value *new_int =
            builder.CreateLoad(int_pointer);
        res = new_int;
    } else {
        std::string fts;
        std::string tts;
        from_type->toString(&fts);
        to_type->toString(&tts);

        Error *e = new Error(
            ErrorInst::InvalidCast, n,
            fts.c_str(), tts.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    pr->block = block;
    pr->type = to_type;
    pr->value = res;

    return true;
}
}
}
