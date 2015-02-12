#include "Do.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../Inst/Inst.h"

namespace dale
{
bool
FormTopLevelDoParse(Generator *gen, Node *node)
{
    Context *ctx = gen->units->top()->ctx; 

    std::vector<Node *> *lst = node->list;

    if (lst->size() < 2) {
        Error *e = new Error(
            ErrorInst::Generator::NoEmptyDo,
            node
        );
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();

    ++node_iter;

    while (node_iter != lst->end()) {
        FormTopLevelInstParse(gen, (*node_iter));
        ctx->er->flush();
        ++node_iter;
    }

    return true;
}
}
