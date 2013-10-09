#include "Struct.h"
#include "../../../Generator/Generator.h"
#include "../../Struct/Struct.h"

namespace dale
{
namespace Form
{
namespace TopLevel
{
namespace Struct
{
bool 
parse(Generator *gen,
      Node *top)
{
    symlist *lst = top->list;
    Node *name = gen->parseOptionalMacroCall(lst->at(1));
    Node *n = lst->at(2);

    return Form::Struct::parse(gen, n, name->token->str_value.c_str());
}
}
}
}
}
