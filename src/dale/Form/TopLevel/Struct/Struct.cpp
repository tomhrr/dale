#include "Struct.h"
#include "../../../Units/Units.h"
#include "../../Struct/Struct.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool 
FormTopLevelStructParse(Units *units, Node *top)
{
    std::vector<Node *> *lst = top->list;
    if (lst->size() < 3) {
        Error *e = new Error(IncorrectMinimumNumberOfArgs, top,
                             "struct", 2, (int) (lst->size() - 1));
        units->top()->ctx->er->addError(e);
        return false;
    }
    Node *name = units->top()->mp->parsePotentialMacroCall((*lst)[1]);
    return FormStructParse(units, (*lst)[2], name->token->str_value.c_str());
}
}
