#include "Variable.h"

namespace dale
{
namespace Element
{
Variable::Variable()
{
    name            = new std::string;
    internal_name   = new std::string;
    has_initialiser = 0;
    index           = 0;
    serialise       = true;
}

Variable::Variable(char *new_name, Element::Type *new_type)
{
    name = new std::string;
    name->append(new_name);
    type = new_type;
    internal_name = new std::string;
    has_initialiser = 0;
    index           = 0;
}

Variable::~Variable()
{
    delete name;
    delete internal_name;
    //delete type;
}

bool Variable::isEqualTo(Element::Variable *other_var)
{
    return type->isEqualTo(other_var->type);
}
}
}
