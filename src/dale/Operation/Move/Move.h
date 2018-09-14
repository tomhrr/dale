#ifndef DALE_OPERATION_MOVE
#define DALE_OPERATION_MOVE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Type/Type.h"

#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Operation {
/*! Move a parsed value, if required.
 *  @param ctx The current context.
 *  @param fn The current function.
 *  @param value_pr The parse result containing the value to be moved.
 *  @param pr The parse result into which the result should be put.
 */
bool Move(Context *ctx, Function *fn, ParseResult *value_pr,
          ParseResult *pr);
}
}

#endif
