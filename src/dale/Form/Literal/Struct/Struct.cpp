#include "Struct.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../Proc/Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool
FormLiteralStructParse(Units *units, Function *fn, llvm::BasicBlock *block,
                       Node *node, const char *struct_name, Struct *st,
                       Type *st_type, bool get_address, ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!node->is_list) {
        Error *e = new Error(UnexpectedElement, node,
                             "list", "struct initialisers", "atom");
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *> *lst = node->list;

    llvm::IRBuilder<> builder(block);

    llvm::Value *storage =
        llvm::cast<llvm::Value>(builder.CreateAlloca(st->type));

    for (std::vector<Node *>::iterator b = lst->begin(),
                                       e = lst->end();
            b != e;
            ++b) {
        Node *member_node = (*b);
        if (!member_node->is_list) {
            Error *e = new Error(UnexpectedElement, member_node,
                                 "list", "struct initialiser", "atom");
            ctx->er->addError(e);
            return false;
        }
        std::vector<Node *> *member_lst = member_node->list;
        if (member_lst->size() != 2) {
            Error *e = new Error(UnexpectedElement, member_node,
                                 "list", "struct initialiser", "atom");
            ctx->er->addError(e);
            return false;
        }
        Node *name_node  = (*member_lst)[0];
        Node *value_node = (*member_lst)[1];

        if (!name_node->is_token) {
            Error *e = new Error(UnexpectedElement, name_node,
                                 "atom", "struct field name", "list");
            ctx->er->addError(e);
            return false;
        }

        const char *name = name_node->token->str_value.c_str();
        Type *type = st->memberToType(name);
        if (!type) {
            Error *e = new Error(FieldDoesNotExistInStruct,
                                 name_node, name, struct_name);
            ctx->er->addError(e);
            return false;
        }

        int index = st->memberToIndex(name);

        std::vector<llvm::Value *> indices;
        STL::push_back2(&indices, ctx->nt->getLLVMZero(),
                        ctx->nt->getNativeInt(index));

        llvm::Value *storage_ptr =
            builder.CreateGEP(storage,
                              llvm::ArrayRef<llvm::Value*>(indices));

        ParseResult pr_value;
        bool res = FormProcInstParse(units, fn, block, value_node, false,
                                      false, NULL, &pr_value);
        if (!res) {
            return false;
        }

        if (!pr_value.type->isEqualTo(type, 1)) {
            if ((type->isIntegerType()
                    && pr_value.type->isIntegerType())
                    || (type->isFloatingPointType()
                        && pr_value.type->isFloatingPointType())) {
                ParseResult pr_cast;
                res = Operation::Cast(ctx, pr_value.block,
                                      pr_value.value, pr_value.type,
                                      type, member_node, 0, &pr_cast);
                if (!res) {
                    return false;
                }
                pr_cast.copyTo(&pr_value);
            } else {
                std::string wanted;
                std::string got;
                type->toString(&wanted);
                pr_value.type->toString(&got);
                Error *e = new Error(IncorrectType, name_node,
                                     wanted.c_str(), got.c_str());
                ctx->er->addError(e);
                return false;
            }
        }

        block = pr_value.block;
        builder.SetInsertPoint(block);
        builder.CreateStore(pr_value.value, storage_ptr);
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
