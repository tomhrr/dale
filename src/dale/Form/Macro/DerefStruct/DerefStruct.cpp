#include "DerefStruct.h"

namespace dale {
Node *
FormMacroDerefStructParse(Context *ctx, Node *n)
{
    assert(n->list && "parseDerefStruct must receive a list!");

    if (!ctx->er->assertArgNums(":@", n, 2, 2)) {
        return NULL;
    }

    std::vector<Node*> *lst = n->list;

    // (:@ array element) => (: (@ array) element)

    // Change the first token.
    (*lst)[0]->token->str_value.clear();
    (*lst)[0]->token->str_value.append(":");

    // Create a new nodelist, first element @ and second element
    // as per the original second element. Remove that element
    // from the provided list, and make the second element of the
    // provided list this new list.

    std::vector<Node*> *newlst = new std::vector<Node *>;
    Token *ad = new Token(TokenType::String);
    ad->str_value.append("@");
    newlst->push_back(new Node(ad));
    newlst->push_back((*lst)[1]);
    (*lst)[1] = new Node(newlst);

    return n;
}
}
