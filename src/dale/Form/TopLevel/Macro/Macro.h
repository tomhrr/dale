#ifndef DALE_FORM_TOPLEVEL_MACRO
#define DALE_FORM_TOPLEVEL_MACRO

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level macro form.
 *  @param units The units context.
 *  @param node The node being parsed.
 *  @param name The unqualified name of the macro being parsed.
 */
bool FormTopLevelMacroParse(Units *units, Node *node, const char *name);
}

#endif
