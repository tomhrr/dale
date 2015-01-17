#include "Struct.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"

namespace dale {
int 
FormLinkageStructParse(Context *ctx, Node *n)
{
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "atom", "linkage", "list"
        );
        ctx->er->addError(e);
        return 0;
    }

    if (n->token->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            n,
            "symbol", "linkage", n->token->tokenType()
        );
        ctx->er->addError(e);
        return 0;
    }

    const char *lnk = n->token->str_value.c_str();

    if (!strcmp(lnk, "extern")) {
        return StructLinkage::Extern;
    } else if (!strcmp(lnk, "intern")) {
        return StructLinkage::Intern;
    } else if (!strcmp(lnk, "opaque")) {
        return StructLinkage::Opaque;
    }

    std::string temp;
    temp.append("'")
    .append(lnk)
    .append("'");

    Error *e = new Error(
        ErrorInst::Generator::UnexpectedElement,
        n,
        "'extern'/'intern'/'opaque'", "linkage",
        temp.c_str()
    );
    ctx->er->addError(e);
    return 0;
}
}
