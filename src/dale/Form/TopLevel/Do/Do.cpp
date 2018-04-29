#include "Do.h"

#include <vector>

#include "../../../Node/Node.h"
#include "../../../Units/Units.h"
#include "../Inst/Inst.h"
#include "Config.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormTopLevelDoParse(Units *units, Node *node) {
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;

    for (std::vector<Node *>::iterator b = (lst->begin() + 1),
                                       e = lst->end();
         b != e; ++b) {
        FormTopLevelInstParse(units, (*b));
        ctx->er->flush();
    }

    return true;
}
}
