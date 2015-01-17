#include "DerefStructDeref.h"

namespace dale {
Node *
FormMacroDerefStructDerefParse(Context *ctx, Node *n)
{
    assert(n->list && "parseDerefStructDeref must receive a list!");

    if (!ctx->er->assertArgNums("@:@", n, 2, 2)) {
        return NULL;
    }

    std::vector<Node*> *lst = n->list;

    // (@:@ array element) => (@ (: (@ array) element))

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("@");

    // Create a new nodelist, first element @, second element the
    // second element from the first list.

    std::vector<Node*> *newlst = new std::vector<Node *>;
    Token *de = new Token(TokenType::String,0,0,0,0);
    de->str_value.append("@");
    Node *nde = new Node(de);
    (*lst)[1]->copyMetaTo(nde);
    newlst->push_back(nde);
    newlst->push_back((*lst)[1]);

    // Create a new nodelist, first element :, second element
    // newlst, third element the third element from the first
    // list. Adjust the original list to suit.

    std::vector<Node*> *newlst2 = new std::vector<Node *>;
    Token *ad = new Token(TokenType::String,0,0,0,0);
    ad->str_value.append(":");
    Node *nad = new Node(ad);
    (*lst)[1]->copyMetaTo(nad);
    newlst2->push_back(nad);
    Node *nnewlst = new Node(newlst);
    (*lst)[1]->copyMetaTo(nnewlst);
    newlst2->push_back(nnewlst);
    newlst2->push_back((*lst)[2]);

    (*lst)[1] = new Node(newlst2);
    nnewlst->copyMetaTo((*lst)[1]);
    lst->pop_back();

    return n;
}
}
