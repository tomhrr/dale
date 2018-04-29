#ifndef DALE_FORM_VALUE
#define DALE_FORM_VALUE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../Units/Units.h"

namespace dale {
llvm::Constant *FormValueParse(Units *units, Type *type, Node *top,
                               int *size);
}

#endif
