#ifndef DALE_TOKEN
#define DALE_TOKEN

#include <string>

#include "../Position/Position.h"

namespace dale
{
namespace TokenType
{
enum
{
    Null,
    LeftParen,
    RightParen,
    String,
    StringLiteral,
    Int,
    FloatingPoint,
    Eof
};
}

class Token
{
    /* these should be protected for the most part */
public:
    int          type;
    std::string  str_value;
    int          int_value;
    Position    begin;
    Position    end;

    Token(int new_type,
          int begin_line_number,
          int begin_column_number,
          int end_line_number,
          int end_column_number);
    Token(Token *token);
    ~Token();
    /* Should probably add a constructor that takes two point
     * arguments, instead of taking the numbers and allocating
     * the object - if you add a token on the stack it would
     * be a bit unintuitive (I think?) to have it run off and
     * make dynamic allocations */
    void toString(std::string *str);
    void valueToString(std::string *str);
    void copyTo(Token *token);
    const char *tokenType(void);
};
}

#endif
