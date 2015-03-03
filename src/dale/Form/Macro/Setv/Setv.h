#ifndef DALE_FORM_MACRO_SETV
#define DALE_FORM_MACRO_SETV

#include "../../../Node/Node.h"
#include "../../../Context/Context.h"

namespace dale {
/*! The setv core macro form.
 *  @param ctx The context.
 *  @param node The node list.
 *
 *  Expands (setv value new-value) to (setf (# value) new-value).
 */
Node *FormMacroSetvParse(Context *ctx, Node *node);
}

#endif
