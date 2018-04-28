#ifndef DALE_FORM_TOPLEVEL_DO
#define DALE_FORM_TOPLEVEL_DO

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level do form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelDoParse(Units *units, Node *node);
}

#endif
