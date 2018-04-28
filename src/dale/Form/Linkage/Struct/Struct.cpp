#include "Struct.h"
#include "../../../Error/Error.h"
#include "../../../Linkage/Linkage.h"

using namespace dale::ErrorInst;

namespace dale {
int FormLinkageStructParse(Context *ctx, Node *node) {
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
        return StructLinkage::Extern;
    } else if (!strcmp(linkage, "intern")) {
        return StructLinkage::Intern;
    } else if (!strcmp(linkage, "opaque")) {
        return StructLinkage::Opaque;
    }

    Error *e = new Error(UnexpectedElement, node,
                         "extern/intern/opaque", "linkage", linkage);
    ctx->er->addError(e);
    return 0;
}
}
