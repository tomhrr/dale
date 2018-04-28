#ifndef DALE_FORM_TOPLEVEL_USINGNAMESPACE
#define DALE_FORM_TOPLEVEL_USINGNAMESPACE

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level using-namespace form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelUsingNamespaceParse(Units *units, Node *node);
}

#endif
