#include "Def.h"

#include <string>
#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Copy/Copy.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Linkage/Linkage.h"
#include "../../Struct/Struct.h"
#include "../../TopLevel/GlobalVariable/GlobalVariable.h"
#include "../../Type/Type.h"
#include "../../Utils/Utils.h"
#include "../../Value/Value.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
Function *getInitFn(Context *ctx, Type *type) {
    std::vector<Type *> init_arg_types;
    init_arg_types.push_back(type);
    return ctx->getFunction("init", &init_arg_types, NULL, 0);
}

bool typeRequiresExplicitInit(Context *ctx, Type *type) {
    std::vector<Type *> init_arg_types;
    init_arg_types.push_back(type);
    Function *fn = ctx->getFunction("requires-explicit-init",
                                    &init_arg_types, NULL, 0);
    return (fn ? true : false);
}

llvm::Constant *parseGlobalLiteral(Units *units, Type *type,
                                   Node *node) {
    llvm::Constant *init = NULL;
    int size;
    init = FormValueParse(units, type, node, &size);

    return init;
}

bool initialise(Context *ctx, llvm::IRBuilder<> *builder, Type *type,
                llvm::Value *value, Function *init_fn) {
    if (!init_fn) {
        init_fn = getInitFn(ctx, type);
    }

    if (init_fn) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(value);
        builder->CreateCall(init_fn->llvm_function,
                            llvm::ArrayRef<llvm::Value *>(call_args));
        return true;
    }

    if (type->array_type) {
        init_fn = getInitFn(ctx, type->array_type);
        if (!init_fn) {
            return true;
        }

        std::vector<llvm::Value *> indices;
        indices.push_back(ctx->nt->getLLVMZero());
        for (int i = 0; i < type->array_size; i++) {
            indices.push_back(
                llvm::cast<llvm::Value>(ctx->nt->getNativeInt(i)));
            llvm::Value *aref = builder->Insert(
                createGEP(value,
                          llvm::ArrayRef<llvm::Value *>(indices)),
                "aref");
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
             b != e; ++b) {
            Type *stype = (*b);
            indices.push_back(
                llvm::cast<llvm::Value>(ctx->nt->getNativeInt(i++)));

            llvm::Value *sref = builder->Insert(
                createGEP(value, llvm::ArrayRef<llvm::Value *>(indices),
                          ctx->toLLVMType(type, NULL, false)),
                "sref");

            indices.pop_back();
            initialise(ctx, builder, stype, sref, NULL);
        }
        return true;
    }

    return true;
}

bool storeValue(Context *ctx, Node *node, Type *type,
                llvm::IRBuilder<> *builder, llvm::Value *dst_ptr,
                ParseResult *pr) {
    std::vector<Type *> param_types;
    param_types.push_back(ctx->tr->getPointerType(type));
    param_types.push_back(pr->type);
    std::vector<bool> lvalues;
    lvalues.push_back(true);
    lvalues.push_back(false);
    Function *or_setf_move = ctx->getFunction(
        "setf-move-init", &param_types, NULL, false, &lvalues);

    if (or_setf_move && !pr->value_is_lvalue) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(dst_ptr);

        ParseResult address_pr;
        bool res = pr->getAddressOfValue(ctx, &address_pr);
        if (!res) {
            return false;
        }
        llvm::Value *src_ptr = address_pr.getValue(ctx);
        call_args.push_back(src_ptr);

        builder->CreateCall(or_setf_move->llvm_function,
                            llvm::ArrayRef<llvm::Value *>(call_args));
        return true;
    }

    if (!Operation::IsCopyPermitted(ctx, node, type)) {
        return false;
    }

    param_types.pop_back();
    param_types.push_back(ctx->tr->getPointerType(type));
    Function *or_setf =
        ctx->getFunction("setf-copy-init", &param_types, NULL, false);

    if (or_setf && type->isEqualTo(pr->type)) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(dst_ptr);

        llvm::Value *src_ptr =
            llvm::cast<llvm::Value>(builder->CreateAlloca(
                ctx->toLLVMType(type, NULL, false, false)));
        builder->CreateStore(pr->getValue(ctx), src_ptr);
        call_args.push_back(src_ptr);

        builder->CreateCall(or_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value *>(call_args));
        return true;
    }

    param_types.pop_back();
    param_types.push_back(pr->type);

    or_setf =
        ctx->getFunction("setf-copy-init", &param_types, NULL, false);
    if (or_setf) {
        std::vector<llvm::Value *> call_args;
        call_args.push_back(dst_ptr);
        call_args.push_back(pr->getValue(ctx));
        builder->CreateCall(or_setf->llvm_function,
                            llvm::ArrayRef<llvm::Value *>(call_args));
        return true;
    }

    bool old_const = pr->type->is_const;
    pr->type->is_const = false;
    bool res =
        ctx->er->assertTypeEquality("def", node, pr->type, type, 1);
    pr->type->is_const = old_const;
    if (!res) {
        return false;
    }

    builder->CreateStore(pr->getValue(ctx), dst_ptr);
    return true;
}

bool parseImplicitVarDefinition(Units *units, Function *fn,
                                llvm::BasicBlock *block,
                                const char *name, Node *node,
                                bool get_address, int linkage,
                                ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    Node *value_node = (*node->list)[2];
    std::vector<Node *> *value_node_list = value_node->list;

    if (value_node_list->size() != 4) {
        Error *e =
            new Error(MustHaveInitialiserForImpliedType, value_node);
        ctx->er->addError(e);
        return false;
    }

    ParseResult value_pr;
    bool res =
        FormProcInstParse(units, fn, block, (*value_node->list)[3],
                          false, false, NULL, &value_pr);
    if (!res) {
        return false;
    }

    Type *type = value_pr.type;
    block = value_pr.block;
    llvm::Type *llvm_type = ctx->toLLVMType(type, (*value_node_list)[2],
                                            false, false, false);
    if (!llvm_type) {
        return false;
    }

    Variable *var = new Variable();
    var->name.append(name);
    var->type = type;
    var->linkage = linkage;
    std::string var_name;
    ctx->ns()->nameToSymbol(name, &var_name);

    llvm::IRBuilder<> builder(block);
    llvm::Value *dst_ptr =
        llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_type));
    var->value = dst_ptr;

    res = ctx->addVariable(name, var);
    if (!res) {
        Error *e = new Error(RedefinitionOfVariable, node, name);
        ctx->er->addError(e);
        return false;
    }

    if (value_pr.retval_used) {
        var->value = value_pr.retval;
        pr->block = value_pr.block;
        return true;
    }

    if (!ctx->er->assertTypeEquality("def", node, value_pr.type, type,
                                     1)) {
        return false;
    }

    if (linkage == Linkage::Auto) {
        res = storeValue(ctx, node, type, &builder, var->value,
                         &value_pr);
        if (!res) {
            return false;
        }
    }

    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &value_pr, &destruct_pr);
    if (!res) {
        return false;
    }

    pr->block = destruct_pr.block;
    return true;
}

bool parseExplicitVarDefinition(Units *units, Function *fn,
                                llvm::BasicBlock *block,
                                const char *name, Node *node,
                                bool get_address, int linkage,
                                ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    Node *value_node = (*node->list)[2];
    std::vector<Node *> *value_node_list = value_node->list;

    Type *type =
        FormTypeParse(units, (*value_node_list)[2], false, false);
    if (!type) {
        return false;
    }

    Function *init_fn = getInitFn(ctx, type);
    bool requires_explicit_init = typeRequiresExplicitInit(ctx, type);
    bool is_zero_sized = (type->array_type && (type->array_size == 0));

    llvm::IRBuilder<> builder(block);
    llvm::Type *llvm_type = ctx->toLLVMType(type, (*value_node_list)[2],
                                            false, false, true);
    if (!llvm_type) {
        return false;
    }

    if (linkage == Linkage::Auto) {
        llvm::Value *dst_ptr =
            llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_type));
        Variable *var = new Variable();
        var->name.append(name);
        var->type = type;
        var->value = dst_ptr;
        var->linkage = linkage;

        bool res = ctx->addVariable(name, var);
        if (!res) {
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
            if (requires_explicit_init && !init_fn) {
                Error *e =
                    new Error(MustHaveInitialiserForType, value_node);
                ctx->er->addError(e);
                return false;
            }

            initialise(ctx, &builder, type, dst_ptr, init_fn);
            pr->set(block, ctx->tr->type_int, ctx->nt->getLLVMZero());

            return true;
        }

        ParseResult value_pr;
        value_pr.retval = dst_ptr;
        value_pr.retval_type = ctx->tr->getPointerType(type);
        value_pr.retval_requires_init = true;
        res =
            FormProcInstParse(units, fn, block, (*value_node->list)[3],
                              get_address, false, type, &value_pr);
        if (!res) {
            return false;
        }

        if (value_pr.retval_used) {
            pr->block = value_pr.block;
            return true;
        }

        /* If the initialisation form is a list where the first token is
         * 'init', then skip this part (assume that the variable has
         * been
         * initialised by the user). This is to save pointless
         * copies/destructs, while still allowing the variable to be
         * fully
         * initialised once the define is complete. */

        Node *var_value_node = (*value_node_list)[3];
        if (var_value_node->is_list) {
            Node *first = (*var_value_node->list)[0];
            if (first && first->is_token &&
                !(first->token->str_value.compare("init"))) {
                return true;
            }
        }

        if (is_zero_sized) {
            type = value_pr.type;
            var->type = type;
            llvm_type = ctx->toLLVMType(type, (*value_node_list)[2],
                                        false, false);
            dst_ptr = llvm::cast<llvm::Value>(
                builder.CreateAlloca(llvm_type));
            var->value = dst_ptr;
        }

        builder.SetInsertPoint(value_pr.block);

        res = storeValue(ctx, node, type, &builder, dst_ptr, &value_pr);
        if (!res) {
            return false;
        }
        ParseResult destruct_pr;
        res = Operation::Destruct(ctx, &value_pr, &destruct_pr);
        if (!res) {
            return false;
        }

        pr->block = destruct_pr.block;
        return true;
    } else {
        Variable *var = new Variable();
        var->name.append(name);
        var->type = type;
        var->linkage = linkage;
        std::string var_name;
        ctx->ns()->nameToSymbol(name, &var_name);

        if (value_node_list->size() == 3) {
            if (type->is_const && !init_fn) {
                Error *e = new Error(MustHaveInitialiserForConstType,
                                     value_node);
                ctx->er->addError(e);
                return false;
            }
            if (requires_explicit_init && !init_fn) {
                Error *e =
                    new Error(MustHaveInitialiserForType, value_node);
                ctx->er->addError(e);
                return false;
            }

            pr->set(block, ctx->tr->type_int, ctx->nt->getLLVMZero());

            return true;
        }

        llvm::Constant *init =
            parseGlobalLiteral(units, type, (*value_node->list)[3]);
        if (!init) {
            return false;
        }

        llvm::IRBuilder<> builder(block);
        llvm::GlobalVariable *llvm_var =
            llvm::cast<llvm::GlobalVariable>(
                units->top()->module->getOrInsertGlobal(
                    var_name.c_str(), llvm_type));
        llvm_var->setLinkage(ctx->toLLVMLinkage(linkage));
        var->value = llvm::cast<llvm::Value>(llvm_var);

        llvm_var->setInitializer(init);

        bool res = ctx->addVariable(name, var);
        if (!res) {
            Error *e = new Error(RedefinitionOfVariable, node, name);
            ctx->er->addError(e);
            return false;
        }

        return true;
    }
}

bool parseVarDefinition(Units *units, Function *fn,
                        llvm::BasicBlock *block, const char *name,
                        Node *node, bool get_address, ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    Node *value_node = (*node->list)[2];
    std::vector<Node *> *value_node_list = value_node->list;

    int linkage = FormLinkageParse(ctx, (*value_node_list)[1]);
    if (!linkage) {
        return false;
    }

    if (value_node_list->size() > 4) {
        Error *e = new Error(IncorrectMaximumNumberOfArgs, value_node,
                             "var", "3", (value_node_list->size() - 1));
        units->top()->ctx->er->addError(e);
        return false;
    }

    if ((linkage != Linkage::Auto) && (linkage != Linkage::Intern) &&
        (value_node_list->size() > 3)) {
        Error *e = new Error(HasBothExternAndInitialiser, value_node);
        ctx->er->addError(e);
        return false;
    }

    pr->set(block, ctx->tr->type_int, ctx->nt->getLLVMZero());
    pr->do_not_destruct = true;
    pr->do_not_copy_with_setf = true;

    /* Check if the type is a single token string equal to "\". If it
     * is, then the type is implied based on the result of parsing the
     * later expression. */

    if ((linkage == Linkage::Auto) && (*value_node_list)[2]->is_token &&
        !(*value_node_list)[2]->token->str_value.compare("\\")) {
        return parseImplicitVarDefinition(units, fn, block, name, node,
                                          get_address, linkage, pr);
    } else {
        return parseExplicitVarDefinition(units, fn, block, name, node,
                                          get_address, linkage, pr);
    }
}

bool FormProcDefParse(Units *units, Function *fn,
                      llvm::BasicBlock *block, Node *node,
                      bool get_address, bool prefixed_with_core,
                      ParseResult *pr) {
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

    Node *def_type = (*value_node_list)[0];
    if (!def_type->token) {
        Error *e = new Error(UnexpectedElement, def_type, "symbol",
                             "def type", "list");
        ctx->er->addError(e);
        return false;
    }

    if (!(def_type->token->str_value.compare("struct"))) {
        FormStructParse(units, value_node, name);
        pr->set(block, ctx->tr->type_int, ctx->nt->getLLVMZero());
        return true;
    } else if (!(def_type->token->str_value.compare("var"))) {
        return parseVarDefinition(units, fn, block, name, node,
                                  get_address, pr);
    } else {
        Error *e = new Error(OnlyVarAndStructPermitted, value_node);
        ctx->er->addError(e);
        return false;
    }
}
}
