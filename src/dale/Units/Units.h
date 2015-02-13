#ifndef DALE_UNITS
#define DALE_UNITS

#include "../Unit/Unit.h"
#include "../Module/Reader/Reader.h"
#include "../Namespace/Namespace.h"
#include <stack>

namespace dale
{
/*! Units

    Whereas a unit is created for each new file that is parsed, a
    single Units object is created at the beginning of the compilation
    process for referring to the individual units, as well as some
    utility classes that remain constant throughout.  Units is the
    highest-level "context" object.
*/
class Units
{
private:
    /*! The stack of units. */
    std::stack<Unit *> units;

public:
    /*! The module reader. */
    Module::Reader *mr;
    /*! The namespace immediately before the first namespace within
     *  the function currently being processed. */
    Namespace *prefunction_ns;

    /*! Construct a new Units object.
     *  @param mr A module reader.
     */
    Units(Module::Reader *mr);
    ~Units(void);

    /*! Get the unit at the top of the stack.
     */
    Unit *top(void);
    /*! Check whether the unit stack is empty.
     */
    bool empty(void);
    /*! Get the size of the stack.
     */
    size_t size(void);
    /*! Pop the top unit from the stack, merging the top unit's
     *  context into the next unit's context, and linking the top
     *  unit's module into the next unit's module.
     */
    void pop(void);
    /*! Push another unit onto the stack.  The context from the
     *  topmost unit is merged into the new unit's context.
     */
    void push(Unit *new_unit);
};
}

#endif
