#ifndef DALE_FORM_PROCBODY
#define DALE_FORM_PROCBODY

#include "../../Generator/Generator.h"

namespace dale
{
namespace Form
{
namespace ProcBody
{
bool
parse(Generator *gen,
      Node *n,
      Element::Function *dfn,
      llvm::Function *fn,
      int skip,
      int is_anonymous,
      llvm::Value *return_value = NULL);
}
}
}

#endif
