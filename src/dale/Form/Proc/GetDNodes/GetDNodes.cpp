#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../llvm_Function.h"
#include "Config.h"

namespace dale
{
std::map<std::string, llvm::GlobalVariable*> string_cache;

llvm::Type *llvm_type_dnode = NULL;
llvm::Type *llvm_type_pdnode = NULL;

llvm::Value *
IntNodeToStaticDNode(Generator *gen,
                     Node *node,
                     llvm::Value *next_node)
{
    if (!node) {
        fprintf(stderr, "Internal error: null node passed to "
                "IntNodeToStaticNode.\n");
        abort();
    }

    /* If it's one node, add the dnode. */
    std::string varname;
    gen->units->top()->getUnusedVarname(&varname);

    /* Add the variable to the module. */

    llvm::Type *llvm_type = llvm_type_dnode;
    llvm::Type *llvm_r_type = llvm_type_pdnode;

    llvm::GlobalVariable *var =
        llvm::cast<llvm::GlobalVariable>(
            gen->units->top()->module->getOrInsertGlobal(varname.c_str(), llvm_type)
        );

    Context *ctx = gen->units->top()->ctx;
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

        llvm::GlobalVariable *svar2 = NULL;

        std::map<std::string, llvm::GlobalVariable*>::iterator f
        = string_cache.find(t->str_value);
        if (f != string_cache.end()) {
            llvm::GlobalVariable *temp = f->second;
            if (temp->getParent() == gen->units->top()->module) {
                svar2 = temp;
            }
        }

        if (!svar2) {
            llvm::Constant *arr =
#if D_LLVM_VERSION_MINOR < 2
                llvm::ConstantArray::get(
#else
                llvm::ConstantDataArray::getString(
#endif
                                         llvm::getGlobalContext(),
                                         t->str_value.c_str(),
                                         true);
            std::string varname2;
            gen->units->top()->getUnusedVarname(&varname2);

            Type *archar =
                ctx->tr->getArrayType(ctx->tr->type_char,
                                 t->str_value.size() + 1);

            svar2 =
                llvm::cast<llvm::GlobalVariable>(
                    gen->units->top()->module->getOrInsertGlobal(varname2.c_str(),
                                           ctx->toLLVMType(archar, NULL, false))
                );

            svar2->setInitializer(arr);
            svar2->setConstant(true);
            svar2->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

            string_cache.insert(std::pair<std::string,
                                llvm::GlobalVariable*>(
                                    t->str_value,
                                    svar2
                                ));
        }

        llvm::Value *temps[2];
        temps[0] = ctx->nt->getNativeInt(0);
        temps[1] = ctx->nt->getNativeInt(0);

        llvm::Constant *pce =
            llvm::ConstantExpr::getGetElementPtr(
                llvm::cast<llvm::Constant>(svar2),
                temps,
                2
            );

        constants.push_back(pce);
    } else {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(
                        ctx->toLLVMType(ctx->tr->type_pchar, NULL, false)
                    )
                )
            )
        );
    }

    if (node->is_list) {
        std::vector<Node *> *list = node->list;
        std::vector<Node *>::reverse_iterator list_iter = list->rbegin();
        llvm::Value *sub_next_node = NULL;

        while (list_iter != list->rend()) {
            llvm::Value *temp_value =
                IntNodeToStaticDNode(gen, (*list_iter), sub_next_node);
            sub_next_node = temp_value;
            ++list_iter;
        }

        constants.push_back(
            llvm::cast<llvm::Constant>(
                sub_next_node
            )
        );
    } else {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(
                        llvm_r_type
                    )
                )
            )
        );
    }

    if (next_node) {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                next_node
            )
        );
    } else {
        constants.push_back(
            llvm::cast<llvm::Constant>(
                llvm::ConstantPointerNull::get(
                    llvm::cast<llvm::PointerType>(
                        llvm_r_type
                    )
                )
            )
        );
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

    constants.push_back(
        llvm::cast<llvm::Constant>(
            llvm::ConstantPointerNull::get(
                llvm::cast<llvm::PointerType>(
                    ctx->toLLVMType(ctx->tr->type_pchar, NULL, false)
                )
            )
        )
    );
    llvm::StructType *st =
        llvm::cast<llvm::StructType>(llvm_type);
    llvm::Constant *init =
        llvm::ConstantStruct::get(
            st,
            constants
        );
    var->setInitializer(init);

    var->setConstant(true);

    return llvm::cast<llvm::Value>(var);
}

bool
FormProcGetDNodesParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->units->top()->ctx;

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

    llvm::Value *v = IntNodeToStaticDNode(gen, node->list->at(1), NULL);

    llvm::IRBuilder<> builder(block);

    pr->set(block, ctx->tr->type_pdnode, v);
    return true;
}
}
