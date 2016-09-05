#ifndef DALE_FORM_TOPLEVEL_GLOBALVARIABLE
#define DALE_FORM_TOPLEVEL_GLOBALVARIABLE

#include "../../../Units/Units.h"

namespace dale
{
/*! Parse a top-level global variable form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelGlobalVariableParse(Units *units, Node *node);
llvm::Constant *parseLiteral(Units *units, Type *type, Node *top, int *size);
}

#endif
