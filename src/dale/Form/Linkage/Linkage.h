#ifndef DALE_FORM_LINKAGE
#define DALE_FORM_LINKAGE

#include "../../Context/Context.h"
#include "../../Node/Node.h"

namespace dale {
/*! Parse an enum linkage form.
 *  @param ctx The context.
 *  @param node The node containing the linkage.
 */
int FormLinkageParse(Context *ctx, Node *node);
}

#endif
