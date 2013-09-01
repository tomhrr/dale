#include "Cast.h"

namespace dale
{
namespace Operation
{
namespace Cast
{
bool execute(Context *ctx,
                     llvm::BasicBlock *block,
                     llvm::Value *value,
                     Element::Type *from_type,
                     Element::Type *to_type,
                     Node *n,
                     bool implicit,
                     ParseResult *pr)
{
    llvm::IRBuilder<> builder(block);
    llvm::Value *res = NULL;
    std::string *struct_name;

    std::vector<llvm::Value *> two_zero_indices;
    llvm::Value *llvm_native_zero =
        llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0);
    two_zero_indices.push_back(llvm_native_zero);
    two_zero_indices.push_back(llvm_native_zero);

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
        int a = from_type->getFPRelativeSize();
        int b = to_type->getFPRelativeSize();
        if (a < b) {
            /* Target floating point is larger. */
            res = builder.CreateFPExt(value, llvm_to_type);
        } else {
            /* Target floating point is smaller. */
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
               || (from_type->base_type == Type::Bool
                   && to_type->isIntegerType())
               || (from_type->isIntegerType()
                   && to_type->base_type == Type::Bool)) {
        int pr_size =
            ctx->nt->internalSizeToRealSize(from_type->getIntegerSize());
        int ta_size =
            ctx->nt->internalSizeToRealSize(to_type->getIntegerSize());

        if (pr_size <= ta_size) {
            /* Target integer is larger. */
            if (to_type->isSignedIntegerType()) {
                /* Target integer is signed - use sext. */
                res = builder.CreateSExt(value, llvm_to_type);
            } else {
                /* Target integer is not signed - use zext. */
                res = builder.CreateZExt(value, llvm_to_type);
            }
        } else {
            /* Target integer is smaller - use trunc regardless of
             * signedness. */
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
    } else if ((struct_name = from_type->struct_name)
               && (to_type->isIntegerType())
               && (ctx->getEnum(struct_name->c_str()))) {

        Element::Struct *mystruct =
            ctx->getStruct(from_type->struct_name->c_str());
        Element::Type *temp_to_type = mystruct->element_types.at(0);

        // Store the struct.
        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(llvm_from_type)
                                );
        builder.CreateStore(value,
                            new_ptr1);

        // Get a pointer to it.
        llvm::Value *one =
            builder.CreateGEP(new_ptr1,
                              llvm::ArrayRef<llvm::Value*>(two_zero_indices));

        // Bitcast the pointer to an int pointer of the right size.
        Element::Type *ptemp_to_type = ctx->tr->getPointerType(temp_to_type);
        llvm::Value *intptr =
            builder.CreateBitCast(
                one, ctx->toLLVMType(ptemp_to_type,
                                     NULL, false)
            );

        // Load that value.
        llvm::Value *newint =
            builder.CreateLoad(intptr);

        // Cast that value to a value of the right type.

        ParseResult temp;
        bool mres = execute(ctx, block,
                            newint,
                            temp_to_type,
                            to_type,
                            n,
                            implicit,
                            &temp);
        if (!mres) {
            return false;
        }

        block = temp.block;
        res   = temp.value;
    } else if ((struct_name = to_type->struct_name)
               && (from_type->isIntegerType())
               && (ctx->getEnum(struct_name->c_str()))) {

        Element::Struct *mystruct = ctx->getStruct(to_type->struct_name->c_str());
        Element::Type *to_type_temp = mystruct->element_types.at(0);

        ParseResult temp;
        bool mres = execute(ctx, block,
                            value,
                            from_type,
                            to_type_temp,
                            n,
                            implicit, &temp);
        if (!mres) {
            return false;
        }
        block = temp.block;
        value = temp.value;

        // Store the integer.
        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
            builder.CreateAlloca(ctx->toLLVMType(to_type_temp, NULL, false))
                                );
        builder.CreateStore(value,
                            new_ptr1);

        // Bitcast the int pointer to a struct pointer.
        Element::Type *pto_type = ctx->tr->getPointerType(to_type);
        llvm::Value *sp =
            builder.CreateBitCast(
                new_ptr1, ctx->toLLVMType(pto_type,
                                          NULL, false)
            );

        // Load that value.
        llvm::Value *newint =
            builder.CreateLoad(sp);

        // Return that value.
        res = newint;
    } else if (to_type->is_array) {
        // Store the value.
        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(llvm_from_type)
                                );
        builder.CreateStore(value, new_ptr1);
        Element::Type *pto_type = ctx->tr->getPointerType(to_type);
        llvm::Value *sp =
            builder.CreateBitCast(
                new_ptr1, ctx->toLLVMType(pto_type, NULL, false)
            );
        llvm::Value *newint =
            builder.CreateLoad(sp);
        res = newint;

    } else {
        std::string fts;
        std::string tts;
        from_type->toStringProper(&fts);
        to_type->toStringProper(&tts);

        Error *e = new Error(
            ErrorInst::Generator::InvalidCast,
            n,
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
}
