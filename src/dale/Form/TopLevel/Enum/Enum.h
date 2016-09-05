#ifndef DALE_FORM_TOPLEVEL_ENUM
#define DALE_FORM_TOPLEVEL_ENUM

#include "../../../Units/Units.h"

namespace dale
{
/*! Parse a top-level enum form.
 *  @param units The units context.
 *  @param node The node being parsed.
 *  @param name The unqualified name of the enum being parsed.
 */
bool FormTopLevelEnumParse(Units *units, Node *node, const char *name);
}

#endif
