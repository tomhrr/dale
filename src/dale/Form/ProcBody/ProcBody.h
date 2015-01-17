#ifndef DALE_FORM_PROCBODY
#define DALE_FORM_PROCBODY

#include "../../Generator/Generator.h"

namespace dale
{
bool
FormProcBodyParse(Generator *gen,
      Node *n,
      Function *dfn,
      llvm::Function *fn,
      int skip,
      int is_anonymous,
      llvm::Value *return_value = NULL);
}

#endif
