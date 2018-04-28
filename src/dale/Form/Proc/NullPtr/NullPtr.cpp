#include "NullPtr.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Type/Type.h"

namespace dale {
bool FormProcNullPtrParse(Units *units, Function *fn,
                          llvm::BasicBlock *block, Node *node,
                          bool get_address, bool prefixed_with_core,
                          ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("nullptr", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;

    Type *type = FormTypeParse(units, (*lst)[1], false, false);
    if (!type) {
        return false;
    }
    Type *ptr_type = ctx->tr->getPointerType(type);

    llvm::Type *llvm_ptr_type =
        ctx->toLLVMType(ptr_type, NULL, false, false);
    if (!llvm_ptr_type) {
        return false;
    }

    llvm::IRBuilder<> builder(block);
    llvm::Value *null_ptr =
        builder.CreateIntToPtr(ctx->nt->getLLVMZero(), llvm_ptr_type);
    pr->set(block, ptr_type, null_ptr);

    return true;
}
}
