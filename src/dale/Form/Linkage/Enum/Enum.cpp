#include "Enum.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"

using namespace dale::ErrorInst;

namespace dale {
int
FormLinkageEnumParse(Context *ctx, Node *node)
{
    if (!node->is_token) {
        Error *e = new Error(UnexpectedElement, node,
                             "atom", "linkage", "list");
        ctx->er->addError(e);
        return 0;
    }

    if (node->token->type != TokenType::String) {
        Error *e = new Error(UnexpectedElement, node,
                             "symbol", "linkage",
                             node->token->tokenType());
        ctx->er->addError(e);
        return 0;
    }

    const char *linkage = node->token->str_value.c_str();

    if (!strcmp(linkage, "extern")) {
        return EnumLinkage::Extern;
    } else if (!strcmp(linkage, "intern")) {
        return EnumLinkage::Intern;
    }

    Error *e = new Error(UnexpectedElement, node,
                         "extern/intern/opaque", "linkage",
                         linkage);
    ctx->er->addError(e);
    return 0; 
}
}
