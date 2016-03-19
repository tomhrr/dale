#ifndef DALE_OPERATION_COPY
#define DALE_OPERATION_COPY

#include "../../ParseResult/ParseResult.h"
#include "../../Context/Context.h"
#include "../../Type/Type.h"
#include "../../Node/Node.h"

#include "../../llvm_Module.h"
#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"

namespace dale
{
namespace Operation
{
/*! Copy a parsed value, if required.
 *  @param ctx The current context.
 *  @param fn The current function.
 *  @param node The node of the value being copied.
 *  @param value_pr The parse result containing the value to be copied.
 *  @param pr The parse result into which the result should be put.
 */
bool Copy(Context *ctx, Function *fn, Node *node, ParseResult *value_pr,
          ParseResult *pr);
}
}

#endif
