#include "Struct.h"
#include "../../../Units/Units.h"
#include "../../Struct/Struct.h"

namespace dale
{
bool 
FormTopLevelStructParse(Units *units,
      Node *top)
{
    std::vector<Node *> *lst = top->list;
    Node *name = units->top()->mp->parsePotentialMacroCall(lst->at(1));
    Node *n = lst->at(2);

    return FormStructParse(units, n, name->token->str_value.c_str());
}
}
