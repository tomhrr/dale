#include "Enum.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"

namespace dale { namespace Form { namespace Literal { namespace Enum {
bool 
parse(Generator *gen,
      llvm::BasicBlock *block,
      Node *n,
      Element::Enum *myenum,
      Element::Type *myenumtype,
      Element::Struct *myenumstructtype,
      bool getAddress,
      ParseResult *pr)
{
    Context *ctx = gen->ctx;

    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, n,
            "atom", "enum literal", "list"
        );
        ctx->er->addError(e);
        return false;
    }

    int num = myenum->nameToNumber(n->token->str_value.c_str());
    if (num == ENUM_NOTFOUND) {
        Error *e = new Error(
            ErrorInst::Generator::EnumValueDoesNotExist,
            n,
            n->token->str_value.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    llvm::IRBuilder<> builder(block);

    llvm::Value *sp = llvm::cast<llvm::Value>(
                          builder.CreateAlloca(myenumstructtype->type)
                      );

    std::vector<llvm::Value *> two_zero_indices;
    stl::push_back2(&two_zero_indices,
                        ctx->nt->getLLVMZero(), ctx->nt->getLLVMZero());
    llvm::Value *res =
        builder.CreateGEP(sp,
                          llvm::ArrayRef<llvm::Value*>(two_zero_indices));

    llvm::Type *llvm_type =
        ctx->toLLVMType(myenumstructtype->element_types.at(0),
                       NULL, false);
    if (!llvm_type) {
        return false;
    }

    builder.CreateStore(llvm::ConstantInt::get(
                            llvm_type, num),
                        res);

    if (getAddress) {
        pr->set(block, ctx->tr->getPointerType(myenumtype), sp);
        return true;
    } else {
        llvm::Value *final_value =
            builder.CreateLoad(sp);

        pr->set(block, myenumtype, final_value);
        return true;
    }
}}}}}
