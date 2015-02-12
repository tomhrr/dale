#include "Units.h"
#include "../llvm_Module.h"
#include "../llvm_Linker.h"

namespace dale
{
Units::Units(Module::Reader *mr)
{
    this->mr = mr;
}

Units::~Units(void)
{
}

Unit* 
Units::top(void)
{
    return units.top();
}

bool
Units::empty(void)
{
    return units.empty();
}

size_t
Units::size(void)
{
    return units.size();
}

void
Units::pop(void)
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
