#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../Inst/Inst.h"
#include "../../Utils/Utils.h"
#include "../../../llvm_Function.h"

using namespace dale::ErrorInst;

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
                   ParseResult *variable_pr, ParseResult *value_pr,
                   ParseResult *pr)
{
    llvm::Value *src_ptr =
        llvm::cast<llvm::Value>(
            builder->CreateAlloca(
                ctx->toLLVMType(value_pr->type, NULL, false, false)
            )
        );
    builder->CreateStore(value_pr->value, src_ptr);

    std::vector<llvm::Value *> call_args;
    call_args.push_back(variable_pr->value);
    call_args.push_back(src_ptr);
    llvm::Value *ret =
        builder->CreateCall(over_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value*>(call_args));

    ParseResult destruct_pr;
    bool res = destructTwo(ctx, variable_pr, value_pr, &destruct_pr, builder);
    if (!res) {
        return false;
    }

    pr->set(destruct_pr.block, ctx->tr->type_bool, ret);
    return true;
}

bool
processSetfExactCall(Context *ctx, Function *over_setf,
                     llvm::IRBuilder<> *builder,
                     ParseResult *variable_pr, ParseResult *value_pr,
                     ParseResult *pr)
{
    std::vector<llvm::Value *> call_args;
    call_args.push_back(variable_pr->value);
    call_args.push_back(value_pr->value);
    llvm::Value *ret =
        builder->CreateCall(over_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value*>(call_args));

    ParseResult destruct_pr;
    bool res = destructTwo(ctx, variable_pr, value_pr, &destruct_pr, builder);
    if (!res) {
        return false;
    }

    pr->set(destruct_pr.block, ctx->tr->type_bool, ret);
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

    ParseResult variable_pr;
    bool res =
        FormProcInstParse(units, fn, block, (*lst)[1], false,
                          false, NULL, &variable_pr);
    if (!res) {
        return false;
    }

    if (!variable_pr.type->points_to) {
        Error *e = new Error(IncorrectArgType, (*lst)[1],
                             "setf", "a pointer", "1", "a value");
        ctx->er->addError(e);
        return false;
    }

    if (variable_pr.type->points_to->is_const) {
        Error *e = new Error(CannotModifyConstVariable, node);
        ctx->er->addError(e);
        return false;
    }

    Node *value_node = (*lst)[2];
    value_node = units->top()->mp->parsePotentialMacroCall(value_node);
    if (!value_node) {
        return false;
    }

    llvm::IRBuilder<> builder(variable_pr.block);
    ParseResult value_pr;
    value_pr.retval = variable_pr.value;
    value_pr.retval_type = variable_pr.type;

    res = FormProcessValue(units, fn, block, value_node, get_address,
                           variable_pr.type->points_to, &value_pr);
    if (!res) {
        return false;
    }
    if (value_pr.retval_used) {
        pr->block = value_pr.block;
        pr->type = ctx->tr->getBasicType(BaseType::Void);
        return true;
    }

    builder.SetInsertPoint(value_pr.block);

    /* If an overridden setf exists, and value_pr is a value of the
     * pointee type of variable_pr, then call the overridden setf
     * after allocating memory for value_pr and copying it into place.
     * */

    if (!prefixed_with_core
            && variable_pr.type->points_to->canBeSetFrom(value_pr.type)) {
        Function *over_setf = getSetfAssign(ctx, variable_pr.type,
                                            variable_pr.type);
        if (over_setf) {
            return processSetfPtrCall(ctx, over_setf, &builder,
                                      &variable_pr, &value_pr, pr);
        }
    }

    /* If an appropriate setf definition exists, which matches
     * the arguments exactly, then use it. */

    if (!prefixed_with_core) {
        Function *over_setf = getSetfAssign(ctx, variable_pr.type,
                                            value_pr.type);
        if (over_setf) {
            return processSetfExactCall(ctx, over_setf, &builder,
                                        &variable_pr, &value_pr, pr);
        }
    }

    if (variable_pr.type->points_to->canBeSetFrom(value_pr.type)) {
        builder.CreateStore(value_pr.value, variable_pr.value);

        ParseResult destruct_pr;
        bool res = destructTwo(ctx, &variable_pr, &value_pr,
                               &destruct_pr, &builder);
        if (!res) {
            return false;
        }

        pr->set(destruct_pr.block, ctx->tr->type_bool,
                llvm::ConstantInt::get(
                    llvm::IntegerType::get(llvm::getGlobalContext(), 1), 1
                ));
        pr->do_not_copy_with_setf = true;
        return true;
    }

    /* This is used to set an error message. */
    ctx->er->assertTypeEquality("setf", (*lst)[2],
                                value_pr.type,
                                variable_pr.type->points_to,
                                false);
    return false;
}
}
