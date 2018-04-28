#ifndef DALE_FORM_PROCBODY
#define DALE_FORM_PROCBODY

#include "../../Units/Units.h"

namespace dale {
/*! Parse a procedure body form.
 *  @param units The units context.
 *  @param node The node containing the procedure body.
 *  @param fn The current function.
 *  @param llvm_fn The LLVM function for the current function.
 *  @param skip The number of nodes to skip in node.
 *  @param is_anonymous Whether the current function is anonymous.
 *  @param return_value The retval value, if applicable.
 */
bool FormProcBodyParse(Units *units, Node *node, Function *fn,
                       llvm::Function *llvm_fn, int skip,
                       bool is_anonymous,
                       llvm::Value *return_value = NULL);
bool resolveDeferredGotos(Context *ctx, Node *node, Function *fn,
                          llvm::BasicBlock *block);
bool terminateBlocks(Context *ctx, Function *fn,
                     llvm::Function *llvm_fn, llvm::Value *last_value,
                     Type *last_type, Node *last_position);
void removePostTerminators(llvm::Function *llvm_fn);
}

#endif
