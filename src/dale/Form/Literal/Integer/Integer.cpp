#include "Integer.h"

#include <cstdlib>

#include "../../../Utils/Utils.h"

#define DECIMAL_RADIX 10

using namespace dale::ErrorInst;

namespace dale {
int FormLiteralIntegerParse(Node *node, ErrorReporter *er) {
    if (!node->is_token) {
        Error *e = new Error(UnexpectedElement, node, "symbol",
                             "integer", "list");
        er->addError(e);
        return -1;
    }
    if (node->token->type != TokenType::Int) {
        Error *e = new Error(UnexpectedElement, node, "integer",
                             "literal", node->token->tokenType());
        er->addError(e);
        return -1;
    }

    const char *num_str = node->token->str_value.c_str();
    char *end_ptr;
    unsigned long num = strtoul(num_str, &end_ptr, DECIMAL_RADIX);

    if (STRTOUL_FAILED(num, num_str, end_ptr)) {
        Error *e = new Error(UnableToParseInteger, node,
                             node->token->str_value.c_str());
        er->addError(e);
        return -1;
    }

    return num;
}
}
