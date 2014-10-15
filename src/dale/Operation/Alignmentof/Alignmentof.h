#ifndef DALE_OPERATION_ALIGNMENTOF
#define DALE_OPERATION_ALIGNMENTOF

#include "../../ParseResult/ParseResult.h"
#include "../../Context/Context.h"
#include "../../Element/Type/Type.h"
#include "../../Node/Node.h"
#include "../../Unit/Unit.h"

#include "llvm/Module.h"
#include "llvm/Function.h"
#include "../../llvm_IRBuilder.h"

namespace dale
{
namespace Operation
{
namespace Alignmentof
{
bool execute(Context *ctx,
             llvm::BasicBlock *block,
             Element::Type *type,
             ParseResult *pr);
}
}
}

#endif
