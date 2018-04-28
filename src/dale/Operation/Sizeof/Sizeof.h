#ifndef DALE_OPERATION_SIZEOF
#define DALE_OPERATION_SIZEOF

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
/*! Get the size of a particular type.
 *  @param ctx The current context.
 *  @param block The current block.
 *  @param type The type.
 *  @param pr The parse result for the size.
 */
bool Sizeof(Context *ctx, llvm::BasicBlock *block, Type *type,
            ParseResult *pr);
/*! Get the size of a particular type.
 *  @param unit The current unit.
 *  @param type The type.
 *
 *  Unlike Sizeof, this returns the actual result as a number, rather
 *  than within a ParseResult.
 */
size_t SizeofGet(Unit *unit, Type *type);
}
}

#endif
