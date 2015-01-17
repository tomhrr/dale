#ifndef DALE_FORM_FUNCTION
#define DALE_FORM_FUNCTION

#include "../../Generator/Generator.h"

namespace dale
{
bool FormFunctionParse(Generator *gen,
           Node *n,
           const char *name,
           dale::Function **new_function,
           int override_linkage,
           int is_anonymous);
}

#endif
