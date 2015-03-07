#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../Inst/Inst.h"
#include "../../Utils/Utils.h"
#include "../../../llvm_Function.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool
destructTwo(Context *ctx, ParseResult *one, ParseResult *two,
            ParseResult *three, llvm::IRBuilder<> *builder)
{
    one->block = two->block;
    bool res = Operation::Destruct(ctx, one, three, builder);
    if (!res) {
        return false;
    }
    two->block = three->block;
    res = Operation::Destruct(ctx, two, three, builder);
    if (!res) {
        return false;
    }
    return true;
}

Function *
getSetfAssign(Context *ctx, Type *dst_type, Type *src_type)
{
    std::vector<Type *> types;
    types.push_back(dst_type);
    types.push_back(src_type);
    return ctx->getFunction("setf-assign", &types, NULL, 0);
}

bool
processSetfPtrCall(Context *ctx, Function *over_setf,
                   llvm::IRBuilder<> *builder,
                   ParseResult *pr_variable, ParseResult *pr_value,
                   ParseResult *pr)
{
    llvm::Value *src_ptr =
        llvm::cast<llvm::Value>(
            builder->CreateAlloca(
                ctx->toLLVMType(pr_value->type, NULL, false, false)
            )
        );
    builder->CreateStore(pr_value->value, src_ptr);

    std::vector<llvm::Value *> call_args;
    call_args.push_back(pr_variable->value);
    call_args.push_back(src_ptr);
    llvm::Value *ret =
        builder->CreateCall(over_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value*>(call_args));

    ParseResult pr_destruct;
    bool res = destructTwo(ctx, pr_variable, pr_value, &pr_destruct, builder);
    if (!res) {
        return false;
    }

    pr->set(pr_destruct.block, ctx->tr->type_bool, ret);
    return true;
}

bool
processSetfExactCall(Context *ctx, Function *over_setf,
                     llvm::IRBuilder<> *builder,
                     ParseResult *pr_variable, ParseResult *pr_value,
                     ParseResult *pr)
{
    std::vector<llvm::Value *> call_args;
    call_args.push_back(pr_variable->value);
    call_args.push_back(pr_value->value);
    llvm::Value *ret =
        builder->CreateCall(over_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value*>(call_args));

    ParseResult pr_destruct;
    bool res = destructTwo(ctx, pr_variable, pr_value, &pr_destruct, builder);
    if (!res) {
        return false;
    }

    pr->set(pr_destruct.block, ctx->tr->type_bool, ret);
    return true;
}

bool
FormProcSetfParse(Units *units, Function *fn, llvm::BasicBlock *block,
                  Node *node, bool get_address, bool prefixed_with_core,
                  ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;

    if (!ctx->er->assertArgNums("setf", node, 2, 2)) {
        return false;
    }

    ParseResult pr_variable;
    bool res =
        FormProcInstParse(units, fn, block, (*lst)[1], false,
                          false, NULL, &pr_variable);
    if (!res) {
        return false;
    }

    if (!pr_variable.type->points_to) {
        Error *e = new Error(IncorrectArgType, (*lst)[1],
                             "setf", "a pointer", "1", "a value");
        ctx->er->addError(e);
        return false;
    }

    if (pr_variable.type->points_to->is_const) {
        Error *e = new Error(CannotModifyConstVariable, node);
        ctx->er->addError(e);
        return false;
    }

    Node *value_node = (*lst)[2];
    value_node = units->top()->mp->parsePotentialMacroCall(value_node);
    if (!value_node) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_variable.block);
    ParseResult pr_value;
    pr_value.retval = pr_variable.value;
    pr_value.retval_type = pr_variable.type;

    res = FormProcessValue(units, fn, block, value_node, get_address,
                           pr_variable.type->points_to, &pr_value);
    if (!res) {
        return false;
    }
    if (pr_value.retval_used) {
        pr->block = pr_value.block;
        pr->type = ctx->tr->getBasicType(BaseType::Void);
        return true;
    }

    builder.SetInsertPoint(pr_value.block);

    /* If an overridden setf exists, and pr_value is a value of the
     * pointee type of pr_variable, then call the overridden setf
     * after allocating memory for pr_value and copying it into place.
     * */

    if (!prefixed_with_core
            && pr_variable.type->points_to->canBeSetFrom(pr_value.type)) {
        Function *over_setf = getSetfAssign(ctx, pr_variable.type,
                                            pr_variable.type);
        if (over_setf) {
            return processSetfPtrCall(ctx, over_setf, &builder,
                                      &pr_variable, &pr_value, pr);
        }
    }

    /* If an appropriate setf definition exists, which matches
     * the arguments exactly, then use it. */

    if (!prefixed_with_core) {
        Function *over_setf = getSetfAssign(ctx, pr_variable.type,
                                            pr_value.type);
        if (over_setf) {
            return processSetfExactCall(ctx, over_setf, &builder,
                                        &pr_variable, &pr_value, pr);
        }
    }

    if (pr_variable.type->points_to->canBeSetFrom(pr_value.type)) {
        builder.CreateStore(pr_value.value, pr_variable.value);

        ParseResult pr_destruct;
        bool res = destructTwo(ctx, &pr_variable, &pr_value,
                               &pr_destruct, &builder);
        if (!res) {
            return false;
        }

        pr->set(pr_destruct.block, ctx->tr->type_bool,
                llvm::ConstantInt::get(
                    llvm::IntegerType::get(llvm::getGlobalContext(), 1), 1
                ));
        pr->do_not_copy_with_setf = true;
        return true;
    }

    /* This is used to set an error message. */
    ctx->er->assertTypeEquality("setf", (*lst)[2],
                                pr_value.type,
                                pr_variable.type->points_to,
                                false);
    return false;
}
}
