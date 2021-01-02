#ifndef DALE_UNITS
#define DALE_UNITS

#include <stack>
#include <string>

#include "../Module/Reader/Reader.h"
#include "../Namespace/Namespace.h"
#include "../Unit/Unit.h"

namespace dale {
/*! Units

    Whereas a unit is created for each new file that is parsed, a
    single Units object is created at the beginning of the compilation
    process for referring to the individual units, as well as some
    utility classes that remain constant throughout.  Units is the
    highest-level "context" object.
*/
class Units {
    public:
    /*! The stack of units. */
    std::vector<Unit *> units;
    /*! The module reader. */
    Module::Reader *mr;
    /*! The namespace immediately before the first namespace within
     *  the function currently being processed. */
    Namespace *prefunction_ns;
    /*! The current module name. */
    std::string module_name;
    /*! Whether the module being compiled is a compile-time-only module.
     */
    bool cto;
    /*! Whether common declarations should be added to each new unit. */
    bool no_common;
    /*! Whether the standard library (libdrt) should be imported into
     *  each new unit. */
    bool no_drt;
    /*! Whether macro expansions should be printed to standard output.
     */
    bool print_expansions;
    /*! Whether debug mode is enabled. */
    bool debug;

    /*! Construct a new Units object.
     *  @param mr A module reader.
     */
    explicit Units(Module::Reader *mr);
    ~Units();

    /*! Get the unit at the top of the stack.
     */
    Unit *top();
    /*! Check whether the unit stack is empty.
     */
    bool empty();
    /*! Get the size of the stack.
     */
    size_t size();
    /*! Pop the top unit from the stack, merging the top unit's
     *  context into the next unit's context, and linking the top
     *  unit's module into the next unit's module.
     */
    void pop();
    /*! Push another unit onto the stack.  The context from the
     *  topmost unit is merged into the new unit's context.
     */
    void push(Unit *new_unit);
};
}

#endif
