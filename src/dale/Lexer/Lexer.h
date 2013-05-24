#ifndef DALE_LEXER
#define DALE_LEXER

#include <vector>

#include "../Utils/Utils.h"
#include "../Error/Error.h"
#include "../Token/Token.h"

namespace dale
{
class Lexer
{
private:
    FILE                    *file;
    Position                *current;
    std::vector<Token *>    *ungot_tokens;
    char lexbuf[8193];
    int  lexcnt;
    int  chari;
    int  been_pushed;
    int  reset_pos;
public:
    Lexer(FILE *new_file,
          std::vector<Token *> *ungot_tokens,
          int line_number,
          int column_number);
    int   getNextToken(Token *token, Error *error);
    void  ungetToken(Token *token);
    int myfgetc();
    int myungetc(char c);
    int pushText(const char *str);

    ino_t currentInode(void);
    ~Lexer();
};
}

#endif
