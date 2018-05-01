#ifndef DALE_FORM_VALUE
#define DALE_FORM_VALUE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../Units/Units.h"

namespace dale {
/*! Parse a value.
 *  @param units The units context.
 *  @param type The type of the value.
 *  @param top The value node.
 *
 *  This is primarily intended for use outside of procedures, because
 *  FormProcInstParse does the job within procedures.  However, there
 *  are instances where this needs to be used within a procedure, e.g.
 *  for intern-scoped variables (since those can't depend on procedure
 *  state).
 */
llvm::Constant *FormValueParse(Units *units, Type *type, Node *top,
                               int *size);
}

#endif
