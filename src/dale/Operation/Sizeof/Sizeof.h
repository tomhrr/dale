#ifndef DALE_OPERATION_SIZEOF
#define DALE_OPERATION_SIZEOF

#include "../../ParseResult/ParseResult.h"
#include "../../Context/Context.h"
#include "../../Element/Type/Type.h"
#include "../../Node/Node.h"
#include "../../Unit/Unit.h"

#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/TypeBuilder.h"

namespace dale
{
namespace Operation
{
namespace Sizeof
{
bool execute(Context *ctx,
             llvm::BasicBlock *block,
             Element::Type *type,
             ParseResult *pr);
size_t get(Unit *unit,
           Element::Type *type);
}
}
}

#endif
