#include "TokenType.h"

namespace dale
{
const char *
tokenTypeToString(int token_type)
{
    const char *ret;
    switch (token_type) {
        case TokenType::Null:          ret = "null";                  break;
        case TokenType::LeftParen:     ret = "left parenthesis";      break;
        case TokenType::RightParen:    ret = "right parenthesis";     break;
        case TokenType::String:        ret = "symbol";                break;
        case TokenType::Int:           ret = "integer";               break;
        case TokenType::FloatingPoint: ret = "floating point number"; break;
        case TokenType::StringLiteral: ret = "string literal";        break;
        case TokenType::Eof:           ret = "EOF";                   break;
        default:                       ret = "[unknown]";
    }
    return ret;
}
}
