#ifndef DALE_OPERATION_OFFSETOF
#define DALE_OPERATION_OFFSETOF

#include "../../ParseResult/ParseResult.h"
#include "../../Context/Context.h"
#include "../../Type/Type.h"
#include "../../Node/Node.h"
#include "../../Unit/Unit.h"

#include "../../llvm_Module.h"
#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"

namespace dale
{
namespace Operation
{
namespace Offsetof
{
bool execute(Context *ctx,
             llvm::BasicBlock *block,
             Type *type,
             const char *field_name,
             ParseResult *pr);
bool executeByIndex(Context *ctx,
                    llvm::BasicBlock *block,
                    Type *type,
                    int index,
                    ParseResult *pr);
size_t get(Unit *unit,
           Type *type,
           const char *field_name);
size_t getByIndex(Unit *unit,
                  Type *type,
                  int index);
}
}
}

#endif
