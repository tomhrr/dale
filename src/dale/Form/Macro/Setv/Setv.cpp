#include "Setv.h"

namespace dale {
Node *
FormMacroSetvParse(Context *ctx, Node *n)
{
    if (!ctx->er->assertArgNums("setv", n, 2, 2)) {
        return NULL;
    }

    std::vector<Node*> *lst = n->list;

    if (!ctx->er->assertArgIsAtom("setv", (*lst)[1], "1")) {
        return NULL;
    }

    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("setf");

    std::vector<Node*> *new_lst = new std::vector<Node *>;
    new_lst->push_back(new Node("#"));
    new_lst->push_back((*lst)[1]);

    (*lst)[1] = new Node(new_lst);

    return n;
}
}
