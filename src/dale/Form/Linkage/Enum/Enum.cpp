#include "Enum.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"

namespace dale { namespace Form { namespace Linkage { namespace Enum {
int parse(Context *ctx,
          Node *n)
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
        return EnumLinkage::Extern;
    } else if (!strcmp(lnk, "intern")) {
        return EnumLinkage::Intern;
    }

    std::string element;
    element.append("'").append(lnk).append("'");

    Error *e = new Error(
        ErrorInst::Generator::UnexpectedElement,
        n,
        "'extern'/'intern'/'opaque'", "linkage",
        element.c_str()
    );
    ctx->er->addError(e);
    return 0; 
}
}}}}
