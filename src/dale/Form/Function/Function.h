#ifndef DALE_FORM_FUNCTION
#define DALE_FORM_FUNCTION

#include "../../Generator/Generator.h"

namespace dale
{
namespace Form
{
namespace Function
{
bool parse(Generator *gen,
           Node *n,
           const char *name,
           Element::Function **new_function,
           int override_linkage,
           int is_anonymous);
}
}
}

#endif
