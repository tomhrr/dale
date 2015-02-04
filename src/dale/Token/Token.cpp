#include "Token.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace dale
{
Token::Token(int new_type,
             int begin_line_number,
             int begin_column_number,
             int end_line_number,
             int end_column_number)
{
    begin.setLineAndColumn(begin_line_number, begin_column_number);
    end.setLineAndColumn(end_line_number, end_column_number);
    type  = new_type;
}

Token::Token(Token *token)
{
    token->copyTo(this);
}

Token::~Token(void)
{
}

void Token::toString(std::string *str)
{
    char buf[100];

    sprintf(buf,
            "%s: (%d %d) (%d %d)",
            tokenType(),
            begin.getLineNumber(),
            begin.getColumnNumber(),
            end.getLineNumber(),
            end.getColumnNumber());

    str->append(buf);
}

void Token::valueToString(std::string *str)
{
    if (type == TokenType::String) {
        str->append(str_value.c_str());
    } else if (type == TokenType::StringLiteral) {
        str->append("\"");
        str->append(str_value.c_str());
        str->append("\"");
    } else if (type == TokenType::Int) {
        str->append(str_value.c_str());
    } else if (type == TokenType::FloatingPoint) {
        str->append(str_value.c_str());
    } else {
        fprintf(stderr,
                "Internal error: token (type %d) has no value.\n",
                type);
        abort();
    }
}

void Token::copyTo(Token *token)
{
    token->type = type;
    begin.copyTo(&(token->begin));
    end.copyTo(&(token->end));

    token->str_value.clear();
    token->str_value.append(str_value.c_str());
}

const char *Token::tokenType(void)
{
    return (type == TokenType::Null)          ? "null"
           : (type == TokenType::LeftParen)     ? "left parenthesis"
           : (type == TokenType::RightParen)    ? "right parenthesis"
           : (type == TokenType::String)        ? "symbol"
           : (type == TokenType::Int)           ? "integer"
           : (type == TokenType::FloatingPoint) ? "floating point number"
           : (type == TokenType::StringLiteral) ? "string literal"
           : (type == TokenType::Eof)           ? "EOF"
           : "UNKNOWN";
}
}
