#ifndef DALE_FORM_LINKAGE_ENUM
#define DALE_FORM_LINKAGE_ENUM

#include "../../../Context/Context.h"
#include "../../../Node/Node.h"

namespace dale {
/*! Parse an enum linkage form.
 *  @param ctx The context.
 *  @param node The node containing the linkage.
 */
int FormLinkageEnumParse(Context *ctx, Node *node);
}

#endif
