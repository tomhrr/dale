#include "Enum.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"

using namespace dale::ErrorInst;

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
    if (!enum_obj->existsName(member_name)) {
        Error *e = new Error(EnumValueDoesNotExist, node, member_name);
        ctx->er->addError(e);
        return false;
    }
    int member_index = enum_obj->nameToIndex(member_name);

    llvm::IRBuilder<> builder(block);
    llvm::Value *storage =
        llvm::cast<llvm::Value>(builder.CreateAlloca(enum_st->type));

    llvm::Value *storage_ptr =
        builder.CreateGEP(storage, ctx->nt->getTwoLLVMZeros());

    llvm::Type *llvm_type =
        ctx->toLLVMType(enum_st->member_types[0], NULL, false);
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
