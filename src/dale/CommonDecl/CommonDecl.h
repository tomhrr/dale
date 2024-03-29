#ifndef DALE_COMMONDECL
#define DALE_COMMONDECL

#include "../Unit/Unit.h"

namespace dale {
/*! CommonDecl

    Provides functions for adding the common declarations/definitions
    to a unit.
*/
namespace CommonDecl {
/*! Add the intrinsic va_start and va_end functions to the unit.
 *  @param unit The unit.
 */
void addVarargsFunctions(Unit *unit);
/*! Add the basic type functions to the unit.
 *  @param unit The unit.
 *  @param arch The architecture.
 *
 *  See BasicTypes.
 */
void addBasicTypes(Unit *unit, int arch);
/*! Add the va-list type to the unit.
 *  @param unit The unit.
 *  @param arch The architecture.
 */
void addVarargsTypes(Unit *unit, int arch);
/*! Add the standard variables to the unit.
 *  @param unit The unit.
 *
 *  These are the global variables provided by the standard C library.
 *  The variables added here are documented in 'Form reference'.
 */
void addStandardVariables(Unit *unit);
}
}

#endif
