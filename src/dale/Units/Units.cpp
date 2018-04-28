#include "Units.h"
#include "../llvmUtils/llvmUtils.h"
#include "../llvm_Linker.h"
#include "../llvm_Module.h"

namespace dale {
Units::Units(Module::Reader *mr) {
    this->mr = mr;
    this->prefunction_ns = NULL;
}

Units::~Units() {}

Unit *Units::top() {
    if (!units.size()) {
        assert(false && "no unit available");
        abort();
    }
    return units.top();
}

bool Units::empty() { return units.empty(); }

size_t Units::size() { return units.size(); }

void Units::pop() {
    Unit *popped = units.top();
    units.pop();

    if (empty()) {
        return;
    }

    Unit *current = units.top();

    std::string link_error;

    if (popped->hasOwnModule()) {
        linkModule(current->linker, popped->module);
    }

    current->ctx->merge(popped->ctx);
    current->ctx->regetPointers(current->module);

    return;
}

void Units::push(Unit *new_unit) {
    if (units.size()) {
        Unit *current = units.top();
        new_unit->ctx->merge(current->ctx);
        new_unit->ee = current->ee;
    }
    units.push(new_unit);

    return;
}
}
