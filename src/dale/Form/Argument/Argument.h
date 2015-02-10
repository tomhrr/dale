#ifndef DALE_FORM_ARGUMENT
#define DALE_FORM_ARGUMENT

#include "../../Generator/Generator.h"

namespace dale
{
bool FormArgumentParse(Generator *gen,
            Variable *var,
            Node *top,
            bool allow_anon_structs,
            bool allow_bitfields,
            bool allow_refs);
}

#endif
