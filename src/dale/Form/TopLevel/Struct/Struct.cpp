#include "Struct.h"
#include "../../../Generator/Generator.h"
#include "../../Struct/Struct.h"

namespace dale
{
bool 
FormTopLevelStructParse(Generator *gen,
      Node *top)
{
    symlist *lst = top->list;
    Node *name = gen->getUnit()->mp->parseOptionalMacroCall(lst->at(1));
    Node *n = lst->at(2);

    return FormStructParse(gen, n, name->token->str_value.c_str());
}
}
