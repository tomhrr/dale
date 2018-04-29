#include "Struct.h"

#include <string>
#include <vector>

#include "../../../Error/Error.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../Proc/Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormInitialiserStructParse(Units *units, Function *fn,
                                llvm::BasicBlock *block, Node *node,
                                const char *struct_name, Struct *st,
                                Type *st_type, bool get_address,
                                ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!node->is_list) {
        Error *e = new Error(UnexpectedElement, node, "list",
                             "struct initialisers", "atom");
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *> *lst = node->list;

    llvm::IRBuilder<> builder(block);

    llvm::Value *storage =
        llvm::cast<llvm::Value>(builder.CreateAlloca(st->type));

    for (std::vector<Node *>::iterator b = lst->begin(), e = lst->end();
         b != e; ++b) {
        Node *member_node = (*b);
        if (!member_node->is_list) {
            Error *e = new Error(UnexpectedElement, member_node, "list",
                                 "struct initialiser", "atom");
            ctx->er->addError(e);
            return false;
        }
        std::vector<Node *> *member_lst = member_node->list;
        if (member_lst->size() != 2) {
            Error *e = new Error(UnexpectedElement, member_node, "list",
                                 "struct initialiser", "atom");
            ctx->er->addError(e);
            return false;
        }
        Node *name_node = (*member_lst)[0];
        Node *value_node = (*member_lst)[1];

        if (!name_node->is_token) {
            Error *e = new Error(UnexpectedElement, name_node, "atom",
                                 "struct field name", "list");
            ctx->er->addError(e);
            return false;
        }

        const char *name = name_node->token->str_value.c_str();
        Type *type = st->nameToType(name);
        if (!type) {
            Error *e = new Error(FieldDoesNotExistInStruct, name_node,
                                 name, struct_name);
            ctx->er->addError(e);
            return false;
        }

        int index = st->nameToIndex(name);

        std::vector<llvm::Value *> indices;
        STL::push_back2(&indices, ctx->nt->getLLVMZero(),
                        ctx->nt->getNativeInt(index));

        llvm::Value *storage_ptr = builder.CreateGEP(
            storage, llvm::ArrayRef<llvm::Value *>(indices));

        ParseResult value_pr;
        bool res = FormProcInstParse(units, fn, block, value_node,
                                     false, false, type, &value_pr);
        if (!res) {
            return false;
        }

        if (!value_pr.type->isEqualTo(type, 1)) {
            if ((type->isIntegerType() &&
                 value_pr.type->isIntegerType()) ||
                (type->isFloatingPointType() &&
                 value_pr.type->isFloatingPointType())) {
                ParseResult cast_pr;
                res = Operation::Cast(
                    ctx, value_pr.block, value_pr.getValue(ctx),
                    value_pr.type, type, member_node, 0, &cast_pr);
                if (!res) {
                    return false;
                }
                cast_pr.copyTo(&value_pr);
            } else {
                std::string wanted;
                std::string got;
                type->toString(&wanted);
                value_pr.type->toString(&got);
                Error *e = new Error(IncorrectType, name_node,
                                     wanted.c_str(), got.c_str());
                ctx->er->addError(e);
                return false;
            }
        }

        block = value_pr.block;
        builder.SetInsertPoint(block);
        builder.CreateStore(value_pr.getValue(ctx), storage_ptr);
    }

    if (get_address) {
        pr->set(block, ctx->tr->getPointerType(st_type), storage);
    } else {
        llvm::Value *final_value = builder.CreateLoad(storage);
        pr->set(block, st_type, final_value);
        pr->address_of_value = storage;
        pr->value_is_lvalue = true;
    }
    return true;
}
}
