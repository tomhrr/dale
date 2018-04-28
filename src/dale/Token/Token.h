#ifndef DALE_TOKEN
#define DALE_TOKEN

#include <string>

#include "../Position/Position.h"
#include "../TokenType/TokenType.h"

namespace dale {
/*! Token

    Represents a single syntactic token.  Values of all types are
    stored as strings.
*/
class Token {
    public:
    /*! The token type. */
    int type;
    /*! The token value. */
    std::string str_value;
    /*! The beginning position of the token. */
    Position begin;
    /*! The ending position of the token. */
    Position end;

    /*! Construct a new token.
     *  @param type The type.
     *  @param begin_line_number The beginning line number of the token.
     *  @param begin_column_number The beginning column number of the
     * token.
     *  @param end_line_number The ending line number of the token.
     *  @param end_column_number The ending column number of the token.
     */
    Token(int type, int begin_line_number = 0,
          int begin_column_number = 0, int end_line_number = 0,
          int end_column_number = 0);
    /*! Construct a new token using the details of another token.
     *  @param other The other token.
     */
    explicit Token(Token *other);
    ~Token();

    /*! Stringify a token.
     *  @param str The string buffer.
     */
    void toString(std::string *str);
    /*! Copy a token's details to another token.
     *  @param other The other token.
     */
    void copyTo(Token *other);
    /*! Get the token's type as a string.
     */
    const char *tokenType();
};
}

#endif
