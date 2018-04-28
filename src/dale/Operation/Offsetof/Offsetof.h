#ifndef DALE_OPERATION_OFFSETOF
#define DALE_OPERATION_OFFSETOF

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
/*! Get the offset of a particular struct member.
 *  @param unit The current unit.
 *  @param block The current block.
 *  @param type The struct type.
 *  @param member_name The struct member name.
 *  @param pr The parse result for the offset.
 */
bool Offsetof(Unit *unit, llvm::BasicBlock *block, Type *type,
              const char *field_name, ParseResult *pr);
/*! Get the offset of a particular struct member by index.
 *  @param unit The current unit.
 *  @param type The struct type.
 *  @param index The struct member index.
 *
 *  Unlike the other Offsetof calls, this one returns the actual
 *  result as a number, rather than within a ParseResult.
 */
size_t OffsetofGetByIndex(Unit *unit, Type *type, int index);
}
}

#endif
