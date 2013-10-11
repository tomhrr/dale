#include "Array.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"
#include "../../Proc/Inst/Inst.h"

namespace dale { namespace Form { namespace Literal { namespace Array {
bool 
parse(Generator *gen,
      Element::Function *dfn,
      llvm::BasicBlock *block,
      Node *n,
      const char *name,
      Element::Type *array_type,
      bool getAddress,
      int *size,
      ParseResult *pr)
{
    Context *ctx = gen->ctx;

    Node *array_list = n;

    if (!array_list->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, array_list,
            "list", "array initialisers", "atom"
        );
        ctx->er->addError(e);
        return false;
    }

    symlist *lst = array_list->list;

    std::vector<Node *>::iterator iter = lst->begin();
    ++iter;

    std::vector<ParseResult *> elements;

    while (iter != lst->end()) {
        ParseResult *el = new ParseResult();
        bool res =
            Form::Proc::Inst::parse(
                gen,
                dfn,
                block,
                (*iter),
                false,
                false,
                array_type->array_type,
                el
            );

        if (!res) {
            return false;
        }
        if (!el->type->isEqualTo(array_type->array_type)) {
            std::string exptype;
            std::string gottype;
            array_type->array_type->toStringProper(&exptype);
            el->type->toStringProper(&gottype);

            Error *e = new Error(
                ErrorInst::Generator::IncorrectType,
                (*iter),
                exptype.c_str(), gottype.c_str()
            );
            ctx->er->addError(e);
            return false;
        }
        elements.push_back(el);
        block = el->block;

        ++iter;
    }

    if ((array_type->array_size != 0)
            && (array_type->array_size != (int) elements.size())) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectNumberOfArrayElements,
            n,
            elements.size(), array_type->array_size
        );
        ctx->er->addError(e);
        return NULL;
    }

    *size = (int) elements.size();
    array_type = ctx->tr->getArrayType(array_type->array_type, *size);

    llvm::Type *llvm_array_type =
        ctx->toLLVMType(array_type, n, false);
    if (!llvm_array_type) {
        return NULL;
    }

    llvm::IRBuilder<> builder(block);

    llvm::Value *llvm_array = builder.CreateAlloca(llvm_array_type);
    std::vector<llvm::Value *> indices;
    indices.push_back(ctx->nt->getLLVMZero());

    for (int i = 0; i < (int) elements.size(); ++i) {
        indices.push_back(ctx->nt->getNativeInt(i));

        llvm::Value *res = builder.Insert(
                               llvm::GetElementPtrInst::Create(
                                   llvm_array,
                                   llvm::ArrayRef<llvm::Value*>(indices)
                               ),
                               "asdf"
                           );

        builder.CreateStore(elements[i]->value, res);

        indices.pop_back();
        delete elements[i];
    }

    pr->set(block, array_type, llvm_array);

    if (getAddress) {
        pr->type = ctx->tr->getPointerType(array_type);
    } else {
        /* Add a load instruction */
        llvm::Value *pvalue =
            llvm::cast<llvm::Value>(builder.CreateLoad(llvm_array));
        pr->value = pvalue;
    }

    return true;
}
}}}}
