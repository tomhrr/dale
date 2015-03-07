#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../Linkage/Linkage.h"
#include "../../Type/Type.h"
#include "../../Struct/Struct.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
Function *
get_init_fn(Context *ctx, Type *type)
{
    std::vector<Type *> init_arg_types;
    init_arg_types.push_back(type);
    return ctx->getFunction("init", &init_arg_types, NULL, 0);
}

bool
initialise(Context *ctx, llvm::IRBuilder<> *builder, Type *type,
           llvm::Value *value, Function *init_fn)
{
    if (!init_fn) {
        init_fn = get_init_fn(ctx, type);
    }

    if (init_fn) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(value);
        builder->CreateCall(init_fn->llvm_function,
                            llvm::ArrayRef<llvm::Value*>(call_args));
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
                llvm::cast<llvm::Value>(ctx->nt->getNativeInt(i))
            );
            llvm::Value *aref = builder->Insert(
                llvm::GetElementPtrInst::Create(
                    value,
                    llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "aref"
            );
            initialise(ctx, builder, type->array_type, aref, init_fn);
            indices.pop_back();
        }

        return true;
    }

    if (type->struct_name.size()) {
        Struct *st = ctx->getStruct(type);
        std::vector<llvm::Value *> indices;
        indices.push_back(ctx->nt->getLLVMZero());
        int i = 0;
        for (std::vector<Type *>::iterator b = st->member_types.begin(),
                                           e = st->member_types.end();
                b != e;
                ++b) {
            Type *type = (*b);
            indices.push_back(
                llvm::cast<llvm::Value>(
                    ctx->nt->getNativeInt(i++)
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
            initialise(ctx, builder, type, sref, NULL);
        }
        return true;
    }

    return true;
}

bool
processValue(Units *units, Function *fn, llvm::BasicBlock *block,
             Node *node, bool get_address, Type *wanted_type,
             ParseResult *pr)
{
    Context *ctx = units->top()->ctx;
    std::vector<Node *> *value_node_list = node->list->at(2)->list;

    Node *var_value_node = (*value_node_list)[3];
    std::vector<Node *> *var_value_node_list = var_value_node->list;
    Variable *var_value = NULL;
    llvm::IRBuilder<> builder(block);

    if (var_value_node->is_token
            && (var_value = ctx->getVariable(
                    var_value_node->token->str_value.c_str()))) {
        pr->set(block, var_value->type,
                builder.CreateLoad(var_value->value));
        pr->do_not_destruct = true;
    } else if (var_value_node->is_list
            && (var_value_node_list->size() == 2)
            && (var_value_node_list->at(0)->is_token)
            && (!var_value_node_list->at(0)->token->str_value.compare("@"))
            && (var_value_node_list->at(1)->is_token)
            && (var_value = ctx->getVariable(
                    var_value_node_list->at(1)->token->str_value.c_str()))) {
        pr->set(block, var_value->type->points_to,
                builder.CreateLoad(builder.CreateLoad(var_value->value)));
        pr->do_not_destruct = true;
    } else {
        bool res =
            FormProcInstParse(units,
                fn, block, var_value_node, get_address, false,
                wanted_type, pr
            );
        if (!res) {
            return false;
        }
    }

    return true;
}

bool
storeValue(Context *ctx, Node *node, Type *type,
           llvm::IRBuilder<> *builder, llvm::Value *dst_ptr, ParseResult *pr)
{
    std::vector<Type *> param_types;
    param_types.push_back(ctx->tr->getPointerType(type));
    param_types.push_back(ctx->tr->getPointerType(type));
    Function *or_setf =
        ctx->getFunction("setf-copy", &param_types, NULL, 0);

    if (or_setf && type->isEqualTo(pr->type)) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(dst_ptr);

        llvm::Value *src_ptr =
            llvm::cast<llvm::Value>(
                builder->CreateAlloca(ctx->toLLVMType(type, NULL,
                                                      false, false))
            );
        builder->CreateStore(pr->value, src_ptr);
        call_args.push_back(src_ptr);

        builder->CreateCall(or_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value*>(call_args));
        return true;
    }

    param_types.pop_back();
    param_types.push_back(pr->type);

    or_setf =
        ctx->getFunction("setf-copy", &param_types, NULL, 0);
    if (or_setf) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(dst_ptr);
        call_args.push_back(pr->value);
        builder->CreateCall(or_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value*>(call_args));
        return true;
    }

    bool old_const = pr->type->is_const;
    pr->type->is_const = false;
    bool res = ctx->er->assertTypeEquality("def", node,
                                           pr->type, type, 1);
    pr->type->is_const = old_const;
    if (!res) {
        return false;
    }

    builder->CreateStore(pr->value, dst_ptr);
    return true;
}

bool
parseImpliedVarDefinition(Units *units, Function *fn, llvm::BasicBlock *block,
                          const char *name, Node *node,
                          bool get_address, int linkage, ParseResult *pr)
{
    Context *ctx = units->top()->ctx;
    Node *value_node = node->list->at(2);
    std::vector<Node *> *value_node_list = value_node->list;

    if (value_node_list->size() != 4) {
        Error *e = new Error(MustHaveInitialiserForImpliedType,
                             value_node);
        ctx->er->addError(e);
        return false;
    }

    ParseResult p;
    bool res = processValue(units, fn, block, node, get_address,
                            NULL, &p);
    if (!res) {
        return false;
    }

    Type *type = p.type;
    block = p.block;
    llvm::IRBuilder<> builder(block);

    llvm::Type *et = ctx->toLLVMType(type, (*value_node_list)[2],
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
        Error *e = new Error(RedefinitionOfVariable, node, name);
        ctx->er->addError(e);
        return false;
    }

    if (p.retval_used) {
        var2->value = p.retval;
        pr->block = p.block;
        return true;
    }

    /* If the constant int 0 is returned, and this isn't an
        * integer type (or bool), then skip this part (assume that
        * the variable has been initialised by the user).  This is to
        * save pointless copies/destructs, while still allowing the
        * variable to be fully initialised once the define is
        * complete. */

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

    res = storeValue(ctx, node, type, &builder, new_ptr, &p);
    if (!res) {
        return false;
    }
    ParseResult temp;
    bool mres = Operation::Destruct(ctx, &p, &temp);
    if (!mres) {
        return false;
    }

    pr->block = temp.block;
    return true;
}

bool
parseExplicitVarDefinition(Units *units, Function *fn, llvm::BasicBlock *block,
                           const char *name, Node *node,
                           bool get_address, int linkage, ParseResult *pr)
{
    Context *ctx = units->top()->ctx;
    Node *value_node = node->list->at(2);
    std::vector<Node *> *value_node_list = value_node->list;

    /* Parse the type. */
    Type *type = FormTypeParse(units, (*value_node_list)[2], false, false);
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
        Struct *st = ctx->getStruct(type);
        if (st->must_init && (value_node_list->size() == 3) && !init_fn) {
            Error *e = new Error(MustHaveInitialiserForType, value_node);
            ctx->er->addError(e);
            return false;
        }
    }

    bool is_zero_sized = (type->array_type && (type->array_size == 0));

    /* Add an alloca instruction for this variable. */

    llvm::IRBuilder<> builder(block);
    llvm::Type *et = ctx->toLLVMType(type, (*value_node_list)[2], false,
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
        Error *e = new Error(RedefinitionOfVariable, node, name);
        ctx->er->addError(e);
        return false;
    }

    if (value_node_list->size() == 3) {
        if (type->is_const && !init_fn) {
            Error *e = new Error(MustHaveInitialiserForConstType,
                                    value_node);
            ctx->er->addError(e);
            return false;
        }

        initialise(ctx, &builder, type, new_ptr, init_fn);

        pr->set(block, ctx->tr->type_int,
                llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));

        return true;
    }

    ParseResult p;
    p.retval      = new_ptr;
    p.retval_type = ctx->tr->getPointerType(type);
    bool res = processValue(units, fn, block, node, get_address,
                            type, &p);
    if (!res) {
        return false;
    }

    /* If the retval was used, then there's no need for anything
        * following. */

    if (p.retval_used) {
        pr->block = p.block;
        return true;
    }

    Node *last = (*value_node_list)[3];
    if (last->is_list) {
        Node *first = last->list->at(0);
        if (first && first->is_token
                && !(first->token->str_value.compare("init"))) {
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
        et = ctx->toLLVMType(type, (*value_node_list)[2], false, false);
        new_ptr = llvm::cast<llvm::Value>(
                    builder.CreateAlloca(et)
                );
        var2->value = new_ptr;
    }

    llvm::IRBuilder<> builder2(p.block);

    res = storeValue(ctx, node, type, &builder2, new_ptr, &p);
    if (!res) {
        return false;
    }
    ParseResult temp;
    bool mres = Operation::Destruct(ctx, &p, &temp);
    if (!mres) {
        return false;
    }

    pr->block = temp.block;
    return true;
}

bool
parseVarDefinition(Units *units, Function *fn, llvm::BasicBlock *block,
                   const char *name, Node *node,
                   bool get_address, ParseResult *pr)
{
    Context *ctx = units->top()->ctx;
    Node *value_node = node->list->at(2);
    std::vector<Node *> *value_node_list = value_node->list;

    /* Parse linkage. */

    int linkage = FormLinkageParse(ctx, (*value_node_list)[1]);
    if (!linkage) {
        return false;
    }

    if ((linkage != Linkage::Auto)
            && (linkage != Linkage::Intern)
            && (value_node_list->size() > 3)) {
        Error *e = new Error(HasBothExternAndInitialiser, value_node);
        ctx->er->addError(e);
        return false;
    }

    pr->set(block, ctx->tr->type_int,
        llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));
    pr->do_not_destruct       = true;
    pr->do_not_copy_with_setf = true;

    /* Check if the type is a single token string equal to "\". If it
     * is, then the type is implied based on the result of parsing the
     * later expression. */

    if ((*value_node_list)[2]->is_token &&
            !(*value_node_list)[2]->token->str_value.compare("\\")) {
        return parseImpliedVarDefinition(units, fn, block, name,
                                         node, get_address, linkage,
                                         pr);
    } else {
        return parseExplicitVarDefinition(units, fn, block, name,
                                          node, get_address, linkage,
                                          pr);
    }
}

bool
FormProcDefParse(Units *units, Function *fn, llvm::BasicBlock *block,
                 Node *node, bool get_address, bool prefixed_with_core,
                 ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("def", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *name_node = (*lst)[1];

    if (!ctx->er->assertArgIsAtom("def", name_node, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("def", name_node, "1")) {
        return false;
    }

    Token *t = name_node->token;
    const char *name = t->str_value.c_str();

    Node *value_node = (*lst)[2];
    if (!ctx->er->assertArgIsList("def", value_node, "2")) {
        return false;
    }
    std::vector<Node *> *value_node_list = value_node->list;
    if (value_node_list->size() == 0) {
        Error *e = new Error(NoEmptyLists, value_node);
        ctx->er->addError(e);
        return false;
    }

    Node *def_type = value_node_list->at(0);
    if (!(def_type->token->str_value.compare("struct"))) {
        FormStructParse(units, value_node, name);
        pr->set(block, ctx->tr->type_int,
              llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));
        return true;
    } else if (!(def_type->token->str_value.compare("var"))) {
        return parseVarDefinition(units, fn, block, name, node,
                                  get_address, pr);
    } else {
        Error *e = new Error(
            OnlyVarAndStructPermitted,
            value_node
        );
        ctx->er->addError(e);
        return false;
    }
}
}
