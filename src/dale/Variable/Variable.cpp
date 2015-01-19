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

bool
Variable::isEqualTo(Variable *other_var)
{
    return type->isEqualTo(other_var->type);
}
}
