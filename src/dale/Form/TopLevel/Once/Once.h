#ifndef DALE_FORM_TOPLEVEL_ONCE
#define DALE_FORM_TOPLEVEL_ONCE

#include "../../../Units/Units.h"

namespace dale
{
/*! Parse a top-level once form.
 *  @param units The units context.
 *  @param node The node containing the once form.
 */
bool FormTopLevelOnceParse(Units *units, Node *node);
}

#endif
