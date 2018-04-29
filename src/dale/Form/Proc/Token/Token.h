#ifndef DALE_FORM_PROC_TOKEN
#define DALE_FORM_PROC_TOKEN

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"

namespace dale {
/*! The standard parser for procedure-body tokens (atoms).
 *  @param units The units context.
 *  @param fn The function currently in scope.
 *  @param block The current block.
 *  @param node The node being parsed.
 *  @param get_address Whether to return the address of the result.
 *  @param prefixed_with_core (Unused by this function.)
 *  @param wanted_type A preferred response type.
 *  @param pr The parse result for the response.
 *
 *  It is assumed that the node is a single token node.
 */
bool FormProcTokenParse(Units *units, Function *fn,
                        llvm::BasicBlock *block, Node *node,
                        bool get_address, bool prefixed_with_core,
                        Type *wanted_type, ParseResult *pr);
}

#endif
