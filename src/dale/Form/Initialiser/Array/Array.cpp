#include "Array.h"

#include <string>
#include <vector>

#include "../../../Error/Error.h"
#include "../../../Linkage/Linkage.h"
#include "../../Proc/Inst/Inst.h"
#include "../../Utils/Utils.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormInitialiserArrayParse(Units *units, Function *dfn,
                               llvm::BasicBlock *block, Node *node,
                               Type *array_type, bool get_address,
                               int *size, ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    Node *array_list = node;

    if (!array_list->is_list) {
        Error *e = new Error(UnexpectedElement, array_list, "list",
                             "array initialisers", "atom");
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *> *lst = array_list->list;

    std::vector<ParseResult *> elements;
    for (std::vector<Node *>::iterator b = (lst->begin() + 1),
                                       e = lst->end();
         b != e; ++b) {
        ParseResult *el = new ParseResult();
        bool res = FormProcInstParse(units, dfn, block, (*b), false,
                                     false, array_type->array_type, el);
        if (!res) {
            return false;
        }
        if (!el->type->isEqualTo(array_type->array_type)) {
            std::string wanted;
            std::string got;
            array_type->array_type->toString(&wanted);
            el->type->toString(&got);

            Error *e = new Error(IncorrectType, (*b), wanted.c_str(),
                                 got.c_str());
            ctx->er->addError(e);
            return false;
        }
        elements.push_back(el);
        block = el->block;
    }

    if ((array_type->array_size != 0) &&
        (array_type->array_size != static_cast<int>(elements.size()))) {
        Error *e = new Error(IncorrectNumberOfArrayElements, node,
                             elements.size(), array_type->array_size);
        ctx->er->addError(e);
        return false;
    }

    *size = static_cast<int>(elements.size());
    array_type = ctx->tr->getArrayType(array_type->array_type, *size);

    llvm::Type *llvm_array_type =
        ctx->toLLVMType(array_type, node, false);
    if (!llvm_array_type) {
        return false;
    }

    llvm::IRBuilder<> builder(block);
    llvm::Value *llvm_array = builder.CreateAlloca(llvm_array_type);

    std::vector<llvm::Value *> indices;
    indices.push_back(ctx->nt->getLLVMZero());

    for (int i = 0; i < static_cast<int>(elements.size()); ++i) {
        indices.push_back(ctx->nt->getNativeInt(i));

        llvm::Value *element_storage = builder.Insert(
            createGEP(llvm_array,
                      llvm::ArrayRef<llvm::Value *>(indices),
                      llvm_array_type),
            "aref");

        builder.CreateStore(elements[i]->getValue(ctx),
                            element_storage);

        indices.pop_back();
        delete elements[i];
    }

    indices.push_back(ctx->nt->getNativeInt(0));
    llvm::Value *array_ptr = builder.Insert(
        createGEP(llvm_array, llvm::ArrayRef<llvm::Value *>(indices),
                  llvm_array_type),
        "aref");

    pr->set(block, ctx->tr->getPointerType(array_type->array_type),
            array_ptr);

    if (!get_address) {
        pr->value_is_lvalue = false;
        pr->type_of_address_of_value = pr->type;
        pr->address_of_value = pr->getValue(ctx);
        pr->type = array_type;
        pr->set(
            pr->block, pr->type,
            llvm::cast<llvm::Value>(builder.CreateLoad(llvm_array)));
    }

    return true;
}
}
