#include "Variable.h"

namespace dale {
Variable::Variable() {
    has_initialiser = 0;
    index = 0;
    serialise = true;
    linkage = Linkage::Auto;
    type = NULL;
    value = NULL;
    fn = NULL;
}

Variable::Variable(const char *name, Type *type) {
    this->name.append(name);
    this->type = type;

    has_initialiser = 0;
    index = 0;
    serialise = true;
    linkage = Linkage::Auto;
    value = NULL;
    fn = NULL;
}

Variable::~Variable() {}

void Variable::copyTo(Variable *other) {
    other->name = this->name;
    other->symbol = this->symbol;
    other->value = this->value;
    other->has_initialiser = this->has_initialiser;
    other->once_tag = this->once_tag;
    other->index = this->index;
    other->linkage = this->linkage;
    other->fn = this->fn;
}

bool Variable::isEqualTo(Variable *other) {
    return type->isEqualTo(other->type);
}
}
