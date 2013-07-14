#include "Variable.h"

namespace dale
{
namespace Element
{
Variable::Variable()
{
    has_initialiser = 0;
    index           = 0;
    serialise       = true;
    linkage         = Linkage::Auto;
}

Variable::Variable(char *new_name, Element::Type *new_type)
{
    name.append(new_name);
    type = new_type;
    has_initialiser = 0;
    index           = 0;
    linkage         = Linkage::Auto;
}

Variable::~Variable()
{
}

bool Variable::isEqualTo(Element::Variable *other_var)
{
    return type->isEqualTo(other_var->type);
}
}
}
