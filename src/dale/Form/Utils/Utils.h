#ifndef FORM_UTILS
#define FORM_UTILS

#include "../../Units/Units.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Function/Function.h"
#include "../Type/Type.h"
#include "../../llvm_Function.h"

namespace dale
{
/*! Load a value into the given parse result.
 *  @param units The units context.
 *  @param fn The function currently in scope.
 *  @param block The current block.
 *  @param var_value_node The node containing the value.
 *  @param get_address Whether to return the address of the result.
 *  @param wanted_type The expected type for the result.
 *  @param pr The parse result for the response.
 *
 *  This is a very thin wrapper around FormProcInstParse.  It
 *  short-circuits that process when the node is either a direct
 *  reference to a variable, or a direct reference to a variable
 *  dereferencing.
 */
bool FormProcessValue(Units *units, Function *fn, llvm::BasicBlock *block,
                      Node *var_value_node, bool get_address,
                      Type *wanted_type, ParseResult *pr);
}

#endif
