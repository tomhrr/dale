#include "Destruct.h"

#include <vector>

#include "../../Form/Utils/Utils.h"

namespace dale {
namespace Operation {
Function *getDestructor(Context *ctx, Type *type) {
    std::vector<Type *> types;
    types.push_back(type);
    Function *fn = ctx->getFunction("destroy", &types, NULL, 0);
    return fn;
}

bool destructArray(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
                   llvm::IRBuilder<> *builder, bool value_is_ptr) {
    Type *array_type = pr->type->array_type;
    llvm::BasicBlock *block = pr->block;

    if (!array_type->is_array) {
        Function *fn = getDestructor(ctx, array_type);
        if (!fn) {
            return true;
        }
    }

    llvm::Value *array_value = pr->getValue(ctx);

    if (!array_value) {
        return true;
    }
    if (!array_value->getType()) {
        return true;
    }

    llvm::Type *llvm_array_type =
        ctx->toLLVMType(pr->type, NULL, false);

    /* Array literals are stored in the variable table as actual
     * arrays, rather than pointers to arrays.  This should be fixed at
     * some point, but for now, if this value is not a pointer, then
     * store it in a temporary location. */

    if (!pr->getValue(ctx)->getType()->isPointerTy()) {
        array_value = llvm::cast<llvm::Value>(
            builder->CreateAlloca(llvm_array_type));
        builder->CreateStore(pr->getValue(ctx), array_value);
    }

    for (int i = (pr->type->array_size - 1); i >= 0; i--) {
        ParseResult element;
        element.type = array_type;
        element.block = block;
        std::vector<llvm::Value *> indices;
        STL::push_back2(
            &indices, ctx->nt->getLLVMZero(),
            llvm::cast<llvm::Value>(ctx->nt->getNativeInt(i)));

        llvm::Value *res = builder->Insert(
            createGEP(array_value,
                      llvm::ArrayRef<llvm::Value *>(indices)),
            "ap");
        res = (!array_type->is_array) ? builder->CreateLoad(res) : res;
        element.set(element.block, element.type, res);
        element.do_not_destruct = false;
        Destruct(ctx, &element, &element, builder);
        block = element.block;
    }

    ret_pr->block = block;
    return true;
}

bool destructStruct(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
                    llvm::IRBuilder<> *builder, bool value_is_ptr) {
    Struct *st = ctx->getStruct(pr->type);
    std::vector<Type *> *st_types = &(st->member_types);

    llvm::Value *struct_value;
    int s0 = builder->GetInsertBlock()->size();
    if (value_is_ptr) {
        struct_value = pr->getValue(ctx);
    } else {
        struct_value = llvm::cast<llvm::Value>(builder->CreateAlloca(
            ctx->toLLVMType(pr->type, NULL, false)));
        builder->CreateStore(pr->getValue(ctx), struct_value);
    }
    int s1 = builder->GetInsertBlock()->size();

    int i = 0;
    for (std::vector<Type *>::iterator b = st_types->begin(),
                                       e = st_types->end();
         b != e; ++b) {
        ParseResult element;
        element.set(ret_pr->block, *b, struct_value);
        std::vector<llvm::Value *> indices;
        STL::push_back2(
            &indices, ctx->nt->getLLVMZero(),
            llvm::cast<llvm::Value>(ctx->nt->getNativeInt(i++)));
        element.set(
            element.block, element.type,
            builder->Insert(
                createGEP(struct_value,
                          llvm::ArrayRef<llvm::Value *>(indices),
                          ctx->toLLVMType(pr->type, NULL, false)),
                "sp"));
        element.do_not_destruct = false;
        Destruct(ctx, &element, &element, builder, true);
        ret_pr->block = element.block;
    }

    /* If the only instructions that have been added are GEP
     * instructions, then remove those instructions, as well as the
     * store and the alloca (and possibly a load), because the
     * destruct operation is a no-op. */

    int s2 = builder->GetInsertBlock()->size();
    int skip = s1;
    bool has_non_gep = false;
    for (const llvm::Instruction &I : *(builder->GetInsertBlock())) {
        if (skip > 0) {
            skip--;
            continue;
        }
        if (I.getOpcode() != llvm::Instruction::GetElementPtr) {
            has_non_gep = true;
            break;
        }
    }
    if (!has_non_gep) {
        int to_remove = s2 - s0;
        llvm::BasicBlock *bb = builder->GetInsertBlock();
        while (to_remove--) {
            bb->back().eraseFromParent();
        }
    }

    return true;
}

bool destruct_(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
               llvm::IRBuilder<> *builder, bool value_is_ptr) {
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
        value_ptr = pr->getValue(ctx);
    } else {
        value_ptr = llvm::cast<llvm::Value>(builder->CreateAlloca(
            ctx->toLLVMType(pr->type, NULL, false)));
        builder->CreateStore(pr->getValue(ctx), value_ptr);
    }

    call_args.push_back(value_ptr);
    builder->CreateCall(fn->llvm_function,
                        llvm::ArrayRef<llvm::Value *>(call_args));

    return true;
}

bool Destruct(Context *ctx, ParseResult *pr, ParseResult *ret_pr,
              llvm::IRBuilder<> *builder, bool value_is_ptr) {
    bool res;

    if (!builder) {
        llvm::IRBuilder<> internal_builder(pr->block);
        res =
            destruct_(ctx, pr, ret_pr, &internal_builder, value_is_ptr);
    } else {
        res = destruct_(ctx, pr, ret_pr, builder, value_is_ptr);
    }

    return res;
}
}
}
