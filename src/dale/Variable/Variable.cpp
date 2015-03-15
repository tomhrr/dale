#include "Variable.h"

namespace dale
{
Variable::Variable()
{
    has_initialiser = 0;
    index           = 0;
    serialise       = true;
    linkage         = Linkage::Auto;
}

Variable::Variable(const char *name, Type *type)
{
    this->name.append(name);
    this->type = type;

    has_initialiser = 0;
    index           = 0;
    linkage         = Linkage::Auto;
}

Variable::~Variable()
{
}

void
Variable::copyTo(Variable *other)
{
    other->name            = this->name;
    other->internal_name   = this->internal_name;
    other->value           = this->value;
    other->has_initialiser = this->has_initialiser;
    other->once_tag        = this->once_tag;
    other->index           = this->index;
    other->linkage         = this->linkage;
}

bool
Variable::isEqualTo(Variable *other)
{
    return type->isEqualTo(other->type);
}
}
