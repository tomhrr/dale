#ifndef DALE_OPERATION_CAST
#define DALE_OPERATION_CAST

#include "../../ParseResult/ParseResult.h"
#include "../../Context/Context.h"
#include "../../Element/Type/Type.h"
#include "../../Node/Node.h"

#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/TypeBuilder.h"

namespace dale
{
namespace Operation
{
namespace Cast
{
bool execute(Context *ctx,
            llvm::Module *mod,
            llvm::BasicBlock *block,
            llvm::Value *value,
            Element::Type *from_type,
            Element::Type *to_type,
            Node *n,
            bool implicit,
            ParseResult *pr);
}
}
}

#endif
