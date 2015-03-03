#ifndef DALE_FORM_MACRO_ARRAYDEREF
#define DALE_FORM_MACRO_ARRAYDEREF

#include "../../../Node/Node.h"
#include "../../../Context/Context.h"

namespace dale {
/*! The array-dereference core macro form.
 *  @param ctx The context.
 *  @param node The node list.
 *
 *  Expands (@$ array-ptr index) to (@ ($ array-ptr index)).
 */
Node *FormMacroArrayDerefParse(Context *ctx, Node *node);
}

#endif
