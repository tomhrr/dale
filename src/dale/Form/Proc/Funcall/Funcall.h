#ifndef DALE_FORM_PROC_FUNCALL
#define DALE_FORM_PROC_FUNCALL

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a funcall form (function pointer call).
 *  @param units The units context.
 *  @param fn The function currently in scope.
 *  @param block The current block.
 *  @param node The node being parsed.
 *  @param get_address Whether to return the address of the result.
 *  @param prefixed_with_core (Unused by this form.)
 *  @param pr The parse result for the response.
 */
bool FormProcFuncallParse(Units *units, Function *fn,
                          llvm::BasicBlock *block, Node *node,
                          bool get_address, bool prefixed_with_core,
                          ParseResult *pr);
}

#endif
