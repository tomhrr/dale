#ifndef DALE_TOKEN
#define DALE_TOKEN

#include <string>

#include "../Position/Position.h"
#include "../TokenType/TokenType.h"

namespace dale
{
class Token
{
public:
    int type;
    std::string str_value;
    Position begin;
    Position end;

    Token(int new_type,
          int begin_line_number = 0,
          int begin_column_number = 0,
          int end_line_number = 0,
          int end_column_number = 0);
    Token(Token *token);
    ~Token();
    void toString(std::string *str);
    void valueToString(std::string *str);
    void copyTo(Token *token);
    const char *tokenType(void);
};
}

#endif
