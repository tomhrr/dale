#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../Linkage/Linkage.h"
#include "../../Type/Type.h"
#include "../../Struct/Struct.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
Function *
get_init_fn(Context *ctx,
            Type *type)
{
    std::vector<Type *> init_arg_types;
    init_arg_types.push_back(type);
    return ctx->getFunction("init", &init_arg_types, NULL, 0);
}

bool
initialise(Context *ctx,
                llvm::IRBuilder<> *builder,
                Type *type,
                llvm::Value *value,
                Function *init_fn)
{
    if (!init_fn) {
        init_fn = get_init_fn(ctx, type);
    }

    if (init_fn) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(value);
        builder->CreateCall(
            init_fn->llvm_function, 
            llvm::ArrayRef<llvm::Value*>(call_args)
        );
        return true;
    }

    if (type->array_type) {
        init_fn = get_init_fn(ctx, type->array_type);
        if (!init_fn) {
            return true;
        }

        std::vector<llvm::Value *> indices;
        indices.push_back(ctx->nt->getLLVMZero());
        for (int i = 0; i < type->array_size; i++) {
            indices.push_back(
                llvm::cast<llvm::Value>(
                    ctx->nt->getNativeInt(i)
                )
            );
            llvm::Value *aref = builder->Insert(
                llvm::GetElementPtrInst::Create(
                    value,
                    llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "aref"
            );
            initialise(ctx, builder, type->array_type,
                        aref, init_fn);
            indices.pop_back();
        }

        return true;
    }

    if (type->struct_name.size()) {
        Struct *sp =
            ctx->getStruct(
                type->struct_name.c_str(),
                &(type->namespaces)
            );
        int i = 0;
        std::vector<llvm::Value *> indices;
        indices.push_back(ctx->nt->getLLVMZero());
        for (std::vector<Type *>::iterator
                b = sp->member_types.begin(),
                e = sp->member_types.end();
                b != e;
                ++b) {
            Type *t = (*b);
            indices.push_back(
                llvm::cast<llvm::Value>(
                    ctx->nt->getNativeInt(i)
                )
            );
            llvm::Value *sref = builder->Insert(
                llvm::GetElementPtrInst::Create(
                    value,
                    llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "sref"
            );
            indices.pop_back();
            i++;
            initialise(ctx, builder, t, sref, NULL);
        }
        return true;
    }

    return true;
}

bool
FormProcDefParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("def", node, 2, 2)) {
        return false;
    }

    symlist *lst = node->list;
    Node *nname = (*lst)[1];

    if (!ctx->er->assertArgIsAtom("def", nname, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("def", nname, "1")) {
        return false;
    }

    Token *t = nname->token;
    char name[255];
    strncpy(name, t->str_value.c_str(), 255);

    Node *ndef = (*lst)[2];

    if (!ctx->er->assertArgIsList("def", ndef, "2")) {
        return false;
    }

    Node *nvar = NULL;

    if ((ndef->list->size() < (int) 1)
            || (!(nvar = (*(ndef->list))[0]))
            || (!(nvar->is_token))
            || (!(nvar->token->type == TokenType::String))) {
        if (nvar->token->str_value.compare("var")
                && nvar->token->str_value.compare("struct")) {
            Error *e = new Error(
                ErrorInst::Generator::OnlyVarPermitted,
                ndef
            );
            ctx->er->addError(e);
            return false;
        }
    }

    if (!(nvar->token->str_value.compare("struct"))) {
        FormStructParse(units, ndef, name);
        pr->set(block, ctx->tr->type_int,
              llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));
        return true;
    }

    symlist *newlist = ndef->list;
    nvar = (*newlist)[0];

    /* Parse linkage. */

    int linkage = FormLinkageParse(ctx, (*newlist)[1]);
    if (!linkage) {
        return false;
    }

    if ((linkage != Linkage::Auto)
            && (linkage != Linkage::Intern)
            && (newlist->size() > 3)) {
        Error *e = new Error(
            ErrorInst::Generator::HasBothExternAndInitialiser,
            ndef
        );
        ctx->er->addError(e);
        return false;
    }

    /* Check if the type is a single token string equal to "\". If
     * it is, then the type is implied based on the result of
     * parsing the later expression. */

    pr->set(block, ctx->tr->type_int,
          llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));
    pr->do_not_destruct       = 1;
    pr->do_not_copy_with_setf = 1;

    Type *type;

    if ((*newlist)[2]->is_token &&
            !(*newlist)[2]->token->str_value.compare("\\")) {
        if (newlist->size() != 4) {
            Error *e = new Error(
                ErrorInst::Generator::MustHaveInitialiserForImpliedType,
                ndef
            );
            ctx->er->addError(e);
            return false;
        }

        ParseResult p;
        Node *last = (*newlist)[3];
        symlist *vlst = last->list;
        Variable *var_value = NULL;
        llvm::IRBuilder<> builder(block);
        if (last->is_token
                && (var_value = ctx->getVariable(
                        last->token->str_value.c_str()))) {
            p.value = builder.CreateLoad(var_value->value);
            p.type  = var_value->type;
            p.do_not_destruct = 1;
            p.block = block;
        } else if (last->is_list
                && (vlst->size() == 2)
                && (vlst->at(0)->is_token)
                && (!vlst->at(0)->token->str_value.compare("@"))
                && (vlst->at(1)->is_token)
                && (var_value = ctx->getVariable(
                        vlst->at(1)->token->str_value.c_str()))) {
            p.value = builder.CreateLoad(builder.CreateLoad(var_value->value));
            p.type  = var_value->type->points_to;
            p.do_not_destruct = 1;
            p.block = block;
        } else {
            bool res =
                FormProcInstParse(units,
                    fn, block, last, get_address, false, NULL, &p
                );
            if (!res) {
                return false;
            }
        }

        type  = p.type;
        block = p.block;
        builder.SetInsertPoint(block);

        llvm::Type *et = ctx->toLLVMType(type, (*newlist)[2], 
                                         false, false, false);
        if (!et) {
            return false;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(et)
                               );
        Variable *var2 = new Variable();
        var2->name.append(name);
        var2->type = type;
        var2->value = new_ptr;
        var2->linkage = Linkage::Auto;
        int avres = ctx->ns()->addVariable(name, var2);

        if (!avres) {
            Error *e = new Error(
                ErrorInst::Generator::RedefinitionOfVariable,
                node,
                name
            );
            ctx->er->addError(e);
            return false;
        }

        if (p.retval_used) {
            var2->value = p.retval;
            pr->block = p.block;
            return true;
        }

        /* If the constant int 0 is returned, and this isn't an
         * integer type (or bool), then skip this part (assume
         * that the variable has been initialised by the user).
         * This is to save pointless copies/destructs, while still
         * allowing the variable to be fully initialised once the
         * define is complete. */

        if (!(type->isIntegerType()) && (type->base_type != BaseType::Bool)) {
            if (llvm::ConstantInt *temp =
                        llvm::dyn_cast<llvm::ConstantInt>(p.value)) {
                if (temp->getValue().getLimitedValue() == 0) {
                    pr->block = p.block;
                    return true;
                }
            }
        }

        if (!ctx->er->assertTypeEquality("def", node, p.type, type, 1)) {
            return false;
        }

        std::vector<Type *> call_arg_types;
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        Function *or_setf =
            ctx->getFunction("setf-copy", &call_arg_types,
                             NULL, 0);
        if (or_setf && type->isEqualTo(p.type)) {
            std::vector<llvm::Value *> call_args2;
            call_args2.push_back(new_ptr);
            llvm::Value *new_ptr2 =
                llvm::cast<llvm::Value>(
                    builder.CreateAlloca(ctx->toLLVMType(type, NULL,
                                                         false, false))
                );
            builder.CreateStore(p.value, new_ptr2);
            call_args2.push_back(new_ptr2);
            builder.CreateCall(
                or_setf->llvm_function,
                llvm::ArrayRef<llvm::Value*>(call_args2));
        } else {
            call_arg_types.pop_back();
            call_arg_types.push_back(p.type);
            Function *or_setf2 =
                ctx->getFunction("setf-copy", &call_arg_types,
                                 NULL, 0);
            if (or_setf2) {
                std::vector<llvm::Value *> call_args2;
                call_args2.push_back(new_ptr);
                call_args2.push_back(p.value);
                builder.CreateCall(
                    or_setf2->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args2));
            } else {
                builder.CreateStore(p.value, new_ptr);
            }
        }
        ParseResult temp;
        bool mres = Operation::Destruct(ctx, &p, &temp);
        if (!mres) {
            return false;
        }

        pr->block = temp.block;
        return true;
    } else {
        /* Parse the type. */
        type = FormTypeParse(units, (*newlist)[2], false, false);
        if (!type) {
            return false;
        }
        
        /* Find the init function, if it exists. */
        std::vector<Type *> init_arg_types;
        init_arg_types.push_back(type);
        Function *init_fn =
            ctx->getFunction("init", &init_arg_types, NULL, 0);

        /* If it's a struct, check if it's must-init. */
        if (type->struct_name.size()) {
            Struct *mine =
                ctx->getStruct(
                    type->struct_name.c_str(),
                    &(type->namespaces)
                );
            if (mine->must_init && (newlist->size() == 3) && !init_fn) {
                Error *e = new Error(
                    ErrorInst::Generator::MustHaveInitialiserForType,
                    ndef
                );
                ctx->er->addError(e);
                return false;
            }
        }

        bool is_zero_sized =
            (type->array_type && (type->array_size == 0));

        /* Add an alloca instruction for this variable. */

        llvm::IRBuilder<> builder(block);
        llvm::Type *et = ctx->toLLVMType(type, (*newlist)[2], false,
                                         false, true);
        if (!et) {
            return false;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(et)
                               );
        Variable *var2 = new Variable();
        var2->name.append(name);
        var2->type = type;
        var2->value = new_ptr;
        var2->linkage = linkage;
        int avres = ctx->ns()->addVariable(name, var2);
        if (!avres) {
            Error *e = new Error(
                ErrorInst::Generator::RedefinitionOfVariable,
                node,
                name
            );
            ctx->er->addError(e);
            return false;
        }

        if (newlist->size() == 3) {
            if (type->is_const && !init_fn) {
                Error *e = new Error(
                    ErrorInst::Generator::MustHaveInitialiserForConstType,
                    ndef
                );
                ctx->er->addError(e);
                return false;
            }

            initialise(ctx, &builder, type, new_ptr, init_fn);

            pr->set(block, ctx->tr->type_int,
                  llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));

            return true;
        }

        ParseResult p;
        Node *last = (*newlist)[3];
        symlist *vlst = last->list;
        Variable *var_value = NULL;
        /* Add the pointer as the retval. */
        p.retval      = new_ptr;
        p.retval_type = ctx->tr->getPointerType(type);
        if (last->is_token
                && (var_value = ctx->getVariable(
                        last->token->str_value.c_str()))) {
            p.value = builder.CreateLoad(var_value->value);
            p.type  = var_value->type;
            p.do_not_destruct = 1;
            p.block = block;
        } else if (last->is_list
                && (vlst->size() == 2)
                && (vlst->at(0)->is_token)
                && (!vlst->at(0)->token->str_value.compare("@"))
                && (vlst->at(1)->is_token)
                && (var_value = ctx->getVariable(
                        vlst->at(1)->token->str_value.c_str()))) {
            p.value = builder.CreateLoad(builder.CreateLoad(var_value->value));
            p.type  = var_value->type->points_to;
            p.do_not_destruct = 1;
            p.block = block;
        } else {
            bool res =
                FormProcInstParse(units,
                    fn, block, last, get_address, false, type, &p
                );
            if (!res) {
                return false;
            }

            /* If the retval was used, then there's no need for anything
             * following. */
            if (p.retval_used) {
                pr->block = p.block;
                return true;
            }
        }

        /* If the constant int 0 is returned and this isn't an integer
         * type, or the initialisation form is a list where the first
         * token is 'init', then skip this part (assume that the
         * variable has been initialised by the user). This is to save
         * pointless copies/destructs, while still allowing the
         * variable to be fully initialised once the define is
         * complete. */

        if (last->is_list) {
            Node *first = last->list->at(0);
            if (first && first->is_token 
                      && !(first->token->str_value.compare("init"))) {
                return true;
            }
        }

        if (!(type->isIntegerType()) && (type->base_type != BaseType::Bool)) {
            if (llvm::ConstantInt *temp =
                        llvm::dyn_cast<llvm::ConstantInt>(p.value)) {
                if (temp->getValue().getLimitedValue() == 0) {
                    pr->block = p.block;
                    return true;
                }
            }
        }

        /* Handle arrays that were given a length of 0. */
        if (is_zero_sized) {
            type = p.type;
            var2->type = type;
            et = ctx->toLLVMType(type, (*newlist)[2], false, false);
            new_ptr = llvm::cast<llvm::Value>(
                          builder.CreateAlloca(et)
                      );
            var2->value = new_ptr;
        }

        llvm::IRBuilder<> builder2(p.block);

        std::vector<Type *> call_arg_types;
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        Function *or_setf =
            ctx->getFunction("setf-copy", &call_arg_types,
                             NULL, 0);
        if (or_setf && type->isEqualTo(p.type)) {
            std::vector<llvm::Value *> call_args2;
            call_args2.push_back(new_ptr);
            llvm::Value *new_ptr2 =
                llvm::cast<llvm::Value>(
                    builder2.CreateAlloca(ctx->toLLVMType(type, NULL,
                                                          false, false))
                );
            builder2.CreateStore(p.value, new_ptr2);
            call_args2.push_back(new_ptr2);
            builder2.CreateCall(
                or_setf->llvm_function,
                llvm::ArrayRef<llvm::Value*>(call_args2));
        } else {
            call_arg_types.clear();
            call_arg_types.push_back(ctx->tr->getPointerType(type));
            call_arg_types.push_back(p.type);
            Function *or_setf2 =
                ctx->getFunction("setf-copy", &call_arg_types,
                                 NULL, 0);
            if (or_setf2) {
                std::vector<llvm::Value *> call_args2;
                call_args2.push_back(new_ptr);
                call_args2.push_back(p.value);
                builder2.CreateCall(
                    or_setf2->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args2));
            } else {
                int old_const = p.type->is_const;
                p.type->is_const = 0;
                bool res = ctx->er->assertTypeEquality("def", node,
                                                       p.type, type, 1);
                p.type->is_const = old_const;
                if (!res) {
                    return false;
                }
                builder2.CreateStore(p.value, new_ptr);
            }
        }
        ParseResult temp;
        bool mres = Operation::Destruct(ctx, &p, &temp);
        if (!mres) {
            return false;
        }

        pr->block = temp.block;
        return true;
    }
}
}
