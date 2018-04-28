#ifndef DALE_FORM_MACRO_ARRAYDEREF
#define DALE_FORM_MACRO_ARRAYDEREF

#include "../../../Context/Context.h"
#include "../../../Node/Node.h"

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
