#include "Lexer.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../Utils/Utils.h"

namespace dale {
Lexer::Lexer(FILE *file, bool line_buffered) {
    current.setLineAndColumn(1, 1);
    this->file = file;
    this->line_buffered = line_buffered;

    count = 0;
    index = 0;

    been_pushed = false;
    reset_position = false;

    buf[0] = '\0';
}

Lexer::~Lexer() {}

int Lexer::getchar_() {
    if (count == 0) {
        size_t bytes;
        if (line_buffered) {
            char *line = NULL;
            size_t allocated = 0;
            bytes = getline(&line, &allocated, file);
            if (!bytes) {
                return EOF;
            }
            strncpy(buf, line, bytes);
        } else {
            bytes = fread(buf, 1, 8192, file);
            if (!bytes) {
                return EOF;
            }
        }

        count = bytes;
        index = 0;

        if (been_pushed) {
            been_pushed = false;
            current.setLineAndColumn(1, 1);
            reset_position = true;
        }
    }

    --count;
    char r = buf[index++];
    return static_cast<int>(r);
}

void Lexer::ungetchar_(char c) {
    ++count;
    --index;
}

void Lexer::pushText(const char *text) {
    int len = strlen(text);
    assert((len + count) <= 8192);
    snprintf(buf + count, len + 1, "%s", text);
    count += len;
    been_pushed = true;
}

void Lexer::ungetToken(Token *token) {
    ungot_tokens.push_back(new Token(token));
}

bool Lexer::getNextToken(Token *token, Error *error) {
    error->instance = ErrorInst::Null;

    if (!(ungot_tokens.empty())) {
        Token *popped_token = ungot_tokens.back();
        ungot_tokens.pop_back();
        popped_token->copyTo(token);
        delete (popped_token);
        return true;
    }

    /* Set when a token is hit: will be used for token begin position.
     */
    int begin_line_count = current.getLineNumber();
    int begin_col_count = current.getColumnNumber();

    /* Set everytime: will be the last position for the token as well
     * as the current position for the context. */
    int end_line_count = begin_line_count;
    int end_col_count = begin_col_count;

    /* Current character. */
    int c;

    /* Current token type. */
    int type = TokenType::Null;

    /* Reset token string buffer. */
    token->str_value.clear();
    token->str_value.reserve(20);

    for (;;) {
        c = getchar_();

        if (reset_position) {
            begin_line_count = 1;
            end_line_count = 1;

            int col_diff = end_col_count - begin_col_count;
            begin_col_count = 1;
            end_col_count = col_diff + 1;

            reset_position = false;
        }

        /* Single-line comments. */
        if ((c == ';') && (token->str_value.compare("#\\"))) {
            if (type) {
                ungetchar_(c);
                break;
            }

            while ((c = getchar_()) && c != EOF && c != '\n') {
            }

            end_line_count++;
            end_col_count = 1;
            begin_col_count = 1;
            continue;
        }

        /* Multiple-line comments */
        if ((c == '|') && !(token->str_value.compare("#"))) {
            type = TokenType::Null;
            int count = 1;
            int last = c;
            while ((c = getchar_()) && (c != EOF)) {
                if (c == '\n') {
                    end_line_count++;
                    end_col_count = 1;
                } else {
                    end_col_count++;
                }
                if ((last == '|') && (c == '#')) {
                    count--;
                } else if ((last == '#') && (c == '|')) {
                    count++;
                }
                if (!count) {
                    break;
                }
                last = c;
            }
            if (c != EOF) {
                c = getchar_();
                if (c == '\n') {
                    end_line_count++;
                }
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
                ungetchar_(c);
                break;
            }
            type = TokenType::StringLiteral;
            begin_line_count = end_line_count;
            begin_col_count = end_col_count;

            /* Read characters until you hit a double-quote. */
            while ((c = getchar_()) && c != EOF) {
                int slash = 0;
                if (c == '\\') {
                    c = getchar_();
                    if (c == EOF) {
                        goto end;
                    }
                    if (c != '"') {
                        token->str_value.push_back('\\');
                        end_col_count++;
                    }
                } else if (c == '"') {
                    goto end;
                }
                token->str_value.push_back(c);
                if (!slash && (c == '\n')) {
                    end_line_count++;
                } else {
                    end_col_count++;
                }
            }
            end:
            if (c == EOF) {
                error->instance = ErrorInst::UnterminatedStringLiteral;
            } else {
                end_col_count++;
            }
            break;
        }

        /* Whitespace. */
        if (isspace(c)) {
            if (type) {
                ungetchar_(c);
                break;
            }

            if (c == '\n') {
                end_line_count++;
                end_col_count = 1;
                begin_col_count = 1;
            } else {
                end_col_count++;
            }
            continue;
        }

        /* End-of-file. */
        if (c == EOF) {
            if (type) {
                ungetchar_(c);
                break;
            }

            type = TokenType::Eof;
            break;
        }

        /* Left parenthesis. */
        if (c == '(' && (token->str_value.compare("#\\"))) {
            if (type) {
                ungetchar_(c);
                break;
            }
            type = TokenType::LeftParen;
            begin_line_count = end_line_count;
            begin_col_count = end_col_count;
            end_col_count++;
            break;
        }

        /* Right parenthesis. */
        if (c == ')' && (token->str_value.compare("#\\"))) {
            if (type) {
                ungetchar_(c);
                break;
            }
            type = TokenType::RightParen;
            begin_line_count = end_line_count;
            begin_col_count = end_col_count;
            end_col_count++;
            break;
        }

        /* Potential integer. */
        if ((!type) && ((c == '-') || isdigit(c))) {
            type = TokenType::Int;
            begin_col_count = end_col_count;
            begin_line_count = end_line_count;
        }

        /* Previous potential integer. */
        if ((type == TokenType::Int) &&
            (token->str_value.size() == 1) &&
            (*(token->str_value.begin()) == '-') && !isdigit(c)) {
            type = TokenType::String;
        }

        /* Plain token. */
        if (!type) {
            type = TokenType::String;
            begin_col_count = end_col_count;
            begin_line_count = end_line_count;
        }

        token->str_value.push_back(c);
        end_col_count++;
    }

    token->type = type;

    if (type == TokenType::Int) {
        if ((token->str_value.length() == 1) &&
            (token->str_value[0] == '-')) {
            token->type = TokenType::String;
        } else if (strchr(token->str_value.c_str(), '.')) {
            token->type = TokenType::FloatingPoint;
            if (!isSimpleFloat(token->str_value.c_str())) {
                error->instance = ErrorInst::InvalidFloatingPointNumber;
            }
        } else {
            if (!isSimpleInt(token->str_value.c_str())) {
                error->instance = ErrorInst::InvalidInteger;
            }
        }
    }

    current.setLineAndColumn(end_line_count, end_col_count);

    if (error->instance == ErrorInst::Null) {
        token->begin.setLineAndColumn(begin_line_count,
                                      begin_col_count);
        current.copyTo(&(token->end));
        return true;
    } else {
        error->begin.setLineAndColumn(begin_line_count,
                                      begin_col_count);
        current.copyTo(&(error->end));
        return false;
    }
}
}
