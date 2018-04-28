#include "Quote.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvmUtils/llvmUtils.h"
#include "../../../llvm_Function.h"
#include "../../Utils/Utils.h"
#include "Config.h"

namespace dale {
static std::map<std::string, llvm::GlobalVariable *> token_cache;

llvm::Type *llvm_type_dnode = NULL;
llvm::Type *llvm_type_pdnode = NULL;

llvm::Constant *getNullConstant(llvm::Type *llvm_type) {
    return llvm::cast<llvm::Constant>(getNullPointer(llvm_type));
}

llvm::GlobalVariable *createTokenGV(Units *units, std::string *value) {
    Context *ctx = units->top()->ctx;

    llvm::Constant *str_value = getStringConstantArray(value->c_str());

    std::string varname_str;
    units->top()->getUnusedVarName(&varname_str);

    Type *archar =
        ctx->tr->getArrayType(ctx->tr->type_char, value->size() + 1);

    llvm::GlobalVariable *token_gv = llvm::cast<llvm::GlobalVariable>(
        units->top()->module->getOrInsertGlobal(
            varname_str.c_str(), ctx->toLLVMType(archar, NULL, false)));

    token_gv->setInitializer(str_value);
    token_gv->setConstant(true);
    token_gv->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    token_cache.insert(std::pair<std::string, llvm::GlobalVariable *>(
        value->c_str(), token_gv));

    return token_gv;
}

llvm::GlobalVariable *getTokenGV(Units *units, std::string *value) {
    /* If there is an entry in the cache for this string, and
     * the global variable in the cache belongs to the current
     * module, then use that global variable. */

    llvm::GlobalVariable *token_gv = NULL;

    std::map<std::string, llvm::GlobalVariable *>::iterator f =
        token_cache.find(*value);
    if (f != token_cache.end()) {
        llvm::GlobalVariable *existing_token_gv = f->second;
        if (existing_token_gv->getParent() == units->top()->module) {
            token_gv = existing_token_gv;
        }
    }

    if (!token_gv) {
        token_gv = createTokenGV(units, value);
    }

    return token_gv;
}

llvm::Constant *getTokenPointer(Units *units, std::string *value) {
    llvm::GlobalVariable *token_gv = getTokenGV(units, value);
    llvm::Constant *token_gv_const =
        llvm::cast<llvm::Constant>(token_gv);

    llvm::Constant *ptr_to_token = createConstantGEP(
        token_gv_const, units->top()->ctx->nt->getTwoLLVMZeros());

    return ptr_to_token;
}

llvm::Value *IntNodeToStaticDNode(Units *units, Node *node,
                                  llvm::Value *next_node) {
    assert(node && "null node passed to conversion function");

    std::string varname;
    units->top()->getUnusedVarName(&varname);

    llvm::Type *llvm_type = llvm_type_dnode;
    llvm::Type *llvm_r_type = llvm_type_pdnode;

    llvm::GlobalVariable *var = llvm::cast<llvm::GlobalVariable>(
        units->top()->module->getOrInsertGlobal(varname.c_str(),
                                                llvm_type));

    Context *ctx = units->top()->ctx;
    var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    std::vector<llvm::Constant *> constants;
    llvm::Constant *first = llvm::cast<llvm::Constant>(
        ctx->nt->getNativeInt(node->is_list));
    constants.push_back(first);

    if (!node->is_list) {
        Token *t = node->token;
        size_t pos = 0;
        while ((pos = t->str_value.find("\\n", pos)) !=
               std::string::npos) {
            t->str_value.replace(pos, 2, "\n");
        }
        if (t->type == TokenType::StringLiteral) {
            t->str_value.insert(0, "\"");
            t->str_value.push_back('"');
        }

        llvm::Constant *ptr_to_token =
            getTokenPointer(units, &(t->str_value));
        constants.push_back(ptr_to_token);
        constants.push_back(getNullConstant(llvm_r_type));
    } else {
        llvm::Type *type =
            ctx->toLLVMType(ctx->tr->type_pchar, NULL, false);
        constants.push_back(getNullConstant(type));

        std::vector<Node *> *list = node->list;
        std::vector<Node *>::reverse_iterator list_iter =
            list->rbegin();
        llvm::Value *sub_next_node = NULL;

        while (list_iter != list->rend()) {
            llvm::Value *sub_node = IntNodeToStaticDNode(
                units, (*list_iter), sub_next_node);
            sub_next_node = sub_node;
            ++list_iter;
        }

        if (sub_next_node) {
            constants.push_back(
                llvm::cast<llvm::Constant>(sub_next_node));
        } else {
            constants.push_back(getNullConstant(llvm_r_type));
        }
    }

    if (next_node) {
        constants.push_back(llvm::cast<llvm::Constant>(next_node));
    } else {
        constants.push_back(getNullConstant(llvm_r_type));
    }

    int pos[8] = {node->getBeginPos()->getLineNumber(),
                  node->getBeginPos()->getColumnNumber(),
                  node->getEndPos()->getLineNumber(),
                  node->getEndPos()->getColumnNumber(),
                  node->macro_begin.getLineNumber(),
                  node->macro_begin.getColumnNumber(),
                  node->macro_end.getLineNumber(),
                  node->macro_end.getColumnNumber()};
    for (int i = 0; i < 8; i++) {
        constants.push_back(
            llvm::cast<llvm::Constant>(ctx->nt->getNativeInt(pos[i])));
    }

    if (node->filename) {
        std::string filename(node->filename);
        llvm::Constant *ptr_to_filename =
            getTokenPointer(units, &filename);
        constants.push_back(ptr_to_filename);
    } else {
        llvm::Type *type =
            ctx->toLLVMType(ctx->tr->type_pchar, NULL, false);
        constants.push_back(getNullConstant(type));
    }

    llvm::StructType *st = llvm::cast<llvm::StructType>(llvm_type);
    llvm::Constant *init = llvm::ConstantStruct::get(st, constants);
    var->setInitializer(init);
    var->setConstant(true);

    return llvm::cast<llvm::Value>(var);
}

bool FormProcQuoteParse(Units *units, Function *fn,
                        llvm::BasicBlock *block, Node *node,
                        bool get_address, bool prefixed_with_core,
                        ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!llvm_type_dnode) {
        llvm::Type *dnode =
            ctx->toLLVMType(ctx->tr->type_dnode, NULL, false);
        assert(dnode && "unable to fetch DNode type");

        llvm::Type *pointer_to_dnode =
            ctx->toLLVMType(ctx->tr->type_pdnode, NULL, false);
        assert(dnode && "unable to fetch pointer-to-DNode type");

        intptr_t intptr_value = (intptr_t)pointer_to_dnode;
        llvm_type_pdnode = (llvm::Type *)intptr_value;
        intptr_value = (intptr_t)dnode;
        llvm_type_dnode = (llvm::Type *)intptr_value;
    }

    if (!ctx->er->assertArgNums("q", node, 1, 1)) {
        return false;
    }

    llvm::Value *static_dnode =
        IntNodeToStaticDNode(units, (*node->list)[1], NULL);
    pr->set(block, ctx->tr->type_pdnode, static_dnode);
    return true;
}
}
