#include "Setv.h"

namespace dale {
Node *
FormMacroSetvParse(Context *ctx, Node *n)
{
    assert(n->list && "parseSetv must receive a list!");

    if (!ctx->er->assertArgNums("setv", n, 2, 2)) {
        return NULL;
    }

    std::vector<Node*> *lst = n->list;
    Node *varn = (*lst)[1];

    if (!ctx->er->assertArgIsAtom("setv", varn, "1")) {
        return NULL;
    }

    // (setv a 10) => (setf (# a) 10)

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append("setf");

    // Create a new nodelist, first element #, second element
    // whatever the current second element is, make it the second
    // element of the current list.

    std::vector<Node*> *newlst = new std::vector<Node *>;
    Token *addrof = new Token(TokenType::String);
    addrof->str_value.append("#");
    newlst->push_back(new Node(addrof));
    newlst->push_back((*lst)[1]);
    (*lst)[1] = new Node(newlst);

    return n;
}
}
