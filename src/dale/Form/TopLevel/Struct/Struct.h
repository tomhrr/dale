#ifndef DALE_FORM_TOPLEVEL_STRUCT
#define DALE_FORM_TOPLEVEL_STRUCT

#include "../../../Units/Units.h"

namespace dale
{
/*! Parse a top-level struct form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelStructParse(Units *units, Node *n);
}

#endif
