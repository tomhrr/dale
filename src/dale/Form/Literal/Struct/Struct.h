#ifndef DALE_FORM_LITERAL_STRUCT
#define DALE_FORM_LITERAL_STRUCT

#include "../../../Context/Context.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"

namespace dale {
bool
FormLiteralStructParse(Units *units,
      Function *dfn,
      llvm::BasicBlock *block,
      Node *n,
      const char *struct_name,
      Struct *str,
      Type *structtype,
      bool get_address,
      ParseResult *pr);
}

#endif
