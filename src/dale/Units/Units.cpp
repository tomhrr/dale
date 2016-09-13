#include "Units.h"
#include "../llvm_Module.h"
#include "../llvm_Linker.h"

namespace dale
{
Units::Units(Module::Reader *mr)
{
    this->mr = mr;
    this->prefunction_ns = NULL;
}

Units::~Units()
{
}

Unit*
Units::top()
{
    return units.top();
}

bool
Units::empty()
{
    return units.empty();
}

size_t
Units::size()
{
    return units.size();
}

void
Units::pop()
{
    Unit *popped = units.top();
    units.pop();

    if (empty()) {
        return;
    }

    Unit *current = units.top();

    std::string link_error;

    if (popped->hasOwnModule()) {
#if D_LLVM_VERSION_MINOR <= 2
        bool res = current->linker->LinkInModule(popped->module, &link_error);
#else
        bool res = current->linker->linkInModule(popped->module, &link_error);
#endif
        assert(!res && "unable to link modules");
        _unused(res);
    }

    current->ctx->merge(popped->ctx);
    current->ctx->regetPointers(current->module);

    return;
}

void
Units::push(Unit *new_unit)
{
    if (units.size()) {
        Unit *current = units.top();
        new_unit->ctx->merge(current->ctx);
        new_unit->ee = current->ee;
    }
    units.push(new_unit);

    return;
}
}
