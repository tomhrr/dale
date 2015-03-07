#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../llvm_Function.h"
#include "Config.h"

namespace dale
{
static std::map<std::string, llvm::GlobalVariable*> token_cache;

llvm::Type *llvm_type_dnode = NULL;
llvm::Type *llvm_type_pdnode = NULL;

llvm::Constant *
getNullConstant(llvm::Type *llvm_type)
{
    return llvm::cast<llvm::Constant>(
        llvm::ConstantPointerNull::get(
            llvm::cast<llvm::PointerType>(llvm_type)
        )
    );
}

llvm::Value *
IntNodeToStaticDNode(Units *units, Node *node, llvm::Value *next_node)
{
    assert(node && "null node passed to conversion function");

    std::string varname;
    units->top()->getUnusedVarname(&varname);

    llvm::Type *llvm_type = llvm_type_dnode;
    llvm::Type *llvm_r_type = llvm_type_pdnode;

    llvm::GlobalVariable *var =
        llvm::cast<llvm::GlobalVariable>(
            units->top()->module->getOrInsertGlobal(varname.c_str(), llvm_type)
        );

    Context *ctx = units->top()->ctx;
    var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    std::vector<llvm::Constant *> constants;
    llvm::Constant *first =
        llvm::cast<llvm::Constant>(ctx->nt->getNativeInt(node->is_list));
    constants.push_back(first);

    if (!node->is_list) {
        Token *t = node->token;
        size_t pos = 0;
        while ((pos = t->str_value.find("\\n", pos)) != std::string::npos) {
            t->str_value.replace(pos, 2, "\n");
        }
        if (t->type == TokenType::StringLiteral) {
            t->str_value.insert(0, "\"");
            t->str_value.push_back('"');
        }

        /* If there is an entry in the cache for this string, and
         * the global variable in the cache belongs to the current
         * module, then use that global variable. */

        llvm::GlobalVariable *token_gv = NULL;

        std::map<std::string, llvm::GlobalVariable*>::iterator
            f = token_cache.find(t->str_value);
        if (f != token_cache.end()) {
            llvm::GlobalVariable *existing_token_gv = f->second;
            if (existing_token_gv->getParent() == units->top()->module) {
                token_gv = existing_token_gv;
            }
        }

        if (!token_gv) {
            llvm::Constant *str_value =
#if D_LLVM_VERSION_MINOR < 2
                llvm::ConstantArray::get(
#else
                llvm::ConstantDataArray::getString(
#endif
                    llvm::getGlobalContext(), t->str_value.c_str(),
                    true
                );

            std::string varname_str;
            units->top()->getUnusedVarname(&varname_str);

            Type *archar =
                ctx->tr->getArrayType(ctx->tr->type_char,
                                      t->str_value.size() + 1);

            token_gv =
                llvm::cast<llvm::GlobalVariable>(
                    units->top()->module->getOrInsertGlobal(
                        varname_str.c_str(),
                        ctx->toLLVMType(archar, NULL, false)
                    )
                );

            token_gv->setInitializer(str_value);
            token_gv->setConstant(true);
            token_gv->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

            token_cache.insert(
                std::pair<std::string, llvm::GlobalVariable*>(
                    t->str_value, token_gv
                )
            );
        }

        llvm::Value *two_zero_indices[2] = { ctx->nt->getNativeInt(0),
                                             ctx->nt->getNativeInt(0) };
        llvm::Constant *ptr_to_token =
            llvm::ConstantExpr::getGetElementPtr(
                llvm::cast<llvm::Constant>(token_gv),
                two_zero_indices, 2
            );

        constants.push_back(ptr_to_token);
    } else {
        llvm::Type *type = ctx->toLLVMType(ctx->tr->type_pchar, NULL, false);
        constants.push_back(getNullConstant(type));
    }

    if (node->is_list) {
        std::vector<Node *> *list = node->list;
        std::vector<Node *>::reverse_iterator list_iter = list->rbegin();
        llvm::Value *sub_next_node = NULL;

        while (list_iter != list->rend()) {
            llvm::Value *temp_value =
                IntNodeToStaticDNode(units, (*list_iter), sub_next_node);
            sub_next_node = temp_value;
            ++list_iter;
        }

        constants.push_back(
            llvm::cast<llvm::Constant>(
                sub_next_node
            )
        );
    } else {
        constants.push_back(getNullConstant(llvm_r_type));
    }

    if (next_node) {
        constants.push_back(llvm::cast<llvm::Constant>(next_node));
    } else {
        constants.push_back(getNullConstant(llvm_r_type));
    }

    int pos[8] = { node->getBeginPos()->getLineNumber(),
                   node->getBeginPos()->getColumnNumber(),
                   node->getEndPos()->getLineNumber(),
                   node->getEndPos()->getColumnNumber(),
                   node->macro_begin.getLineNumber(),
                   node->macro_begin.getColumnNumber(),
                   node->macro_end.getLineNumber(),
                   node->macro_end.getColumnNumber() };
    for (int i = 0; i < 8; i++) {
        constants.push_back(
            llvm::cast<llvm::Constant>(ctx->nt->getNativeInt(pos[i]))
        );
    }

    llvm::Type *type = ctx->toLLVMType(ctx->tr->type_pchar, NULL, false);
    constants.push_back(getNullConstant(type));

    llvm::StructType *st =
        llvm::cast<llvm::StructType>(llvm_type);
    llvm::Constant *init =
        llvm::ConstantStruct::get(st, constants);
    var->setInitializer(init);

    var->setConstant(true);

    return llvm::cast<llvm::Value>(var);
}

bool
FormProcGetDNodesParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!llvm_type_dnode) {
        llvm::Type *dnode =
            ctx->toLLVMType(ctx->tr->type_dnode, NULL, false);
        if (!dnode) {
            fprintf(stderr, "Unable to fetch DNode type.\n");
            abort();
        }

        llvm::Type *pointer_to_dnode =
            ctx->toLLVMType(ctx->tr->type_pdnode, NULL, false);
        if (!pointer_to_dnode) {
            fprintf(stderr, "Unable to fetch pointer to DNode type.\n");
            abort();
        }

        intptr_t temp = (intptr_t) pointer_to_dnode;
        llvm_type_pdnode = (llvm::Type *) temp;
        intptr_t temp1 = (intptr_t) dnode;
        llvm_type_dnode = (llvm::Type *) temp1;
    }

    if (!ctx->er->assertArgNums("get-dnodes", node, 1, 1)) {
        return false;
    }

    llvm::Value *v = IntNodeToStaticDNode(units, node->list->at(1), NULL);

    llvm::IRBuilder<> builder(block);

    pr->set(block, ctx->tr->type_pdnode, v);
    return true;
}
}
