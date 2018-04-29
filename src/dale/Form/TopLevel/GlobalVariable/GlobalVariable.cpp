#include "GlobalVariable.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../../Linkage/Linkage.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Copy/Copy.h"
#include "../../../Operation/Offsetof/Offsetof.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../Units/Units.h"
#include "../../../llvmUtils/llvmUtils.h"
#include "../../../Decoder/Decoder.h"
#include "../../Linkage/Linkage.h"
#include "../../ProcBody/ProcBody.h"
#include "../../Type/Type.h"
#include "../../Utils/Utils.h"
#include "../../Literal/Literal.h"
#include "Config.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/Debug.h"

using namespace dale::ErrorInst;

namespace dale {
llvm::Constant *simpleParseLiteral(Units *units, Type *type, Node *top,
                                   int *size) {
    Context *ctx = units->top()->ctx;
    TypeRegister *tr = ctx->tr;

    if (type->isIntegerType() && top->is_token &&
        (top->token->type == TokenType::Int)) {
        ParseResult pr;
        FormIntegerLiteralParse(ctx, type, NULL, top->token, &pr);
        return llvm::dyn_cast<llvm::Constant>(pr.getValue(ctx));
    } else if (type->isFloatingPointType() && top->is_token &&
               (top->token->type == TokenType::FloatingPoint)) {
        ParseResult pr;
        FormFloatingPointLiteralParse(ctx, type, NULL, top->token, &pr);
        return llvm::dyn_cast<llvm::Constant>(pr.getValue(ctx));
    } else if (top->is_token &&
               (top->token->type == TokenType::StringLiteral)) {
        std::string var_name;
        units->top()->getUnusedVarName(&var_name);

        *size = strlen(top->token->str_value.c_str()) + 1;

        Type *char_array_type = tr->getArrayType(tr->type_char, *size);
        llvm::Type *llvm_type =
            ctx->toLLVMType(char_array_type, NULL, false);

        llvm::Module *mod = units->top()->module;
        assert(
            !mod->getGlobalVariable(llvm::StringRef(var_name.c_str())));

        llvm::GlobalVariable *var = llvm::cast<llvm::GlobalVariable>(
            mod->getOrInsertGlobal(var_name.c_str(), llvm_type));

        llvm::Constant *constr_str =
            getStringConstantArray(top->token->str_value.c_str());
        var->setInitializer(constr_str);
        var->setConstant(true);
        var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

        llvm::Constant *const_pchar =
            createConstantGEP(llvm::cast<llvm::Constant>(var),
                              ctx->nt->getTwoLLVMZeros());

        return const_pchar;
    } else if (type->array_type) {
        if (!top->is_list) {
            return NULL;
        }
        std::vector<llvm::Constant *> constants;
        std::vector<Node *> *lst = top->list;
        if (lst->size() < 2) {
            return NULL;
        }
        Node *first = lst->at(0);
        if (!first->is_token ||
            first->token->str_value.compare("array")) {
            return NULL;
        }

        for (std::vector<Node *>::iterator b = (lst->begin() + 1),
                                           e = lst->end();
             b != e; ++b) {
            int size;
            llvm::Constant *constant =
                simpleParseLiteral(units, type->array_type, *b, &size);
            if (!constant) {
                return NULL;
            }

            constants.push_back(constant);
        }

        llvm::Constant *const_arr = llvm::ConstantArray::get(
            llvm::cast<llvm::ArrayType>(
                ctx->toLLVMType(type, top, false, false)),
            constants);

        return const_arr;
    } else if (type->struct_name.size()) {
        Struct *st = ctx->getStruct(type);
        if (!top->is_list) {
            return NULL;
        }
        std::vector<llvm::Constant *> constants;
        std::vector<Node *> *lst = top->list;

        for (std::vector<Node *>::iterator b = lst->begin(),
                                           e = lst->end();
             b != e; ++b) {
            Node *member_node = (*b);
            if (!member_node->is_list) {
                return NULL;
            }
            std::vector<Node *> *member_lst = member_node->list;
            if (member_lst->size() != 2) {
                return NULL;
            }
            Node *name_node = (*member_lst)[0];
            Node *value_node = (*member_lst)[1];

            if (!name_node->is_token) {
                return NULL;
            }

            const char *name = name_node->token->str_value.c_str();
            Type *type = st->nameToType(name);
            if (!type) {
                return NULL;
            }

            int size;
            llvm::Constant *constant =
                simpleParseLiteral(units, type, value_node, &size);
            if (!constant) {
                return NULL;
            }
            constants.push_back(constant);
        }

        llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
        if (!llvm_type) {
            return NULL;
        }

        llvm::StructType *llvm_st =
            llvm::cast<llvm::StructType>(llvm_type);
        llvm::Constant *const_st =
            llvm::ConstantStruct::get(llvm_st, constants);

        return const_st;
    } else {
        return NULL;
    }
}

llvm::Constant *parseLiteral(Units *units, Type *type, Node *top,
                             int *size) {
    Context *ctx = units->top()->ctx;

    llvm::Constant *result = simpleParseLiteral(units, type, top, size);
    if (result) {
        return result;
    }

    /* The size argument is only set when parsing a string literal; it
     * will contain the final size of the returned array. */

    /* If the argument node is an address-of form for a global
     * variable/function, return the address of that global
     * variable/function as a constant value.  This is to get around
     * the fact that arbitrary pointer values returned from the
     * function created below will not be valid with respect to global
     * variables/functions. */

    bool is_rvalue = false;
    if (top->is_list && (top->list->size() == 2) &&
        ((*top->list)[0]->is_token) &&
        (!(*top->list)[0]->token->str_value.compare("move"))) {
        is_rvalue = true;
    }

    std::string str;
    type->toString(&str);

    /* Create an empty no-argument function that returns the specified
     * type. */

    llvm::Type *llvm_return_type = ctx->toLLVMType(type, top, false);
    if (!llvm_return_type) {
        return NULL;
    }

    std::vector<llvm::Type *> empty_args;
    llvm::FunctionType *ft =
        getFunctionType(llvm_return_type, empty_args, false);

    std::string new_name;
    units->top()->getUnusedFunctionName(&new_name);

    llvm::Constant *const_fn =
        units->top()->module->getOrInsertFunction(new_name.c_str(), ft);

    llvm::Function *llvm_fn = llvm::cast<llvm::Function>(const_fn);
    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_C));

    std::vector<Variable *> args;
    Function *fn = new Function(type, &args, llvm_fn, 0, &new_name);

    fn->linkage = Linkage::Intern;
    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node *> nodes;
    nodes.push_back(top);
    Node *wrapper_top = new Node(&nodes);

    std::vector<NSNode *> active_ns_nodes = ctx->active_ns_nodes;
    std::vector<NSNode *> used_ns_nodes = ctx->used_ns_nodes;
    if (!units->prefunction_ns) {
        units->prefunction_ns = ctx->active_ns_nodes.front()->ns;
    }
    ctx->popUntilNamespace(units->prefunction_ns);

    ctx->enableRetrievalLog();
    Function *temp_fn = new Function();
    temp_fn->llvm_function = llvm_fn;
    units->top()->pushGlobalFunction(temp_fn);
    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;
    FormProcBodyParse(units, wrapper_top, fn, llvm_fn, 0, 0);
    ctx->deactivateNamespace(anon_name.c_str());
    units->top()->popGlobalFunction();

    ctx->active_ns_nodes = active_ns_nodes;
    ctx->used_ns_nodes = used_ns_nodes;

    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count_begin != error_count_end) {
        llvm_fn->replaceAllUsesWith(
            llvm::UndefValue::get(llvm_fn->getType()));
        llvm_fn->eraseFromParent();
        return NULL;
    }

    llvm::FunctionType *wrapper_ft = getFunctionType(
        ctx->toLLVMType(ctx->tr->type_pchar, NULL, false), empty_args,
        false);

    std::string wrapper_new_name;
    units->top()->getUnusedFunctionName(&wrapper_new_name);

    llvm::Constant *wrapper_const_fn =
        units->top()->module->getOrInsertFunction(
            wrapper_new_name.c_str(), wrapper_ft);
    llvm::Function *wrapper_fn =
        llvm::cast<llvm::Function>(wrapper_const_fn);

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*getContext(), "entry", wrapper_fn);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args;
    llvm::Value *ret = builder.CreateCall(
        llvm_fn, llvm::ArrayRef<llvm::Value *>(call_args));

    llvm::Value *ret_storage1 = builder.CreateAlloca(llvm_return_type);
    llvm::Value *ret_storage2 = builder.CreateAlloca(llvm_return_type);
    builder.CreateStore(ret, ret_storage2);

    std::vector<Type *> call_arg_types;
    Type *ptr_type = ctx->tr->getPointerType(type);
    STL::push_back2(&call_arg_types, ptr_type, ptr_type);

    std::vector<Type *> types;
    types.push_back(ptr_type);
    types.push_back(type);
    std::vector<bool> lvalues;
    lvalues.push_back(true);
    lvalues.push_back(false);
    Function *or_move = ctx->getFunction("setf-move-assign", &types,
                                         NULL, false, &lvalues);
    Function *or_setf =
        ctx->getFunction("setf-copy-assign", &call_arg_types, NULL, 0);

    if (or_move && is_rvalue) {
        or_setf = or_move;
    } else if (!Operation::IsCopyPermitted(ctx, top, type)) {
        return NULL;
    }

    if (or_setf) {
        std::vector<llvm::Value *> or_call_args;
        STL::push_back2(&or_call_args, ret_storage1, ret_storage2);
        builder.CreateCall(or_setf->llvm_function,
                           llvm::ArrayRef<llvm::Value *>(or_call_args));
    } else {
        builder.CreateStore(ret, ret_storage1);
    }

    ParseResult cast_pr;
    bool res = Operation::Cast(ctx, block, ret_storage1,
                               ctx->tr->getPointerType(type),
                               ctx->tr->type_pchar, top, 0, &cast_pr);
    if (!res) {
        return NULL;
    }
    block = cast_pr.block;
    llvm::Value *ret_cast = cast_pr.getValue(ctx);

    char data[256];
    memset(data, 0, 256);

    char ptr_int[64];
    snprintf(ptr_int, sizeof(ptr_int), "%lld",
             reinterpret_cast<long long int>(&data)); // NOLINT

    llvm::Value *ptr_value = ctx->nt->getConstantInt(
        llvm::IntegerType::get(*getContext(), sizeof(char *) * 8),
        ptr_int);
    ParseResult cast_pr_ptr;
    res = Operation::Cast(ctx, block, ptr_value, ctx->tr->type_intptr,
                          ctx->tr->type_pchar, top, 0, &cast_pr_ptr);
    if (!res) {
        return NULL;
    }
    builder.SetInsertPoint(cast_pr_ptr.block);

    llvm::Value *new_ptr_value = builder.CreateAlloca(
        ctx->toLLVMType(ctx->tr->type_pchar, NULL, false));
    Function *malloc = ctx->getFunction("malloc", NULL, NULL, 0);
    assert(malloc && "no memcpy function available");
    std::vector<llvm::Value *> malloc_args;
    malloc_args.push_back(
        llvm::ConstantInt::get(ctx->nt->getNativeSizeType(), 256));
    builder.CreateStore(
        builder.CreateBitCast(
            builder.CreateCall(
                malloc->llvm_function,
                llvm::ArrayRef<llvm::Value *>(malloc_args)),
            ctx->toLLVMType(ctx->tr->type_pchar, NULL, false)),
        new_ptr_value);

    Function *memcpy_fn = ctx->getFunction("memcpy", NULL, NULL, 0);
    assert(memcpy_fn && "no memcpy function available");

    size_t struct_size = Operation::SizeofGet(units->top(), type);
    char struct_size_str[8];
    snprintf(struct_size_str, sizeof(struct_size_str), "%u",
             static_cast<unsigned>(struct_size));

    std::vector<llvm::Value *> memcpy_args;
    memcpy_args.push_back(builder.CreateBitCast(
        builder.CreateLoad(new_ptr_value),
        ctx->toLLVMType(ctx->tr->type_pvoid, NULL, false)));
    memcpy_args.push_back(builder.CreateBitCast(
        ret_cast, ctx->toLLVMType(ctx->tr->type_pvoid, NULL, false)));
    memcpy_args.push_back(
        ctx->nt->getConstantInt((llvm::IntegerType *)ctx->toLLVMType(
                                    ctx->tr->type_size, NULL, false),
                                struct_size_str));

    builder.CreateCall(memcpy_fn->llvm_function,
                       llvm::ArrayRef<llvm::Value *>(memcpy_args));
    builder.CreateRet(builder.CreateLoad(new_ptr_value));

    if (units->debug) {
        functionDebugPass(llvm_fn);
        functionDebugPass(wrapper_fn);
    }

    cloneModuleIfRequired(units->top());

    llvm::Function *bf =
        units->top()->ee->FindFunctionNamed(wrapper_new_name.c_str());
    std::vector<llvm::GenericValue> values;

#if D_LLVM_VERSION_ORD >= 34
    units->top()->ee->getFunctionAddress(wrapper_new_name);
#endif

    llvm::GenericValue res2 = units->top()->ee->runFunction(bf, values);
    memcpy(data, res2.PointerVal, 256);

    error_count_begin = ctx->er->getErrorTypeCount(ErrorType::Error);

    llvm::Constant *parsed =
        decodeRawData(units, top, reinterpret_cast<char *>(&data),
                      type, size);
    ctx->disableRetrievalLog();

    wrapper_fn->eraseFromParent();
    (llvm::cast<llvm::Function>(const_fn))->eraseFromParent();

    if (parsed) {
        return parsed;
    }

    error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count_begin != error_count_end) {
        return NULL;
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, top, type_str.c_str());
    ctx->er->addError(e);
    return NULL;
}

void zeroInitialise(Context *ctx, llvm::GlobalVariable *llvm_var,
                    llvm::Type *llvm_type, Type *type,
                    bool *has_initialiser) {
    if (type->points_to) {
        llvm_var->setInitializer(getNullPointer(llvm_type));
    } else if (type->struct_name.size() || type->is_array) {
        llvm_var->setInitializer(
            llvm::ConstantAggregateZero::get(llvm_type));
    } else if (type->isIntegerType()) {
        llvm_var->setInitializer(ctx->nt->getConstantInt(
            llvm::IntegerType::get(*getContext(),
                                   ctx->nt->internalSizeToRealSize(
                                       type->getIntegerSize())),
            "0"));
    } else {
        *has_initialiser = false;
    }
    return;
}

bool FormTopLevelGlobalVariableParse(Units *units, Node *node,
                                     const char *name) {
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *top_lst = node->list;

    Node *def_node;
    if (!name) {
        Node *name_node = (*(top_lst))[1];
        name = name_node->token->str_value.c_str();
        def_node = (*(node->list))[2];
    } else {
        def_node = node;
    }

    std::vector<Node *> *lst = def_node->list;
    Node *linkage_node = (*lst)[1];
    Node *type_node = (*lst)[2];
    Node *value_node = (*lst)[3];

    bool has_initialiser;
    if (lst->size() < 3) {
        Error *e = new Error(IncorrectMinimumNumberOfArgs, def_node,
                             "var", "2", (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    } else if (lst->size() == 3) {
        has_initialiser = false;
    } else if (lst->size() == 4) {
        has_initialiser = true;
    } else {
        Error *e = new Error(IncorrectMaximumNumberOfArgs, def_node,
                             "var", "3", (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    }

    int linkage = FormLinkageParse(ctx, linkage_node);

    Type *ret_type = FormTypeParse(units, type_node, false, false);
    if (ret_type == NULL) {
        return false;
    }
    if (ret_type->array_type && (ret_type->array_size == 0)) {
        Error *e =
            new Error(ZeroLengthGlobalArraysAreUnsupported, def_node);
        ctx->er->addError(e);
        return false;
    }

    if (has_initialiser) {
        value_node =
            units->top()->mp->parsePotentialMacroCall(value_node);
        if (!value_node) {
            return false;
        }
    }

    int size = 0;
    llvm::Constant *init = NULL;
    if (has_initialiser) {
        init = parseLiteral(units, ret_type, value_node, &size);
        if (!init) {
            return false;
        }
    }

    std::string new_name;
    if (linkage == Linkage::Extern_C) {
        new_name.append(name);
    } else {
        ctx->ns()->nameToSymbol(name, &new_name);
    }

    Variable *existing_var = ctx->getVariable(name);
    if (existing_var && existing_var->type->isEqualTo(ret_type) &&
        (existing_var->linkage == linkage) && !has_initialiser) {
        /* A redeclaration of a global variable is a no-op. */
        return true;
    }

    Variable *var = new Variable();
    var->name.append(name);
    var->type = ret_type;
    var->symbol.append(new_name);
    var->once_tag = units->top()->once_tag;
    var->linkage = linkage;

    bool res = ctx->ns()->addVariable(name, var);
    if (!res) {
        Error *e = new Error(RedefinitionOfVariable, def_node, name);
        ctx->er->addError(e);
        return false;
    }

    Namespace *current_ns = (*(ctx->used_ns_nodes.rbegin()))->ns;
    std::vector<Type *> types;
    Function *matching_fn =
        current_ns->getFunction(name, &types, NULL, false, true);
    if (matching_fn) {
        Error *e =
            new Error(RedeclarationOfDifferentKind, def_node, name);
        ctx->er->addError(e);
        return false;
    }

    llvm::Type *llvm_ret_type = ctx->toLLVMType(
        ret_type, def_node, false,
        (Linkage::isExternAll(linkage) && !has_initialiser));
    if (!llvm_ret_type) {
        return false;
    }

    assert(!units->top()->module->getGlobalVariable(
               llvm::StringRef(new_name.c_str())) &&
           "variable already exists in module");

    llvm::GlobalVariable *llvm_var = llvm::cast<llvm::GlobalVariable>(
        units->top()->module->getOrInsertGlobal(new_name.c_str(),
                                                llvm_ret_type));
    llvm_var->setLinkage(ctx->toLLVMLinkage(linkage));

    if (init) {
        llvm_var->setInitializer(init);
    } else if (!Linkage::isExternAll(linkage)) {
        has_initialiser = true;
        zeroInitialise(ctx, llvm_var, llvm_ret_type, ret_type,
                       &has_initialiser);
        var->has_initialiser = has_initialiser;
    }

    var->value = llvm::cast<llvm::Value>(llvm_var);

    return true;
}
}
