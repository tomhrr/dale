#include "Linkage.h"
#include "../../Error/Error.h"
#include "../../Linkage/Linkage.h"

using namespace dale::ErrorInst;

namespace dale {
int FormLinkageParse(Context *ctx, Node *node) {
    if (!node->is_token) {
        Error *e = new Error(UnexpectedElement, node, "atom", "linkage",
                             "list");
        ctx->er->addError(e);
        return 0;
    }

    if (node->token->type != TokenType::String) {
        Error *e = new Error(UnexpectedElement, node, "symbol",
                             "linkage", node->token->tokenType());
        ctx->er->addError(e);
        return 0;
    }

    const char *linkage = node->token->str_value.c_str();

    if (!strcmp(linkage, "extern")) {
        return Linkage::Extern;
    } else if (!strcmp(linkage, "intern")) {
        return Linkage::Intern;
    } else if (!strcmp(linkage, "auto")) {
        return Linkage::Auto;
    } else if (!strcmp(linkage, "extern-c")) {
        return Linkage::Extern_C;
    } else if (!strcmp(linkage, "_extern-weak")) {
        return Linkage::Extern_Weak;
    }

    Error *e =
        new Error(UnexpectedElement, node,
                  "extern/intern/auto/extern-c", "linkage", linkage);
    ctx->er->addError(e);
    return 0;
}
}
