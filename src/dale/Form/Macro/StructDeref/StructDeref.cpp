#include "StructDeref.h"

namespace dale { namespace Form { namespace Macro { namespace StructDeref {
Node *
parse(Context *ctx, Node *n)
{
    assert(n->list && "parseStructDeref must receive a list!");

    if (!ctx->er->assertArgNums("@:", n, 2, 2)) {
        return NULL;
    }

    std::vector<Node*> *lst = n->list;

    // (@: array element) => (@ (: array element))

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("@");

    // Create a new nodelist, first element :, second element and
    // third element from the current list. Remove the third
    // element from the provided list, make the second element of
    // the provided list this new list.

    std::vector<Node*> *newlst = new std::vector<Node *>;
    Token *ad = new Token(TokenType::String,0,0,0,0);
    ad->str_value.append(":");
    newlst->push_back(new Node(ad));
    newlst->push_back((*lst)[1]);
    newlst->push_back((*lst)[2]);
    (*lst)[1] = new Node(newlst);
    lst->pop_back();

    return n;
}
}}}}
