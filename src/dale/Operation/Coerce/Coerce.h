#ifndef DALE_OPERATION_COERCE
#define DALE_OPERATION_COERCE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Type/Type.h"

#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Operation {
/*! Coerce a value from one type to another.
 *  @param ctx The current context.
 *  @param block The current block.
 *  @param value The value to be cast.
 *  @param from_type The type from which the value should be cast.
 *  @param to_type The type to which the value should be cast.
 *  @param pr The parse result for the coercion result.
 *
 *  The coercion operation is akin to cast, except that it only
 *  applies when the from type is a char array and the to type is a
 *  char pointer.
 */
bool Coerce(Context *ctx, llvm::BasicBlock *block, llvm::Value *value,
            Type *from_type, Type *to_type, ParseResult *pr);
}
}

#endif
