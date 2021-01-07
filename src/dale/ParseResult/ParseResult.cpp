#include "ParseResult.h"

namespace dale {
void ParseResult::init() {
    block = NULL;
    type = NULL;
    value = NULL;
    address_of_value = NULL;
    type_of_address_of_value = NULL;

    treat_as_terminator = false;
    do_not_destruct = true;
    do_not_copy_with_setf = false;
    freshly_copied = false;
    value_is_lvalue = false;

    retval = NULL;
    retval_type = NULL;
    retval_used = false;
    retval_requires_init = false;
}

ParseResult::ParseResult() { init(); }

ParseResult::ParseResult(llvm::BasicBlock *block, Type *type,
                         llvm::Value *value) {
    init();
    set(block, type, value);
}

ParseResult::~ParseResult() {}

void ParseResult::copyTo(ParseResult *x) {
    x->treat_as_terminator = treat_as_terminator;
    x->do_not_destruct = do_not_destruct;
    x->do_not_copy_with_setf = do_not_copy_with_setf;
    x->block = block;
    x->type = type;
    x->value = value;
    x->address_of_value = address_of_value;
    x->freshly_copied = freshly_copied;
    x->value_is_lvalue = value_is_lvalue;
    x->type_of_address_of_value = type_of_address_of_value;
    x->retval_type = retval_type;
    x->retval = retval;
    x->retval_used = retval_used;
    x->retval_requires_init = retval_requires_init;
}

void ParseResult::set(llvm::BasicBlock *block, Type *type,
                      llvm::Value *value) {
    this->block = block;
    this->type = type;
    this->value = value;
}

bool ParseResult::setAddressOfValue(Context *ctx) {
    if (address_of_value) {
        return true;
    }

    llvm::IRBuilder<> builder(block);
    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return false;
    }
    if (retval && retval_used) {
        address_of_value = retval;
    } else {
        address_of_value = builder.CreateAlloca(llvm_type);
        builder.CreateStore(getValue(ctx), address_of_value);
    }
    return true;
}

bool ParseResult::getAddressOfValue(Context *ctx, ParseResult *pr) {
    bool res = setAddressOfValue(ctx);
    if (!res) {
        return false;
    }
    Type *new_type = (type_of_address_of_value)
                         ? type_of_address_of_value
                         : ctx->tr->getPointerType(type);

    pr->set(block, new_type, address_of_value);
    return true;
}

llvm::Value *ParseResult::getValue(Context *ctx) {
    if (retval && retval_used) {
        llvm::IRBuilder<> builder(block);
        return builder.CreateLoad(retval);
    } else if (!value && address_of_value) {
        llvm::IRBuilder<> builder(block);
        value = builder.CreateLoad(address_of_value);
        return value;
    } else {
        return value;
    }
}
}
