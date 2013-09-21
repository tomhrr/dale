#ifndef DALE_COMMONDECL
#define DALE_COMMONDECL

#include "../Unit/Unit.h"

namespace dale
{
namespace CommonDecl
{
void addVarargsFunctions(Unit *unit);
void addBasicTypes(Unit *unit, bool is_x86_64);
void addVarargsTypes(Unit *unit, bool is_x86_64);
void addStandardVariables(Unit *unit);
}
}

#endif
