#ifndef DALE_FORM_PROCBODY
#define DALE_FORM_PROCBODY

#include "../../Units/Units.h"

namespace dale
{
bool
FormProcBodyParse(Units *units,
      Node *n,
      Function *dfn,
      llvm::Function *fn,
      int skip,
      int is_anonymous,
      llvm::Value *return_value = NULL);
}

#endif
