#include "Integer.h"

#include <cstdlib>

#include "../../../Utils/Utils.h"

#define DECIMAL_RADIX 10

namespace dale
{
int
FormLiteralIntegerParse(Node *node, ErrorReporter *er)
{
    if (!node->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            node,
            "symbol", "integer", "list"
        );
        er->addError(e);
        return -1;
    }
    if (node->token->type != TokenType::Int) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement,
            node,
            "integer", "literal", node->token->tokenType()
        );
        er->addError(e);
        return -1;
    }

    char *endptr;
    unsigned long addnum =
        strtoul(node->token->str_value.c_str(), &endptr, DECIMAL_RADIX);

    if (STRTOUL_FAILED(addnum, node->token->str_value.c_str(), endptr)) {
        Error *e = new Error(
            ErrorInst::Generator::UnableToParseInteger,
            node,
            node->token->str_value.c_str()
        );
        er->addError(e);
        return -1;
    }

    return addnum;
}
}
