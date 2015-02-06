#ifndef DALE_UNITSTACK
#define DALE_UNITSTACK

#include "../Unit/Unit.h"
#include <stack>

namespace dale
{
/*! UnitStack

    Whereas a unit is created for each new file that is parsed, a
    single unit stack is created for each new top-level file (i.e.
    those specified in the call to the compiler, and excluding any
    files parsed by way of include calls).  It facilitates the
    merging/linking of units as parsing starts and finishes for each.
*/
class UnitStack
{
private:
    /*! The stack of units. */
    std::stack<Unit *> units;

public:
    /*! Construct a new unit stack.
     *  @param initial The first unit.
     */
    UnitStack(Unit *initial);
    ~UnitStack(void);

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
