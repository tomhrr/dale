#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Def
{
bool execute(Generator *gen,
             Element::Function *fn,
             llvm::BasicBlock *block,
             Node *node,
             bool get_address,
             bool prefixed_with_core,
             ParseResult *pr)
{
    Context *ctx = gen->ctx;
    Element::Type *type_int = ctx->tr->getBasicType(Type::Int);

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
        gen->parseStructDefinition(name, ndef);
        pr->set(block, type_int,
              llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));
        return true;
    }

    symlist *newlist = ndef->list;
    nvar = (*newlist)[0];

    /* Parse linkage. */

    int linkage = gen->parseLinkage((*newlist)[1]);
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

    pr->set(block, type_int,
          llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));
    pr->do_not_destruct       = 1;
    pr->do_not_copy_with_setf = 1;

    Element::Type *type;

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
        bool res =
            gen->parseFunctionBodyInstr(
                fn, block, (*newlist)[3], get_address, NULL, &p
            );
        if (!res) {
            return false;
        }
        type  = p.type;
        block = p.block;

        llvm::IRBuilder<> builder(block);
        llvm::Type *et = ctx->toLLVMType(type, (*newlist)[2], false, false);
        if (!et) {
            return false;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(et)
                               );
        Element::Variable *var2 = new Element::Variable();
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

        /* If the constant int 0 is returned, and this isn't an
         * integer type (or bool), then skip this part (assume
         * that the variable has been initialised by the user).
         * This is to save pointless copies/destructs, while still
         * allowing the variable to be fully initialised once the
         * define is complete. */

        if (!(type->isIntegerType()) && (type->base_type != Type::Bool)) {
            if (llvm::ConstantInt *temp =
                        llvm::dyn_cast<llvm::ConstantInt>(p.value)) {
                if (temp->getValue().getLimitedValue() == 0) {
                    pr->block = p.block;
                    return true;
                }
            }
        }

        if (!ctx->er->assertTypeEquality("def", node, p.type, type, 1)) {
            return NULL;
        }

        std::vector<Element::Type *> call_arg_types;
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        Element::Function *or_setf =
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
            Element::Function *or_setf2 =
                ctx->getFunction("setf-copy", &call_arg_types,
                                 NULL, 0);
            if (or_setf2) {
                std::vector<llvm::Value *> call_args2;
                call_args2.push_back(new_ptr);
                call_args2.push_back(p.value);
                builder.CreateCall(
                    or_setf->llvm_function,
                    llvm::ArrayRef<llvm::Value*>(call_args2));
            } else {
                builder.CreateStore(p.value, new_ptr);
            }
        }
        ParseResult temp;
        bool mres = gen->destructIfApplicable(&p, NULL, &temp);
        if (!mres) {
            return false;
        }

        pr->block = temp.block;
        return true;
    } else {
        /* Parse the type. */
        type = gen->parseType((*newlist)[2], false, false);
        if (!type) {
            return false;
        }
        /* If it's a struct, check if it's must-init. */
        if (type->struct_name) {
            Element::Struct *mine =
                ctx->getStruct(
                    type->struct_name->c_str(),
                    type->namespaces
                );
            if (mine->must_init && (newlist->size() == 3)) {
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
                                         false);
        if (!et) {
            return false;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(et)
                               );
        Element::Variable *var2 = new Element::Variable();
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
            if (type->is_const) {
                Error *e = new Error(
                    ErrorInst::Generator::MustHaveInitialiserForConstType,
                    ndef
                );
                ctx->er->addError(e);
                return false;
            }

            /* Finished - no value for this define. If the type is
             * const, though, it's an error. */

            pr->set(block, type_int,
                  llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));

            return true;
        }

        ParseResult p;
        bool res =
            gen->parseFunctionBodyInstr(
                fn, block, (*newlist)[3], get_address, type, &p
            );
        if (!res) {
            return false;
        }

        /* If the constant int 0 is returned, and this isn't an
         * integer type, then skip this part (assume that the
         * variable has been initialised by the user). This is to
         * save pointless copies/destructs, while still allowing
         * the variable to be fully initialised once the define is
         * complete. */

        if (!(type->isIntegerType()) && (type->base_type != Type::Bool)) {
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

        std::vector<Element::Type *> call_arg_types;
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        call_arg_types.push_back(ctx->tr->getPointerType(type));
        Element::Function *or_setf =
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
            Element::Function *or_setf2 =
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
                if (!ctx->er->assertTypeEquality("def", node, p.type, type, 1)) {
                    return false;
                }
                builder2.CreateStore(p.value, new_ptr);
            }
        }
        ParseResult temp;
        bool mres = gen->destructIfApplicable(&p, NULL, &temp);
        if (!mres) {
            return false;
        }

        pr->block = temp.block;
        return true;
    }
}
}
}
}
