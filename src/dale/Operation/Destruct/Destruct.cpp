#include "Destruct.h"

namespace dale
{
namespace Operation
{
Function *
getDestructor(Context *ctx, Type *type)
{
    std::vector<Type *> types;
    types.push_back(ctx->tr->getPointerType(type));
    Function *fn = ctx->getFunction("destroy", &types, NULL, 0);
    return fn;
}

bool
destructArray(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
              llvm::IRBuilder<> *builder, bool value_is_ptr)
{
    Type *array_type = pr->type->array_type;
    llvm::BasicBlock *block = pr->block;
    llvm::Value *array_value = pr->value;

    if (!array_value) {
        return true;
    }
    if (!array_value->getType()) {
        return true;
    }

    if (!array_type->is_array) {
        Function *fn = getDestructor(ctx, array_type);
        if (!fn) {
            return true;
        }
    }

    llvm::Type *llvm_array_type = ctx->toLLVMType(pr->type, NULL, false);

    /* Array literals are stored in the variable table as actual
     * arrays, rather than pointers to arrays.  This should be fixed at
     * some point, but for now, if this value is not a pointer, then
     * store it in a temporary location. */

    if (!pr->value->getType()->isPointerTy()) {
        array_value = llvm::cast<llvm::Value>(
            builder->CreateAlloca(llvm_array_type)
        );
        builder->CreateStore(pr->value, array_value);
    }

    for (int i = (pr->type->array_size - 1); i >= 0; i--) {
        ParseResult element;
        element.type  = array_type;
        element.block = block;
        std::vector<llvm::Value *> indices;
        STL::push_back2(
            &indices,
            ctx->nt->getLLVMZero(),
            llvm::cast<llvm::Value>(ctx->nt->getNativeInt(i))
        );

        llvm::Value *res =
            builder->Insert(
                llvm::GetElementPtrInst::Create(
                    array_value, llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "ap"
            );
        if (!array_type->is_array) {
            element.value = builder->CreateLoad(res);
        } else {
            element.value = res;
        }
        Destruct(ctx, &element, &element, builder);
        block = element.block;
    }

    ret_pr->block = block;
    return true;
}

bool
destructStruct(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
               llvm::IRBuilder<> *builder, bool value_is_ptr)
{
    Struct *st = ctx->getStruct(pr->type);
    std::vector<Type*> *st_types = &(st->member_types);

    llvm::Value *struct_value;
    if (value_is_ptr) {
        struct_value = pr->value;
    } else {
        struct_value = llvm::cast<llvm::Value>(
            builder->CreateAlloca(
                ctx->toLLVMType(pr->type, NULL, false))
            );
        builder->CreateStore(pr->value, struct_value);
    }

    int i = 0;
    for (std::vector<Type*>::iterator b = st_types->begin(),
                                      e = st_types->end();
            b != e;
            ++b) {
        ParseResult element;
        element.set(ret_pr->block, *b, struct_value);
        std::vector<llvm::Value *> indices;
        STL::push_back2(
            &indices,
            ctx->nt->getLLVMZero(),
            llvm::cast<llvm::Value>(ctx->nt->getNativeInt(i++))
        );
        element.value =
            builder->Insert(
                llvm::GetElementPtrInst::Create(
                    struct_value,
                    llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "sp"
            );
        Destruct(ctx, &element, &element, builder, true);
        ret_pr->block = element.block;
    }

    return true;
}

bool
destruct_(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
          llvm::IRBuilder<> *builder, bool value_is_ptr)
{
    pr->copyTo(ret_pr);

    if (pr->do_not_destruct) {
        return true;
    }

    if (pr->type->is_array && pr->type->array_size) {
        return destructArray(ctx, pr, ret_pr, builder, value_is_ptr);
    }

    Function *fn = getDestructor(ctx, pr->type);
    if (!fn) {
        if (pr->type->struct_name.size()) {
            destructStruct(ctx, pr, ret_pr, builder, value_is_ptr);
        }
        return true;
    }

    std::vector<llvm::Value *> call_args;
    llvm::Value *value_ptr;
    if (value_is_ptr) {
        value_ptr = pr->value;
    } else {
        value_ptr = llvm::cast<llvm::Value>(
            builder->CreateAlloca(ctx->toLLVMType(pr->type, NULL, false))
        );
        builder->CreateStore(pr->value, value_ptr);
    }

    call_args.push_back(value_ptr);
    builder->CreateCall(
        fn->llvm_function,
        llvm::ArrayRef<llvm::Value*>(call_args)
    );

    return true;
}

bool
Destruct(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
         llvm::IRBuilder<> *builder, bool value_is_ptr)
{
    bool res;

    if (!builder) {
        llvm::IRBuilder<> internal_builder(pr->block);
        res = destruct_(ctx, pr, ret_pr, &internal_builder, value_is_ptr);
    } else {
        res = destruct_(ctx, pr, ret_pr, builder, value_is_ptr);
    }

    return res;
}
}
}
