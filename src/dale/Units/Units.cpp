#include "Units.h"

#include <string>

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
    return units.back();
}

bool Units::empty() { return units.empty(); }

size_t Units::size() { return units.size(); }

void Units::pop() {
    Unit *popped = units.back();
    units.pop_back();

    if (empty()) {
        return;
    }

    Unit *current = units.back();

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
        Unit *current = units.back();
        new_unit->ctx->merge(current->ctx);
        new_unit->ee = current->ee;
    }
    units.push_back(new_unit);

    return;
}
}
