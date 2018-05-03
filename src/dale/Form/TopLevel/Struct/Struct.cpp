#include "Struct.h"

#include <vector>

#include "../../../Units/Units.h"
#include "../../Struct/Struct.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormTopLevelStructParse(Units *units, Node *top,
                             const char *name) {
    std::vector<Node *> *lst = top->list;
    if (!name) {
        if (lst->size() < 3) {
            Error *e =
                new Error(IncorrectMinimumNumberOfArgs, top, "struct",
                          2, static_cast<int>(lst->size() - 1));
            units->top()->ctx->er->addError(e);
            return false;
        }
        Node *name_node =
            units->top()->mp->parsePotentialMacroCall((*lst)[1]);
        if (!name_node) {
            return false;
        }
        name = name_node->token->str_value.c_str();
        top = (*lst)[2];
    }
    return FormStructParse(units, top, name);
}
}
