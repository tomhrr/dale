#include "Setf.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Copy/Copy.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Utils/Utils.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool destructTwo(Context *ctx, ParseResult *one, ParseResult *two,
                 ParseResult *three, llvm::IRBuilder<> *builder) {
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

Function *getCopyAssign(Context *ctx, Type *dst_type, Type *src_type) {
    std::vector<Type *> types;
    types.push_back(dst_type);
    types.push_back(src_type);
    return ctx->getFunction("setf-copy-assign", &types, NULL, false);
}

bool processCopyPtrCall(Context *ctx, Function *over_setf,
                        llvm::IRBuilder<> *builder,
                        ParseResult *variable_pr, ParseResult *value_pr,
                        ParseResult *pr) {
    llvm::Value *src_ptr =
        llvm::cast<llvm::Value>(builder->CreateAlloca(
            ctx->toLLVMType(value_pr->type, NULL, false, false)));
    builder->CreateStore(value_pr->getValue(ctx), src_ptr);

    std::vector<llvm::Value *> call_args;
    call_args.push_back(variable_pr->getValue(ctx));
    call_args.push_back(src_ptr);
    llvm::Value *ret =
        builder->CreateCall(over_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value *>(call_args));

    ParseResult destruct_pr;
    bool res =
        destructTwo(ctx, variable_pr, value_pr, &destruct_pr, builder);
    if (!res) {
        return false;
    }

    pr->set(destruct_pr.block, ctx->tr->type_bool, ret);
    return true;
}

bool processCopyExactCall(Context *ctx, Function *over_setf,
                          llvm::IRBuilder<> *builder,
                          ParseResult *variable_pr,
                          ParseResult *value_pr, ParseResult *pr) {
    std::vector<llvm::Value *> call_args;
    call_args.push_back(variable_pr->getValue(ctx));
    call_args.push_back(value_pr->getValue(ctx));
    llvm::Value *ret =
        builder->CreateCall(over_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value *>(call_args));

    ParseResult destruct_pr;
    bool res =
        destructTwo(ctx, variable_pr, value_pr, &destruct_pr, builder);
    if (!res) {
        return false;
    }

    pr->set(destruct_pr.block, ctx->tr->type_bool, ret);
    return true;
}

Function *getMoveAssign(Context *ctx, Type *dst_type, Type *src_type) {
    std::vector<Type *> types;
    types.push_back(dst_type);
    types.push_back(src_type);
    std::vector<bool> lvalues;
    lvalues.push_back(true);
    lvalues.push_back(false);
    return ctx->getFunction("setf-move-assign", &types, NULL, false,
                            &lvalues);
}

bool processMovePtrCall(Context *ctx, Function *over_setf,
                        llvm::IRBuilder<> *builder,
                        ParseResult *variable_pr, ParseResult *value_pr,
                        ParseResult *pr) {
    ParseResult value_ptr_pr;
    bool res = value_pr->getAddressOfValue(ctx, &value_ptr_pr);
    if (!res) {
        return false;
    }

    std::vector<llvm::Value *> call_args;
    call_args.push_back(variable_pr->getValue(ctx));
    call_args.push_back(value_ptr_pr.getValue(ctx));
    llvm::Value *ret =
        builder->CreateCall(over_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value *>(call_args));

    res = Operation::Destruct(ctx, variable_pr, pr, builder);
    if (!res) {
        return false;
    }

    pr->set(pr->block, ctx->tr->type_bool, ret);
    return true;
}

bool FormProcSetfProcess(Units *units, Function *fn,
                         llvm::BasicBlock *block, Node *node,
                         Node *value_node, bool get_address,
                         bool prefixed_with_core,
                         ParseResult *variable_pr,
                         ParseResult *value_pr, ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    llvm::IRBuilder<> builder(block);

    if (!prefixed_with_core) {
        Function *over_setf =
            getMoveAssign(ctx, variable_pr->type, value_pr->type);
        if (over_setf && !value_pr->value_is_lvalue) {
            return processMovePtrCall(ctx, over_setf, &builder,
                                      variable_pr, value_pr, pr);
        }
    }

    if (!Operation::IsCopyPermitted(ctx, node,
                                    variable_pr->type->points_to)) {
        return false;
    }

    /* If an overridden setf exists, and value_pr is a value of the
     * pointee type of variable_pr, then call the overridden setf
     * after allocating memory for value_pr and copying it into place.
     * */

    if (!prefixed_with_core &&
        variable_pr->type->points_to->canBeSetFrom(value_pr->type)) {
        Function *over_setf =
            getCopyAssign(ctx, variable_pr->type, variable_pr->type);
        if (over_setf) {
            return processCopyPtrCall(ctx, over_setf, &builder,
                                      variable_pr, value_pr, pr);
        }
    }

    /* If an appropriate setf definition exists, which matches
     * the arguments exactly, then use it. */

    if (!prefixed_with_core) {
        Function *over_setf =
            getCopyAssign(ctx, variable_pr->type, value_pr->type);
        if (over_setf) {
            return processCopyExactCall(ctx, over_setf, &builder,
                                        variable_pr, value_pr, pr);
        }
    }

    if (variable_pr->type->points_to->canBeSetFrom(value_pr->type)) {
        builder.CreateStore(value_pr->getValue(ctx),
                            variable_pr->getValue(ctx));

        ParseResult destruct_pr;
        bool res = destructTwo(ctx, variable_pr, value_pr, &destruct_pr,
                               &builder);
        if (!res) {
            return false;
        }

        pr->set(destruct_pr.block, ctx->tr->type_bool,
                ctx->nt->getLLVMTrue());
        pr->do_not_copy_with_setf = true;
        return true;
    }

    /* This is used to set an error message. */
    ctx->er->assertTypeEquality("setf", value_node, value_pr->type,
                                variable_pr->type->points_to, false);
    return false;
}

bool FormProcSetfParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;

    if (!ctx->er->assertArgNums("setf", node, 2, 2)) {
        return false;
    }

    ParseResult variable_pr;
    bool res = FormProcInstParse(units, fn, block, (*lst)[1], false,
                                 false, NULL, &variable_pr);
    if (!res) {
        return false;
    }

    if (!variable_pr.type->points_to) {
        Error *e = new Error(IncorrectArgType, (*lst)[1], "setf",
                             "a pointer", "1", "a value");
        ctx->er->addError(e);
        return false;
    }

    if (variable_pr.type->points_to->is_const) {
        Error *e = new Error(CannotModifyConstVariable, node);
        ctx->er->addError(e);
        return false;
    }

    Node *value_node = (*lst)[2];
    llvm::IRBuilder<> builder(variable_pr.block);
    ParseResult value_pr;
    value_pr.retval = variable_pr.getValue(ctx);
    value_pr.retval_type = variable_pr.type;

    res = FormProcInstParse(units, fn, block, value_node, get_address,
                            false, variable_pr.type->points_to,
                            &value_pr);
    if (!res) {
        return false;
    }
    if (value_pr.retval_used) {
        pr->block = value_pr.block;
        pr->type = ctx->tr->getBasicType(BaseType::Void);
        return true;
    } else {
        value_pr.retval = NULL;
    }

    return FormProcSetfProcess(
        units, fn, value_pr.block, node, value_node, get_address,
        prefixed_with_core, &variable_pr, &value_pr, pr);
}
}
