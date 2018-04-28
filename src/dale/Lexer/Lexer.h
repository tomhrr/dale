#ifndef DALE_LEXER
#define DALE_LEXER

#include <vector>

#include "../Error/Error.h"
#include "../Token/Token.h"
#include "../Utils/Utils.h"

namespace dale {
/*! Lexer

    The lexer class.  A new lexer should be created for each file: see
    Unit.
*/
class Lexer {
    private:
    /*! The file pointer for the current file. */
    FILE *file;
    /*! The current position. */
    Position current;
    /*! A stack of "ungot" tokens.  See ungetToken. */
    std::vector<Token *> ungot_tokens;
    /*! The file buffer. */
    char buf[8193];
    /*! The number of bytes remaining to be processed from buf. */
    int count;
    /*! The index of the next byte to be processed from buf. */
    int index;
    /*! Whether buf was last populated by pushText. */
    bool been_pushed;
    /*! Whether the current position needs to be reset. */
    bool reset_position;
    /*! Whether to process input line-by-line. */
    bool line_buffered;

    /*! Get the next character. */
    int getchar_();
    /*! Unget a character. */
    void ungetchar_(char c);

    public:
    /*! Construct a new lexer.
     *  @param file The file to read.
     *  @param line_buffered Whether to process input line-by-line.
     */
    explicit Lexer(FILE *file, bool line_buffered = false);
    ~Lexer();
    /*! Get the next token.
     *  @param token The token buffer.
     *  @param error The error buffer.
     *
     *  On success, token will be populated.  On failure, error will
     *  be set accordingly, and token will be in an indeterminate
     *  state.
     */
    bool getNextToken(Token *token, Error *error);
    /*! Unget the token.
     *  @param token The token.
     *
     *  This copies the token.
     */
    void ungetToken(Token *token);
    /*! Push text into the lexer.
     *  @param str The text.
     *
     *  This must be done before any data is read from the lexer.
     */
    void pushText(const char *text);
};
}

#endif
