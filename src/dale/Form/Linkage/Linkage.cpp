#include "Linkage.h"
#include "../../Linkage/Linkage.h"
#include "../../Error/Error.h"

namespace dale {
int
FormLinkageParse(Context *ctx, Node *n)
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

    if (!strcmp(lnk, "extern"))       {
        return Linkage::Extern;
    }
    else if (!strcmp(lnk, "intern"))       {
        return Linkage::Intern;
    }
    else if (!strcmp(lnk, "auto"))         {
        return Linkage::Auto;
    }
    else if (!strcmp(lnk, "extern-c"))     {
        return Linkage::Extern_C;
    }
    else if (!strcmp(lnk, "_extern-weak")) {
        return Linkage::Extern_Weak;
    }

    std::string temp;
    temp.append("'")
    .append(lnk)
    .append("'");

    Error *e = new Error(
        ErrorInst::Generator::UnexpectedElement,
        n,
        "'extern'/'intern'/'auto'/'extern-c'", "linkage",
        temp.c_str()
    );
    ctx->er->addError(e);
    return 0;
}
}
