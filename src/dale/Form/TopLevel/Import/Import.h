#ifndef DALE_FORM_TOPLEVEL_IMPORT
#define DALE_FORM_TOPLEVEL_IMPORT

#include "../../../Units/Units.h"

namespace dale {
/*! Parse a top-level import form.
 *  @param units The units context.
 *  @param node The node containing the import form.
 */
bool FormTopLevelImportParse(Units *units, Node *node);
}

#endif
