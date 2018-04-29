#include "Cast.h"

#include <string>

#include "../../ErrorType/ErrorType.h"

using namespace dale::ErrorInst;

namespace dale {
namespace Operation {
bool Cast(Context *ctx, llvm::BasicBlock *block, llvm::Value *value,
          Type *from_type, Type *to_type, Node *n, bool implicit,
          ParseResult *pr) {
    llvm::IRBuilder<> builder(block);
    llvm::Value *res = NULL;
    std::string *struct_name;
    std::string *to_struct_name;

    llvm::Type *llvm_from_type =
        ctx->toLLVMType(from_type, NULL, false);
    if (!llvm_from_type) {
        return false;
    }
    llvm::Type *llvm_to_type = ctx->toLLVMType(to_type, NULL, false);
    if (!llvm_to_type) {
        return false;
    }

    if (from_type->isFloatingPointType() &&
        to_type->isFloatingPointType()) {
        int a = from_type->getFloatingPointRelativeSize();
        int b = to_type->getFloatingPointRelativeSize();
        if (a < b) {
            res = builder.CreateFPExt(value, llvm_to_type);
        } else {
            res = builder.CreateFPTrunc(value, llvm_to_type);
        }
    } else if (from_type->isFloatingPointType() &&
               to_type->isIntegerType()) {
        if (to_type->isSignedIntegerType()) {
            res = builder.CreateFPToSI(value, llvm_to_type);
        } else {
            res = builder.CreateFPToUI(value, llvm_to_type);
        }
    } else if (from_type->isIntegerType() &&
               to_type->isFloatingPointType()) {
        if (from_type->isSignedIntegerType()) {
            res = builder.CreateSIToFP(value, llvm_to_type);
        } else {
            res = builder.CreateUIToFP(value, llvm_to_type);
        }
    } else if ((from_type->isIntegerType() &&
                to_type->isIntegerType()) ||
               (from_type->base_type == BaseType::Bool &&
                to_type->isIntegerType()) ||
               (from_type->isIntegerType() &&
                to_type->base_type == BaseType::Bool)) {
        int size_pr = ctx->nt->internalSizeToRealSize(
            from_type->getIntegerSize());
        int ta_size =
            ctx->nt->internalSizeToRealSize(to_type->getIntegerSize());

        if (size_pr <= ta_size) {
            if (to_type->isSignedIntegerType()) {
                res = builder.CreateSExt(value, llvm_to_type);
            } else {
                res = builder.CreateZExt(value, llvm_to_type);
            }
        } else {
            res = builder.CreateTrunc(value, llvm_to_type);
        }
    } else if (!implicit && from_type->isIntegerType() &&
               !(from_type->points_to) && to_type->points_to) {
        res = builder.CreateIntToPtr(value, llvm_to_type);
    } else if (!implicit && from_type->points_to &&
               !(to_type->points_to) && to_type->isIntegerType()) {
        res = builder.CreatePtrToInt(value, llvm_to_type);
    } else if (from_type->points_to && to_type->points_to) {
        res = builder.CreateBitCast(value, llvm_to_type);
    } else if ((struct_name = &(from_type->struct_name)) &&
               struct_name->size() &&
               (to_struct_name = &(to_type->struct_name)) &&
               to_struct_name->size() && !from_type->is_const &&
               to_type->is_const) {
        res = builder.CreateBitCast(value, llvm_to_type);
    } else {
        std::string fts;
        std::string tts;
        from_type->toString(&fts);
        to_type->toString(&tts);

        Error *e = new Error(InvalidCast, n, fts.c_str(), tts.c_str());
        ctx->er->addError(e);
        return false;
    }

    pr->set(block, to_type, res);

    return true;
}
}
}
