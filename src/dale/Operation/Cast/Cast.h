#ifndef DALE_OPERATION_CAST
#define DALE_OPERATION_CAST

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Type/Type.h"

#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Operation {
/*! Cast the value from one type to another.
 *  @param ctx The current context.
 *  @param block The current block.
 *  @param value The value to be cast.
 *  @param from_type The type from which the value should be cast.
 *  @param to_type The type to which the value should be cast.
 *  @param n The reference node (for errors).
 *  @param implicit Allow pointers to be cast to integers, and v.v.
 *  @param pr The parse result for the cast result.
 */
bool Cast(Context *ctx, llvm::BasicBlock *block, llvm::Value *value,
          Type *from_type, Type *to_type, Node *n, bool implicit,
          ParseResult *pr);
}
}

#endif
