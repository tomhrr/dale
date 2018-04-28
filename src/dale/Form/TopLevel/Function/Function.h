#ifndef DALE_FORM_TOPLEVEL_FUNCTION
#define DALE_FORM_TOPLEVEL_FUNCTION

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level function form.
 *  @param units The units context.
 *  @param node The node being parsed.
 *  @param name The unqualified name of the function being parsed.
 */
bool FormTopLevelFunctionParse(Units *units, Node *node,
                               const char *name);
}

#endif
