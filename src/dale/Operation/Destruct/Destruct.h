#ifndef DALE_OPERATION_DESTRUCT
#define DALE_OPERATION_DESTRUCT

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Type/Type.h"

#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Operation {
/*! Destruct a parsed value, if required.
 *  @param ctx The current context.
 *  @param value_pr The parse result containing the value to be
 * destroyed.
 *  @param pr The parse result into which the result should be put.
 *  @param builder The current builder (optional).
 *  @param value_is_ptr Whether the value is a pointer (optional).
 */
bool Destruct(Context *ctx, ParseResult *value_pr, ParseResult *pr,
              llvm::IRBuilder<> *builder = NULL,
              bool value_is_ptr = false);
}
}

#endif
