#include "DerefStructDeref.h"

namespace dale {
Node *
FormMacroDerefStructDerefParse(Context *ctx, Node *n)
{
    if (!ctx->er->assertArgNums("@:@", n, 2, 2)) {
        return NULL;
    }

    std::vector<Node*> *lst = n->list;

    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("@");

    std::vector<Node*> *new_lst_inner = new std::vector<Node *>;
    Node *deref_node = new Node("@");
    (*lst)[1]->copyMetaTo(deref_node);
    new_lst_inner->push_back(deref_node);
    new_lst_inner->push_back((*lst)[1]);

    Node *new_lst_inner_node = new Node(new_lst_inner);
    (*lst)[1]->copyMetaTo(new_lst_inner_node);

    std::vector<Node*> *new_lst_sm = new std::vector<Node *>;
    Node *struct_mem_node = new Node(":");
    (*lst)[1]->copyMetaTo(struct_mem_node);
    new_lst_sm->push_back(struct_mem_node);
    new_lst_sm->push_back(new_lst_inner_node);
    new_lst_sm->push_back((*lst)[2]);

    (*lst)[1] = new Node(new_lst_sm);
    lst->pop_back();

    return n;
}
}
