#include "Struct.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../Proc/Inst/Inst.h"

using namespace dale::ErrorInst::Generator;

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
        Node *sel = (*b);
        if (!sel->is_list) {
            Error *e = new Error(UnexpectedElement, sel,
                                 "list", "struct initialiser", "atom");
            ctx->er->addError(e);
            return false;
        }
        std::vector<Node *> *sellst = sel->list;
        if (sellst->size() != 2) {
            Error *e = new Error(UnexpectedElement, sel,
                                 "list", "struct initialiser", "atom");
            ctx->er->addError(e);
            return false;
        }
        Node *name      = (*sellst)[0];
        Node *namevalue = (*sellst)[1];

        if (!name->is_token) {
            Error *e = new Error(UnexpectedElement, sel,
                                 "atom", "struct field name", "list");
            ctx->er->addError(e);
            return false;
        }

        Type *nametype =
            st->memberToType(name->token->str_value.c_str());

        if (!nametype) {
            Error *e = new Error(FieldDoesNotExistInStruct,
                                 name, name->token->str_value.c_str(),
                                 struct_name);
            ctx->er->addError(e);
            return false;
        }

        int index = st->memberToIndex(name->token->str_value.c_str());

        std::vector<llvm::Value *> indices;
        STL::push_back2(&indices, ctx->nt->getLLVMZero(),
                        ctx->nt->getNativeInt(index));

        llvm::Value *storage_ptr =
            builder.CreateGEP(storage,
                              llvm::ArrayRef<llvm::Value*>(indices));

        ParseResult newvalue;
        bool mres = FormProcInstParse(units, fn, block, namevalue, false,
                                      false, NULL, &newvalue);

        if (!mres) {
            return false;
        }
        if (!newvalue.type->isEqualTo(nametype, 1)) {
            if ((nametype->isIntegerType()
                    && newvalue.type->isIntegerType())
                    || (nametype->isFloatingPointType()
                        && newvalue.type->isFloatingPointType())) {
                ParseResult casttemp;
                bool res = Operation::Cast(ctx, newvalue.block,
                                           newvalue.value, newvalue.type,
                                           nametype, sel, 0, &casttemp);
                if (!res) {
                    return false;
                }
                casttemp.copyTo(&newvalue);
            } else {
                std::string expstr;
                std::string gotstr;
                nametype->toString(&expstr);
                newvalue.type->toString(&gotstr);
                Error *e = new Error(IncorrectType, name,
                                     expstr.c_str(), gotstr.c_str());
                ctx->er->addError(e);
                return false;
            }
        }

        block = newvalue.block;
        builder.SetInsertPoint(block);
        builder.CreateStore(newvalue.value, storage_ptr);
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
