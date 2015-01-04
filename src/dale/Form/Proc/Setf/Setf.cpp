#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Setf
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "parseSetf must receive a list!");

    symlist *lst = node->list;

    if (!ctx->er->assertArgNums("setf", node, 2, 2)) {
        return false;
    }

    /* Used to use getAddress for the first argument, but now setf
     * always takes a pointer as its first argument, to facilitate
     * overloading etc. */

    ParseResult pr_variable;
    bool res =
        Form::Proc::Inst::parse(gen, fn, block, (*lst)[1], false, 
                                    false, NULL,
                                    &pr_variable);

    if (!res) {
        return false;
    }

    /* Make sure that the first argument is a pointer. */

    if (!pr_variable.type->points_to) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            (*lst)[1],
            "setf", "a pointer", "1", "a value"
        );
        ctx->er->addError(e);
        return false;
    }

    /* Can't modify const variables. */

    if (pr_variable.type->points_to->is_const) {
        Error *e = new Error(
            ErrorInst::Generator::CannotModifyConstVariable,
            node
        );
        ctx->er->addError(e);
        return false;
    }

    Node *val_node = (*lst)[2];
    val_node = gen->parseOptionalMacroCall(val_node);
    if (!val_node) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_variable.block);
    ParseResult pr_value;
    pr_value.retval = pr_variable.value;
    pr_value.retval_type = pr_variable.type;
    Element::Variable *var_value = NULL;
    symlist *vlst = val_node->list;

    /* If the value is a variable, or a variable dereference, load the
     * underlying value directly, to shortcut setf-copy for types that
     * define it. */

    if (val_node->is_token
            && (var_value = ctx->getVariable(
                    val_node->token->str_value.c_str()))) {
        pr_value.value = var_value->value;
        pr_value.type  = ctx->tr->getPointerType(var_value->type);
        pr_value.do_not_destruct = 1;
        pr_value.block = pr_variable.block;
    } else if (val_node->is_list
                && (vlst->size() == 2)
                && (vlst->at(0)->is_token)
                && (!vlst->at(0)->token->str_value.compare("@"))
                && (vlst->at(1)->is_token)
                && (var_value = ctx->getVariable(
                        vlst->at(1)->token->str_value.c_str()))) {
        pr_value.value = builder.CreateLoad(var_value->value);
        pr_value.type  = var_value->type;
        pr_value.do_not_destruct = 1;
        pr_value.block = pr_variable.block;
    } else {
        res =
            Form::Proc::Inst::parse(gen, 
                fn, pr_variable.block, val_node, false,
                false,
                pr_variable.type->points_to,
                &pr_value
            );

        if (!res) {
            return false;
        }
        if (pr_value.retval_used) {
            pr->block = pr_value.block;
            pr->type = ctx->tr->getBasicType(Type::Void);
            return true;
        }
    }

    builder.SetInsertPoint(pr_value.block);

    /* If overridden setf exists, and pr_value is a value of the
     * pointee type of pr_variable, then call overridden setf
     * after allocating memory for pr_value and copying it into
     * place. */

    if (!prefixed_with_core
            && pr_variable.type->points_to->canBeSetFrom(pr_value.type)) {
        std::vector<Element::Type *> types;
        types.push_back(pr_variable.type);
        types.push_back(pr_variable.type);
        Element::Function *over_setf =
            ctx->getFunction("setf-assign", &types, NULL, 0);
        if (!over_setf) {
            goto cont1;
        }
        llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(
                                        ctx->toLLVMType(pr_value.type, 
                                                        NULL, false,
                                                        false)
                                    )
                                );
        builder.CreateStore(pr_value.value, new_ptr2);
        std::vector<llvm::Value *> call_args;
        call_args.push_back(pr_variable.value);
        call_args.push_back(new_ptr2);
        llvm::Value *ret =
            builder.CreateCall(over_setf->llvm_function,
                               llvm::ArrayRef<llvm::Value*>(call_args));

        ParseResult temp;
        pr_variable.block = pr_value.block;
        bool mres = gen->destructIfApplicable(&pr_variable, &builder, &temp);
        if (!mres) {
            return false;
        }
        pr_value.block = temp.block;
        mres = gen->destructIfApplicable(&pr_value, &builder, &temp);
        if (!mres) {
            return false;
        }
        pr->set(temp.block, ctx->tr->type_bool, ret);
        return true;
    }

cont1:

    /* If an appropriate setf definition exists, which matches
     * the arguments exactly, then use it. */

    if (!prefixed_with_core) {
        std::vector<Element::Type *> types;
        types.push_back(pr_variable.type);
        types.push_back(pr_value.type);
        Element::Function *over_setf =
            ctx->getFunction("setf-assign", &types, NULL, 0);
        if (!over_setf) {
            goto cont2;
        }
        std::vector<llvm::Value *> call_args;
        call_args.push_back(pr_variable.value);
        call_args.push_back(pr_value.value);
        llvm::Value *ret =
            builder.CreateCall(over_setf->llvm_function,
                               llvm::ArrayRef<llvm::Value*>(call_args));

        ParseResult temp;
        pr_variable.block = pr_value.block;
        bool mres = gen->destructIfApplicable(&pr_variable, &builder, &temp);
        if (!mres) {
            return false;
        }
        pr_value.block = temp.block;
        mres = gen->destructIfApplicable(&pr_value, &builder, &temp);
        if (!mres) {
            return false;
        }

        pr->set(temp.block, ctx->tr->type_bool, ret);
        return true;
    }

cont2:

    /* var_value is only present to support the overridden set
     * operations: if this point is reached, then fall back to the
     * standard form-processing logic. */

    if (var_value) {
        res =
            Form::Proc::Inst::parse(gen, 
                fn, pr_value.block, val_node, false,
                false,
                pr_variable.type->points_to,
                &pr_value
            );

        if (!res) {
            return false;
        }
    }

    if (pr_variable.type->points_to->canBeSetFrom(pr_value.type)) {
        builder.CreateStore(pr_value.value, pr_variable.value);

        ParseResult temp;
        pr_variable.block = pr_value.block;
        bool mres = gen->destructIfApplicable(&pr_variable, &builder, &temp);
        if (!mres) {
            return false;
        }
        pr_value.block = temp.block;
        mres = gen->destructIfApplicable(&pr_value, &builder, &temp);
        if (!mres) {
            return false;
        }

        pr->set(temp.block, ctx->tr->type_bool, 
                llvm::ConstantInt::get(
                    llvm::IntegerType::get(llvm::getGlobalContext(), 1), 1
                ));
        pr->do_not_copy_with_setf = 1;
        return true;
    }

    /* todo: it would be good to also show the setf-assign
     * candidates here, if applicable. */
    ctx->er->assertTypeEquality("setf", (*lst)[2],
                            pr_value.type,
                            pr_variable.type->points_to,
                            false);

    return false;
}
}
}
}
}
