#ifndef DALE_FORM_LITERAL_ENUM
#define DALE_FORM_LITERAL_ENUM

#include "../../../Context/Context.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"

namespace dale { namespace Form { namespace Literal { namespace Enum {
bool
parse(Generator *gen,
      llvm::BasicBlock *block,
      Node *n,
      Element::Enum *myenum,
      Element::Type *myenumtype,
      Element::Struct *myenumstructtype,
      bool getAddress,
      ParseResult *pr);
}}}}

#endif
