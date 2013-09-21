#ifndef DALE_UNITSTACK
#define DALE_UNITSTACK

#include "../Unit/Unit.h"
#include <stack>

namespace dale
{
class UnitStack
{
public:
    UnitStack(Unit *initial);
    ~UnitStack(void);

    Unit *top(void);
    bool empty(void);
    size_t size(void);
    void pop(void);
    void push(Unit *new_unit);

private:
    std::stack<Unit *> units;
};
}

#endif
