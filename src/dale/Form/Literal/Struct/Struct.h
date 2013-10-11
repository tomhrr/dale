#ifndef DALE_FORM_LITERAL_STRUCT
#define DALE_FORM_LITERAL_STRUCT

#include "../../../Context/Context.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"

namespace dale { namespace Form { namespace Literal { namespace Struct {
bool
parse(Generator *gen,
      Element::Function *dfn,
      llvm::BasicBlock *block,
      Node *n,
      const char *struct_name,
      Element::Struct *str,
      Element::Type *structtype,
      bool get_address,
      ParseResult *pr);
}}}}

#endif
