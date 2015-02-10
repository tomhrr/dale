#include "Destruct.h"

namespace dale
{
namespace Operation
{
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
     * each element in the array in order from last to first. */
    if (pr->type->is_array && pr->type->array_size) {
        Type *mine =
            pr->type->array_type;
        llvm::BasicBlock   *mbl  = pr->block;
        int i = pr->type->array_size;
        llvm::Value *actual_value = pr->value;
        if (!pr->value) {
            return true;
        }
        if (!pr->value->getType()) {
            return true;
        }

        std::vector<Type *> types;
        if (!mine->is_array) {
            types.push_back(ctx->tr->getPointerType(mine));
            Function *fn = ctx->getFunction("destroy", &types, NULL, 0);
            if (!fn) {
                return true;
            }
        }

        /* Hmph: array literals are stored in the variable table as
         * actual arrays, rather than pointers to arrays. This should
         * be fixed at some point, but for now, if this value is not a
         * pointer, then store it in a temporary location. */

        if (!(pr->value->getType()->isPointerTy())) {
            if (builder) {
                llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                        builder->CreateAlloca(
                            ctx->toLLVMType(pr->type, NULL, false))
                        );
                builder->CreateStore(pr->value, new_ptr2);
                actual_value = new_ptr2;
            } else {
                llvm::IRBuilder<> builder(mbl);
                llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                        builder.CreateAlloca(
                            ctx->toLLVMType(pr->type, NULL, false))
                        );
                builder.CreateStore(pr->value, new_ptr2);
                actual_value = new_ptr2;
            }
        }

        for (i = (pr->type->array_size - 1); i >= 0; i--) {
            ParseResult temp;
            temp.type  = mine;
            temp.block = mbl;
            std::vector<llvm::Value *> indices;
            STL::push_back2(&indices,
                            ctx->nt->getLLVMZero(),
                            llvm::cast<llvm::Value>(
                                llvm::ConstantInt::get(
                                    ctx->nt->getNativeIntType(),
                                    i
                                )
                            ));
            ParseResult mnew;

            if (builder) {
                llvm::Value *res = builder->Insert(
                                       llvm::GetElementPtrInst::Create(
                                           actual_value,
                                           llvm::ArrayRef<llvm::Value*>(indices)
                                       ),
                                       "asdf"
                                   );
                if (!mine->is_array) {
                    temp.value = builder->CreateLoad(res);
                } else {
                    temp.value = res;
                }
                Destruct(ctx, &temp, &mnew, builder);
            }
            else {
                llvm::IRBuilder<> builder(mbl);
                llvm::Value *res = builder.Insert(
                                       llvm::GetElementPtrInst::Create(
                                           actual_value,
                                           llvm::ArrayRef<llvm::Value*>(indices)
                                       ),
                                       "asdf"
                                   );
                if (!mine->is_array) {
                    temp.value = builder.CreateLoad(res);
                } else {
                    temp.value = res;
                }
                Destruct(ctx, &temp, &mnew, &builder);
            }
            mbl = mnew.block;
        }
        pr_ret->block = mbl;
        return true;
    }

    std::vector<Type *> types;
    types.push_back(ctx->tr->getPointerType(pr->type));
    Function *fn = ctx->getFunction("destroy", &types,
                            NULL, 0);
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
            llvm::Value *actual_value = pr->value;

            if (!value_is_ptr) {
                if (builder) {
                    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                            builder->CreateAlloca(
                                ctx->toLLVMType(pr->type, NULL, false))
                            );
                    builder->CreateStore(pr->value, new_ptr2);
                    actual_value = new_ptr2;
                } else {
                    llvm::IRBuilder<> builder(pr->block);
                    llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                            builder.CreateAlloca(
                                ctx->toLLVMType(pr->type, NULL, false))
                            );
                    builder.CreateStore(pr->value, new_ptr2);
                    actual_value = new_ptr2;
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
                element.set(pr->block, *b, actual_value);
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
                                actual_value,
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
                                actual_value,
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
