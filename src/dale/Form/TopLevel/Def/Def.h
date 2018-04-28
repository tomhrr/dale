#ifndef DALE_FORM_TOPLEVEL_DEF
#define DALE_FORM_TOPLEVEL_DEF

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level def form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelDefParse(Units *units, Node *node);
}

#endif
