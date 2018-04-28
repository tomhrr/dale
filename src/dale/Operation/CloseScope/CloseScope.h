#ifndef DALE_OPERATION_CLOSESCOPE
#define DALE_OPERATION_CLOSESCOPE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Type/Type.h"

#include "../../llvm_Function.h"
#include "../../llvm_IRBuilder.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Operation {
/*! Close the current scope, destructing values as required.
 *  @param ctx The current context.
 *  @param fn The current function.
 *  @param block The current block.
 *  @param skip_value A single value that should not be destructed.
 *  @param entire Whether the entire current function can be closed.
 */
bool CloseScope(Context *ctx, Function *fn, llvm::BasicBlock *block,
                llvm::Value *skip_value, bool entire);
}
}

#endif
