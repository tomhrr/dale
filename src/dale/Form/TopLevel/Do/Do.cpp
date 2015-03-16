#include "Do.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale
{
bool
FormTopLevelDoParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx; 

    std::vector<Node *> *lst = node->list;

    if (lst->size() < 2) {
        Error *e = new Error(NoEmptyDo, node);
        ctx->er->addError(e);
        return false;
    }

    for (std::vector<Node *>::iterator b = (lst->begin() + 1),
                                       e = lst->end();
            b != e;
            ++b) {
        FormTopLevelInstParse(units, (*b));
        ctx->er->flush();
    }

    return true;
}
}
