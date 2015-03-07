#ifndef DALE_FORM_PROC_GETDNODES
#define DALE_FORM_PROC_GETDNODES

namespace dale
{
/*! Parse a procedure-body get-dnodes statement.
 *  @param units The units context.
 *  @param fn The function currently in scope.
 *  @param block The current block.
 *  @param node The node being parsed.
 *  @param get_address Whether to return the address of the result.
 *  @param prefixed_with_core (Unused by this form.)
 *  @param pr The parse result for the response.
 *
 *  get-dnodes is an undocumented (well, except for here) form that
 *  takes an arbitrary form and returns a pointer to a static DNode
 *  representing that form.  It is only used once, in the
 *  quasiquotation macro; hopefully there is another way to handle the
 *  problem in that particular case, or perhaps the use of this in
 *  other contexts will become apparent over time.
 */
bool
FormProcGetDNodesParse(Units *units, Function *fn, llvm::BasicBlock *block,
                       Node *node, bool get_address, bool prefixed_with_core,
                       ParseResult *pr);
}

#endif
