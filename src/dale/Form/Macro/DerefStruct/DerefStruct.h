#ifndef DALE_FORM_MACRO_DEREFSTRUCT
#define DALE_FORM_MACRO_DEREFSTRUCT

#include "../../../Context/Context.h"
#include "../../../Node/Node.h"

namespace dale {
/*! The dereference-struct core macro form.
 *  @param ctx The context.
 *  @param node The node list.
 *
 *  Expands (:@ struct-ptr element) to (: (@ struct-ptr) element).
 */
Node *FormMacroDerefStructParse(Context *ctx, Node *node);
}

#endif
