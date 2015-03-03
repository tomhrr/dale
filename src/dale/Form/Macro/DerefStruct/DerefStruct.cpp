#include "DerefStruct.h"

namespace dale {
Node *
FormMacroDerefStructParse(Context *ctx, Node *n)
{
    if (!ctx->er->assertArgNums(":@", n, 2, 2)) {
        return NULL;
    }

    std::vector<Node*> *lst = n->list;

    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append(":");

    std::vector<Node*> *new_lst = new std::vector<Node *>;
    new_lst->push_back(new Node("@"));
    new_lst->push_back((*lst)[1]);

    (*lst)[1] = new Node(new_lst);

    return n;
}
}
