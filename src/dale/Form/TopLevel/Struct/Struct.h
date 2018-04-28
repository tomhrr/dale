#ifndef DALE_FORM_TOPLEVEL_STRUCT
#define DALE_FORM_TOPLEVEL_STRUCT

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level struct form.
 *  @param units The units context.
 *  @param node The node being parsed.
 *  @param name The unqualified name of the struct being parsed.
 */
bool FormTopLevelStructParse(Units *units, Node *n, const char *name);
}

#endif
