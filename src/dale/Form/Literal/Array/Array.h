#ifndef DALE_FORM_LITERAL_ARRAY
#define DALE_FORM_LITERAL_ARRAY

#include "../../../Context/Context.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"

namespace dale { namespace Form { namespace Literal { namespace Array {
bool
parse(Generator *gen,
      Element::Function *dfn,
      llvm::BasicBlock *block,
      Node *n,
      const char *name,
      Element::Type *array_type,
      bool getAddress,
      int *size,
      ParseResult *pr);
}}}}

#endif
