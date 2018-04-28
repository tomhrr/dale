#ifndef DALE_FORM_TOPLEVEL_NAMESPACE
#define DALE_FORM_TOPLEVEL_NAMESPACE

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level namespace form.
 *  @param units The units context.
 *  @param node The node containing the namespace form.
 */
bool FormTopLevelNamespaceParse(Units *units, Node *node);
}

#endif
