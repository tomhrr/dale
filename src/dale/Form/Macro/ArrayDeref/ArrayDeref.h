#ifndef DALE_FORM_MACRO_ARRAYDEREF
#define DALE_FORM_MACRO_ARRAYDEREF

#include "../../../Node/Node.h"
#include "../../../Context/Context.h"

namespace dale {
Node *
FormMacroArrayDerefParse(Context *ctx, Node *node);
}

#endif
