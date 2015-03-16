#ifndef DALE_FORM_LINKAGE_STRUCT
#define DALE_FORM_LINKAGE_STRUCT

#include "../../../Context/Context.h"
#include "../../../Node/Node.h"

namespace dale {
/*! Parse a struct linkage form.
 *  @param ctx The context.
 *  @param node The node containing the linkage.
 */
int FormLinkageStructParse(Context *ctx, Node *node);
}

#endif
