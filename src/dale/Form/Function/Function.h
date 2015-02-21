#ifndef DALE_FORM_FUNCTION
#define DALE_FORM_FUNCTION

#include "../../Units/Units.h"

namespace dale
{
bool FormFunctionParse(Units *units,
           Node *n,
           const char *name,
           Function **new_function,
           int override_linkage,
           int is_anonymous);
}

#endif
