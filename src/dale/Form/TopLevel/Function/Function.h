#ifndef DALE_FORM_TOPLEVEL_FUNCTION
#define DALE_FORM_TOPLEVEL_FUNCTION

#include "../../../Units/Units.h"

namespace dale
{
/*! Parse a top-level function form.
 *  @param units The units context.
 *  @param node The node being parsed.
 */
bool FormTopLevelFunctionParse(Units *units, Node *node);
}

#endif
