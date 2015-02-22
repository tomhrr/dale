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
destructArray(Context *ctx, ParseResult *pr, ParseResult *pr_ret,
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

    llvm::IRBuilder<> internal_builder(block);
    if (!builder) {
        builder = &internal_builder;
    }

    if (!pr->value->getType()->isPointerTy()) {
        array_value = llvm::cast<llvm::Value>(
            builder->CreateAlloca(llvm_array_type)
        );
        builder->CreateStore(pr->value, array_value);
    }

    for (int i = (pr->type->array_size - 1); i >= 0; i--) {
        ParseResult temp;
        temp.type  = array_type;
        temp.block = block;
        std::vector<llvm::Value *> indices;
        STL::push_back2(&indices,
                        ctx->nt->getLLVMZero(),
                        llvm::cast<llvm::Value>(
                            llvm::ConstantInt::get(
                                ctx->nt->getNativeIntType(), i
                            )
                        ));
        ParseResult mnew;

        llvm::Value *res = builder->Insert(
                                llvm::GetElementPtrInst::Create(
                                    array_value,
                                    llvm::ArrayRef<llvm::Value*>(indices)
                                ),
                                "asdf"
                            );
        if (!array_type->is_array) {
            temp.value = builder->CreateLoad(res);
        } else {
            temp.value = res;
        }
        Destruct(ctx, &temp, &mnew, builder);
    }

    pr_ret->block = block;
    return true;
}

bool
Destruct(Context *ctx, ParseResult *pr, ParseResult *pr_ret,
         llvm::IRBuilder<> *builder, bool value_is_ptr)
{
    pr->copyTo(pr_ret);

    if (pr->do_not_destruct) {
        return true;
    }

    assert(pr->type);

    /* If it's an array with a known size, call this function for
     * each element in the array, in order from last to first. */
    if (pr->type->is_array && pr->type->array_size) {
        return destructArray(ctx, pr, pr_ret, builder, value_is_ptr);
    }

    Function *fn = getDestructor(ctx, pr->type);
    if (!fn) {
        /* If this is a struct, call Destruct on each of the elements,
         * in the absence of a destructor for the struct as a whole.
         * */
        Type *type = pr->type;
        if (type->struct_name.size()) {
            Struct *st = ctx->getStruct(type->struct_name.c_str(),
                                                 &(type->namespaces));
            std::vector<Type*> *st_types = &(st->member_types);
            int i = 0;
            llvm::Value *array_value = pr->value;

            if (!value_is_ptr) {
                if (builder) {
                    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                            builder->CreateAlloca(
                                ctx->toLLVMType(pr->type, NULL, false))
                            );
                    builder->CreateStore(pr->value, new_ptr2);
                    array_value = new_ptr2;
                } else {
                    llvm::IRBuilder<> builder(pr->block);
                    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                            builder.CreateAlloca(
                                ctx->toLLVMType(pr->type, NULL, false))
                            );
                    builder.CreateStore(pr->value, new_ptr2);
                    array_value = new_ptr2;
                }
            }

            for (std::vector<Type*>::iterator
                    b = st_types->begin(),
                    e = st_types->end();
                    b != e;
                    ++b) {
                ParseResult element;
                ParseResult mnew;
                std::string ts;
                (*b)->toString(&ts);
                element.set(pr->block, *b, array_value);
                std::vector<llvm::Value *> indices;
                STL::push_back2(&indices,
                                ctx->nt->getLLVMZero(),
                                llvm::cast<llvm::Value>(
                                    llvm::ConstantInt::get(
                                        ctx->nt->getNativeIntType(),
                                        i++
                                    )
                                ));
                if (builder) {
                    llvm::Value *res = 
                        builder->Insert(
                            llvm::GetElementPtrInst::Create(
                                array_value,
                                llvm::ArrayRef<llvm::Value*>(indices)
                            ),
                            "asdf"
                        );
                    element.value = res;
                    Destruct(ctx, &element, &mnew, builder, true);
                } else {
                    llvm::IRBuilder<> builder(pr->block);
                    llvm::Value *res = 
                        builder.Insert(
                            llvm::GetElementPtrInst::Create(
                                array_value,
                                llvm::ArrayRef<llvm::Value*>(indices)
                            ),
                            "asdf"
                        );
                    element.value = res;
                    Destruct(ctx, &element, &mnew, &builder, true);
                }
            }
        }
        return true;
    }
    int destroy_builder = 0;
    if (!builder) {
        destroy_builder = 1;
        builder = new llvm::IRBuilder<>(pr->block);
    }
    std::vector<llvm::Value *> call_args;
    llvm::Value *new_ptr2;
    if (value_is_ptr) {
        new_ptr2 = pr->value;
    } else {
        new_ptr2 = llvm::cast<llvm::Value>(
            builder->CreateAlloca(ctx->toLLVMType(pr->type, NULL, false))
        );
        builder->CreateStore(pr->value, new_ptr2);
    }

    call_args.push_back(new_ptr2);
    builder->CreateCall(
        fn->llvm_function,
        llvm::ArrayRef<llvm::Value*>(call_args));
    if (destroy_builder) {
        delete builder;
    }
    return true;
}
}
}
