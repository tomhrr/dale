#ifndef DALE_OPERATION_OFFSETOF
#define DALE_OPERATION_OFFSETOF

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
namespace Offsetof
{
bool execute(Context *ctx,
             llvm::BasicBlock *block,
             Element::Type *type,
             const char *field_name,
             ParseResult *pr);
bool executeByIndex(Context *ctx,
                    llvm::BasicBlock *block,
                    Element::Type *type,
                    int index,
                    ParseResult *pr);
size_t get(Unit *unit,
           Element::Type *type,
           const char *field_name);
size_t getByIndex(Unit *unit,
                  Element::Type *type,
                  int index);
}
}
}

#endif
