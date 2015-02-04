#ifndef DALE_TOKENTYPE
#define DALE_TOKENTYPE

namespace dale
{
/*! TokenType

    Contains the token type definitions.
*/
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
}

#endif
