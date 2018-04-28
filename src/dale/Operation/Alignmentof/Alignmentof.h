#ifndef DALE_OPERATION_ALIGNMENTOF
#define DALE_OPERATION_ALIGNMENTOF

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Type/Type.h"
#include "../../Unit/Unit.h"

#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Operation {
/*! Get the required alignment for a particular type.
 *  @param ctx The current context.
 *  @param block The current block.
 *  @param type The type.
 *  @param pr The parse result for the alignment result.
 */
bool Alignmentof(Context *ctx, llvm::BasicBlock *block, Type *type,
                 ParseResult *pr);
}
}

#endif
