#ifndef DALE_FORM_MACRO_DEREFSTRUCTDEREF
#define DALE_FORM_MACRO_DEREFSTRUCTDEREF

#include "../../../Context/Context.h"
#include "../../../Node/Node.h"

namespace dale {
/*! The dereference-struct-dereference core macro form.
 *  @param ctx The context.
 *  @param node The node list.
 *
 *  Expands (@:@ struct-ptr element) to (@ (: (@ struct-ptr) element)).
 */
Node *FormMacroDerefStructDerefParse(Context *ctx, Node *node);
}

#endif
