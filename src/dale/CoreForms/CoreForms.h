#ifndef DALE_COREFORMS
#define DALE_COREFORMS

namespace dale
{
/*! CoreForms

    Provides for checking whether a given binding is already in use as
    a core form, and for whether a given core form may be overridden.
    init must be called before exists or existsNoOverride is called.
*/
namespace CoreForms
{
/*! Initialise the core form functions.
 */
void init(void);
/*! Check whether a given binding is a core form.
 *  @param name The name of the binding.
 */
bool exists(const char *name);
/*! Check whether a given binding is a core form that may not be
 *  overridden.
 *  @param name The name of the binding.
 */
bool existsNoOverride(const char *name);
}
}

#endif
