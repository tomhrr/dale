#ifndef DALE_FORM_TOPLEVEL_MODULE
#define DALE_FORM_TOPLEVEL_MODULE

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level module form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelModuleParse(Units *units, Node *node);
}

#endif
