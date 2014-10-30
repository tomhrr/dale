#include "UnitStack.h"
#include "../llvm_Module.h"
#include "llvm/Linker.h"

namespace dale
{
UnitStack::UnitStack(Unit *initial)
{
    units.push(initial);
}

UnitStack::~UnitStack(void)
{
}

Unit* 
UnitStack::top(void)
{
    return units.top();
}

bool
UnitStack::empty(void)
{
    return units.empty();
}

size_t
UnitStack::size(void)
{
    return units.size();
}

void
UnitStack::pop(void)
{
    Unit *popped = units.top();
    units.pop();

    if (empty()) {
        return;
    }

    Unit *current = units.top();

    std::string link_error;

#if D_LLVM_VERSION_MINOR <= 2
    if (current->linker->LinkInModule(popped->module, &link_error)) {
#else
    if (current->linker->linkInModule(popped->module, &link_error)) {
#endif
        fprintf(stderr,
                "Internal error: cannot link modules: "
                "%s\n", link_error.c_str());
        abort();
    }

    current->ctx->merge(popped->ctx);
    current->ctx->regetPointers(current->module);

    return;
}

void
UnitStack::push(Unit *new_unit)
{
    Unit *current = units.top();
    new_unit->ctx->merge(current->ctx);
    new_unit->ee = current->ee;
    units.push(new_unit);

    return;
}
}
