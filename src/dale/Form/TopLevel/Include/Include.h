#ifndef DALE_FORM_TOPLEVEL_INCLUDE
#define DALE_FORM_TOPLEVEL_INCLUDE

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level include form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelIncludeParse(Units *units, Node *node);
}

#endif
