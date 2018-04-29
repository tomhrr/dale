#include "Token.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace dale {
Token::Token(int new_type, int begin_line_number,
             int begin_column_number, int end_line_number,
             int end_column_number) {
    begin.setLineAndColumn(begin_line_number, begin_column_number);
    end.setLineAndColumn(end_line_number, end_column_number);
    type = new_type;
}

Token::Token(Token *other) { other->copyTo(this); }

Token::~Token() {}

void Token::toString(std::string *str) {
    if (type == TokenType::String || type == TokenType::Int ||
        type == TokenType::FloatingPoint) {
        str->append(str_value.c_str());
    } else if (type == TokenType::StringLiteral) {
        str->append("\"");
        str->append(str_value.c_str());
        str->append("\"");
    } else {
        assert(false);
    }
}

void Token::copyTo(Token *token) {
    token->type = type;
    begin.copyTo(&(token->begin));
    end.copyTo(&(token->end));

    token->str_value.clear();
    token->str_value.append(str_value.c_str());
}

const char *Token::tokenType() { return tokenTypeToString(type); }
}
