#ifndef DALE_TOKENTYPE
#define DALE_TOKENTYPE

namespace dale {
/*! TokenType

    Contains the token type definitions and related utility functions.
*/
namespace TokenType {
enum {
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

/*! Get the token type as a string.
 *  @param token_type The token type.
 *
 *  If the token type is invalid, the string "[unknown]" will be
 *  returned.
 */
const char *tokenTypeToString(int token_type);
}

#endif
