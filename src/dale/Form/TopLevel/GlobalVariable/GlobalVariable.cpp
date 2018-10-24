#include "GlobalVariable.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../../Decoder/Decoder.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Copy/Copy.h"
#include "../../../Operation/Offsetof/Offsetof.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../Units/Units.h"
#include "../../../llvmUtils/llvmUtils.h"
#include "../../Linkage/Linkage.h"
#include "../../Literal/Literal.h"
#include "../../ProcBody/ProcBody.h"
#include "../../Type/Type.h"
#include "../../Utils/Utils.h"
#include "../../Value/Value.h"
#include "Config.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/Debug.h"

using namespace dale::ErrorInst;

namespace dale {
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
        init = FormValueParse(units, ret_type, value_node, &size);
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

    bool res = ctx->addVariable(name, var);
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
