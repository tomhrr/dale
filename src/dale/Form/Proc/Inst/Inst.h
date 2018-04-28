#ifndef DALE_FORM_PROC_INST
#define DALE_FORM_PROC_INST

#include "../../../Units/Units.h"

namespace dale {
/*! The standard parser for procedure-body instructions.
 *  @param units The units context.
 *  @param fn The function currently in scope.
 *  @param block The current block.
 *  @param node The node being parsed.
 *  @param get_address Whether to return the address of the result.
 *  @param prefixed_with_core Whether to treat the node list (if it is
 *                            a list) as being prefixed with the "core"
 * form.
 *  @param wanted_type A preferred response type.
 *  @param pr The parse result for the response.
 *  @param no_copy Whether to skip copying the result.
 *
 *  One of the most awkward form-parsing functions, internally, since
 *  there is a good deal of flexibility around what can appear as the
 *  first symbol in an instruction.
 */
bool FormProcInstParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       Type *wanted_type, ParseResult *pr,
                       bool no_copy = false);
}

#endif
