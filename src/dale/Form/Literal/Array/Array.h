#ifndef DALE_FORM_LITERAL_ARRAY
#define DALE_FORM_LITERAL_ARRAY

#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../Type/Type.h"
#include "../../../Function/Function.h"

namespace dale {
bool
FormLiteralArrayParse(Units *units,
      Function *dfn,
      llvm::BasicBlock *block,
      Node *n,
      const char *name,
      Type *array_type,
      bool get_address,
      int *size,
      ParseResult *pr);
}

#endif
