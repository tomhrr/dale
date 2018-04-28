#ifndef DALE_FORM_PROC_DEF
#define DALE_FORM_PROC_DEF

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a procedure-body def form.
 *  @param units The units context.
 *  @param fn The function currently in scope.
 *  @param block The current block.
 *  @param node The node being parsed.
 *  @param get_address Whether to return the address of the result.
 *  @param prefixed_with_core (Unused by this form.)
 *  @param pr The parse result for the response.
 */
bool FormProcDefParse(Units *units, Function *fn,
                      llvm::BasicBlock *block, Node *node,
                      bool get_address, bool prefixed_with_core,
                      ParseResult *pr);

Function *getInitFn(Context *ctx, Type *type);
bool initialise(Context *ctx, llvm::IRBuilder<> *builder, Type *type,
                llvm::Value *value, Function *init_fn);
bool storeValue(Context *ctx, Node *node, Type *type,
                llvm::IRBuilder<> *builder, llvm::Value *dst_ptr,
                ParseResult *pr);
}

#endif
