#include "Lexer.h"

#include "../Utils/Utils.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace dale
{
Lexer::Lexer(FILE *new_file,
             std::vector<Token *> *new_ungot_tokens,
             int line_number,
             int column_number)
{
    current = new Position(line_number, column_number);

    file         = new_file;
    ungot_tokens = new_ungot_tokens;

    lexcnt = 0;
    chari  = 0;
    been_pushed = 0;
    reset_pos = 0;

    lexbuf[0] = '\0';
}

int Lexer::pushText(const char *text)
{
    int len = strlen(text);
    if ((len + lexcnt) > 8192) {
        fprintf(stderr, "Internal error: too much text pushed "
                "onto lexer buffer.\n");
        abort();
    }
    strcat(lexbuf, text);
    lexcnt += len;
    been_pushed = 1;

    return 1;
}

int Lexer::myfgetc()
{
    if (lexcnt == 0) {
        int bytes = fread(lexbuf, 1, 8192, file);
        if (!bytes) {
            return EOF;
        }
        lexcnt = bytes;
        chari = 0;

        if (been_pushed) {
            been_pushed = 0;
            current->setLineAndColumn(1,1);
            reset_pos = 1;
        }
    }
    --lexcnt;
    char r = lexbuf[chari++];
    return (int) r;
}

int Lexer::myungetc(char c)
{
    ++lexcnt;
    --chari;
    return 1;
}

Lexer::~Lexer(void)
{
    delete current;
    delete ungot_tokens;
}

ino_t Lexer::currentInode(void)
{
    return get_inode(fileno(file));
}

void Lexer::ungetToken(Token *token)
{
    Token *push_token = new Token(
        token->type,
        token->begin.line_number,
        token->begin.column_number,
        token->end.line_number,
        token->end.column_number
    );
    token->copyTo(push_token);

    ungot_tokens->push_back(push_token);
}

int Lexer::getNextToken(Token *token, Error *error)
{
    /* Reset error token. */
    error->instance = ErrorInst::Lexer::Null;

    /* Before doing anything else, check for an existing un-got
     * token. */
    if (!(ungot_tokens->empty())) {
        Token *popped_token = ungot_tokens->back();
        ungot_tokens->pop_back();
        popped_token->copyTo(token);
        delete(popped_token);
        return 1;
    }

    /* Set when you hit a token - will be used for token begin
     * position. */
    int begin_line_count = current->getLineNumber();
    int begin_col_count  = current->getColumnNumber();

    /* Set everytime - will be the last position for the token as
     * well as the current position for the context. */
    int end_line_count   = begin_line_count;
    int end_col_count    = begin_col_count;

    /* Current character. */
    int c;

    /* Current token type. */
    int type = TokenType::Null;

    /* Reset token string buffer. */
    token->str_value.clear();
    token->str_value.reserve(20);

    while (1) {
        c = myfgetc();

        if (reset_pos == 1) {
            begin_line_count = 1;
            end_line_count   = 1;

            int col_diff     = end_col_count - begin_col_count;
            begin_col_count  = 1;
            end_col_count    = col_diff + 1;

            reset_pos        = 0;
        }

        /* Single-line comments. */
        if ((c == ';') && (token->str_value.compare("#\\"))) {
            if (type) {
                myungetc(c);
                break;
            }

            while ((c = myfgetc()) && c != EOF && c != '\n') {
            }

            end_line_count++;
            end_col_count = 1;
            begin_col_count = 1;
            continue;
        }

        /* Multiple-line comments */
        if ((c == '|') && !(token->str_value.compare("#"))) {
            type = TokenType::Null;
            while ((c = myfgetc()) && (c != EOF) && (c != '|')) {
                if (c == '\n') {
                    end_line_count++;
                    end_col_count = 1;
                } else {
                    end_col_count++;
                }
            }
            if (c != EOF) {
                myfgetc();
            }
            end_col_count = 1;
            begin_col_count = 1;
            token->str_value.clear();
            token->str_value.reserve(20);
            continue;
        }

        /* String literals. */
        if ((c == '"') && (token->str_value.compare("#\\"))) {
            if (type) {
                myungetc(c);
                break;
            }
            type = TokenType::StringLiteral;
            begin_line_count = end_line_count;
            begin_col_count  = end_col_count;

            /* Read characters until you hit a double-quote. */
            while ((c = myfgetc())
                    && c != EOF
                    && ((c != '"')
                        || (token->str_value.length() &&
                            (token->str_value.at(
                                 token->str_value.length() - 1
                             ) == '\\')))) {
                if (c == '"') {
                    token->str_value.at(
                        token->str_value.length() - 1
                    ) = c;
                } else {
                    token->str_value.push_back(c);
                }
                end_col_count++;
            }
            if (c == EOF) {
                error->instance =
                    ErrorInst::Lexer::UnterminatedStringLiteral;
            } else {
                end_col_count++;
            }
            break;
        }

        /* Whitespace. */
        if (isspace(c)) {
            if (type) {
                myungetc(c);
                break;
            }

            if (c == '\n') {
                end_line_count++;
                end_col_count   = 1;
                begin_col_count = 1;
            } else {
                end_col_count++;
            }
            continue;
        }

        /* End-of-file. */
        if (c == EOF) {
            if (type) {
                myungetc(c);
                break;
            }

            type = TokenType::Eof;
            break;
        }

        /* Left parenthesis. */
        if (c == '(' && (token->str_value.compare("#\\"))) {
            if (type) {
                myungetc(c);
                break;
            }
            type = TokenType::LeftParen;
            begin_line_count = end_line_count;
            begin_col_count  = end_col_count;
            end_col_count++;
            break;
        }

        /* Right parenthesis. */
        if (c == ')' && (token->str_value.compare("#\\"))) {
            if (type) {
                myungetc(c);
                break;
            }
            type = TokenType::RightParen;
            begin_line_count = end_line_count;
            begin_col_count  = end_col_count;
            end_col_count++;
            break;
        }

        /* Potential integer. */
        if ((!type) && ((c == '-') || isdigit(c))) {
            type = TokenType::Int;
            begin_col_count  = end_col_count;
            begin_line_count = end_line_count;
        }

        /* Plain token. */
        if (!type) {
            type = TokenType::String;
            begin_col_count  = end_col_count;
            begin_line_count = end_line_count;
        }

        token->str_value.push_back(c);
        end_col_count++;
    }

    token->type = type;

    if (type == TokenType::Int) {
        if ((token->str_value.length() == 1)
                && (token->str_value.at(0) == '-')) {
            token->type = TokenType::String;
        } else if (strchr(token->str_value.c_str(), '.')) {
            token->type = TokenType::FloatingPoint;
            if (!is_simple_float(token->str_value.c_str())) {
                error->instance =
                    ErrorInst::Lexer::InvalidFloatingPointNumber;
            }
        } else {
            if (!is_simple_int(token->str_value.c_str())) {
                error->instance =
                    ErrorInst::Lexer::InvalidInteger;
            }
        }
    }

    if (error->instance == ErrorInst::Lexer::Null) {
        token->begin.setLineAndColumn(
            begin_line_count,
            begin_col_count
        );
        current->setLineAndColumn(
            end_line_count,
            end_col_count
        );
        current->copyTo(&(token->end));
        return 1;
    } else {
        error->begin.setLineAndColumn(
            begin_line_count,
            begin_col_count
        );
        current->setLineAndColumn(
            end_line_count,
            end_col_count
        );
        current->copyTo(&(error->end));

        return 0;
    }
}
}
