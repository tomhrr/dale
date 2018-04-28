#ifndef DALE_FORM_TOPLEVEL_INST
#define DALE_FORM_TOPLEVEL_INST

#include "../../../Units/Units.h"

namespace dale {
/*! Parse an arbitrary top-level form.
 *  @param units The units context.
 *  @param block The node to be parsed.
 */
bool FormTopLevelInstParse(Units *units, Node *node);
}

#endif
