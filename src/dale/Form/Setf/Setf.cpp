#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Setf
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
        gen->parseFunctionBodyInstr(fn, block, (*lst)[1], false, NULL,
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

    /* Previously: 'if pr_value is a variable, use the value
     * directly, rather than calling PFBI and getting a copy'.
     * That only worked because there was a branch further down
     * that accepted two pointers-to-type as the arguments to this
     * form, and set the destination to be the value of the
     * source, directly. That form of call is no longer supported,
     * because it was extremely error-prone and unintuitive. So
     * this will now cause additional copies, which is
     * unfortunate; there is probably a way to make it better. */

    llvm::IRBuilder<> builder(pr_variable.block);
    ParseResult pr_value;
    res =
        gen->parseFunctionBodyInstr(
            fn, pr_variable.block, val_node, false,
            pr_variable.type->points_to,
            &pr_value
        );

    if (!res) {
        return NULL;
    }

    builder.SetInsertPoint(pr_value.block);

    /* If overridden setf exists, and pr_value is a value of the
     * pointee type of pr_variable, then call overridden setf
     * after allocating memory for pr_value and copying it into
     * place. */

    if (!prefixed_with_core
            && pr_value.type->isEqualTo(pr_variable.type->points_to)) {
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
        pr->set(temp.block, ctx->tr->getBasicType(Type::Bool), ret);
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

        pr->set(temp.block, ctx->tr->getBasicType(Type::Bool), ret);
        return true;
    }

cont2:

    if (pr_value.type->isEqualTo(pr_variable.type->points_to)) {
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

        pr->set(temp.block, ctx->tr->getBasicType(Type::Bool), 
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
