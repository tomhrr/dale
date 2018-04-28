#ifndef DALE_FORM_PROC_QUOTE
#define DALE_FORM_PROC_QUOTE

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a procedure-body quote statement.
 *  @param units The units context.
 *  @param fn The function currently in scope.
 *  @param block The current block.
 *  @param node The node being parsed.
 *  @param get_address Whether to return the address of the result.
 *  @param prefixed_with_core (Unused by this form.)
 *  @param pr The parse result for the response.
 *
 *  This takes an arbitrary form and returns a pointer to a static
 *  DNode representing that form.  It is similar to ' in CL/Scheme,
 *  except that there's no concept of a 'symbol' as such here.
 */
bool FormProcQuoteParse(Units *units, Function *fn,
                        llvm::BasicBlock *block, Node *node,
                        bool get_address, bool prefixed_with_core,
                        ParseResult *pr);
}

#endif
