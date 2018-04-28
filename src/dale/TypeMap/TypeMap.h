#ifndef DALE_ELEMENT_TYPEMAP
#define DALE_ELEMENT_TYPEMAP

#include <map>
#include <string>

namespace dale {
/*! TypeMap

    Provides functions for adding entries to and retrieving entries
    from a simple string-to-string type map.  This is primarily so
    that error messages involving constructed types can refer to the
    form that evaluates to that underlying type name, rather than the
    type itself (e.g. '(Vector int)' instead of 'vectori').
*/

/*! A map from underlying type name to the string that should be used
 *  for type display. */
extern std::map<std::string, std::string> dale_typemap;

/*! Add a type map entry.
 *  @param from The underlying type name.
 *  @param to The string that should be used for type display.
 */
bool addTypeMapEntry(const char *from, const char *to);
/*! Retrieve a type map entry.
 *  @param from The underlying type name.
 *  @param to A buffer to which the type display string will be written.
 *
 *  If the type does not exist in the map, this returns false.
 */
bool getTypeMapEntry(const char *from, std::string *to);
}

#endif
