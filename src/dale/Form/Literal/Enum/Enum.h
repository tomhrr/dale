#ifndef DALE_FORM_LITERAL_ENUM
#define DALE_FORM_LITERAL_ENUM

#include "../../../Context/Context.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../Type/Type.h"

namespace dale {
bool
FormLiteralEnumParse(Generator *gen,
      llvm::BasicBlock *block,
      Node *n,
      Enum *myenum,
      Type *myenumtype,
      Struct *myenumstructtype,
      bool getAddress,
      ParseResult *pr);
}

#endif
