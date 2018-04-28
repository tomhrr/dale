#ifndef DALE_FORM_MACRO_STRUCTDEREF
#define DALE_FORM_MACRO_STRUCTDEREF

#include "../../../Context/Context.h"
#include "../../../Node/Node.h"

namespace dale {
/*! The struct-dereference core macro form.
 *  @param ctx The context.
 *  @param node The node list.
 *
 *  Expands (@: struct element) to (@ (: struct element)).
 */
Node *FormMacroStructDerefParse(Context *ctx, Node *node);
}

#endif
