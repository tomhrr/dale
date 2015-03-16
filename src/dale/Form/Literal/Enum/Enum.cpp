#include "Enum.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"

using namespace dale::ErrorInst::Generator;

namespace dale {
bool 
FormLiteralEnumParse(Units *units, llvm::BasicBlock *block, Node *node,
                     Enum *enum_obj, Type *enum_type,
                     Struct *enum_st, bool get_address,
                     ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!node->is_token) {
        Error *e = new Error(UnexpectedElement, node,
                             "atom", "enum literal", "list");
        ctx->er->addError(e);
        return false;
    }

    const char *member_name = node->token->str_value.c_str();
    if (!enum_obj->existsMember(member_name)) {
        Error *e = new Error(EnumValueDoesNotExist, node, member_name);
        ctx->er->addError(e);
        return false;
    }
    int member_index = enum_obj->memberToIndex(member_name);

    llvm::IRBuilder<> builder(block);
    llvm::Value *storage =
        llvm::cast<llvm::Value>(builder.CreateAlloca(enum_st->type));

    std::vector<llvm::Value *> two_zero_indices;
    STL::push_back2(&two_zero_indices,
                    ctx->nt->getLLVMZero(), ctx->nt->getLLVMZero());

    llvm::Value *storage_ptr =
        builder.CreateGEP(storage,
                          llvm::ArrayRef<llvm::Value*>(two_zero_indices));

    llvm::Type *llvm_type =
        ctx->toLLVMType(enum_st->member_types.at(0), NULL, false);
    if (!llvm_type) {
        return false;
    }

    builder.CreateStore(llvm::ConstantInt::get(llvm_type, member_index),
                        storage_ptr);

    if (get_address) {
        pr->set(block, ctx->tr->getPointerType(enum_type), storage);
    } else {
        llvm::Value *final_value = builder.CreateLoad(storage);
        pr->set(block, enum_type, final_value);
    }

    return true;
}
}
