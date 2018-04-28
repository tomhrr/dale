#ifndef DALE_FORM_STRUCT
#define DALE_FORM_STRUCT

#include "../../Units/Units.h"

namespace dale {
/*! Parse a struct definition form.
 *  @param units The units context.
 *  @param node The node containing the definition.
 *  @param name The name of the new struct type.
 */
bool FormStructParse(Units *units, Node *n, const char *name);
}

#endif
