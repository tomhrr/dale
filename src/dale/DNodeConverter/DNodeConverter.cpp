#include "DNodeConverter.h"

#include <cstring>
#include "../Utils/Utils.h"

namespace dale
{
DNodeConverter::DNodeConverter(ErrorReporter *er)
{
    this->er = er;
}

Node *
DNodeConverter::toNode(DNode *dnode)
{
    Node *tempnode = new Node();
    tempnode->list_begin.setLineAndColumn(dnode->begin_line, dnode->begin_column);
    tempnode->list_end.setLineAndColumn(dnode->end_line, dnode->end_column);
    tempnode->macro_begin.setLineAndColumn(dnode->macro_begin_line, dnode->macro_begin_column);
    tempnode->macro_end.setLineAndColumn(dnode->macro_end_line, dnode->macro_end_column);
    tempnode->filename = dnode->filename;

    if (!dnode->is_list) {
        Token *token = new Token(TokenType::Null,
                                 dnode->begin_line,
                                 dnode->begin_column,
                                 dnode->end_line,
                                 dnode->end_column);

        if (!dnode->token_str) {
            Error *e = new Error(
                ErrorInst::Generator::DNodeHasNoString,
                tempnode
            );
            er->addError(e);
            delete token;
            return NULL;
        }
        if (strlen(dnode->token_str) == 0) {
            Error *e = new Error(
                ErrorInst::Generator::DNodeHasNoString,
                tempnode
            );
            er->addError(e);
            delete token;
            return NULL;
        }

        char c = (dnode->token_str)[0];
        char d = (dnode->token_str)[1];
        int len = strlen(dnode->token_str);
        if ((((len > 1) && (c == '-') && (isdigit(d))))
                || (isdigit(c))) {
            /* Is an integer (or a float). */

            token->str_value.append(dnode->token_str);
            Node *n = new Node(token);
            n->macro_begin.setLineAndColumn(dnode->macro_begin_line, dnode->macro_begin_column);
            n->macro_end.setLineAndColumn(dnode->macro_end_line, dnode->macro_end_column);

            if (strchr(dnode->token_str, '.')) {
                if (!isSimpleFloat(dnode->token_str)) {
                    Error *e = new Error(
                        ErrorInst::Lexer::InvalidFloatingPointNumber,
                        n
                    );

                    er->addError(e);
                    return NULL;
                } else {
                    token->type = TokenType::FloatingPoint;
                }
            } else {
                if (!isSimpleInt(dnode->token_str)) {
                    Error *e = new Error(
                        ErrorInst::Lexer::InvalidInteger,
                        n
                    );
                    er->addError(e);
                    return NULL;
                } else {
                    token->type = TokenType::Int;
                }
            }
            n->filename = dnode->filename;
            return n;
        }
        if ((c == '"') &&
                (dnode->token_str)[strlen(dnode->token_str)-1] == '"') {
            /* Is a string literal. */

            char str[256];
            str[0] = '\0';

            strncpy(str, (dnode->token_str) + 1, (strlen(dnode->token_str)-2));
            str[strlen(dnode->token_str)-2] = '\0';

            token->type = TokenType::StringLiteral;
            token->str_value.append(str);

            Node *n = new Node(token);
            n->macro_begin.setLineAndColumn(dnode->macro_begin_line, dnode->macro_begin_column);
            n->macro_end.setLineAndColumn(dnode->macro_end_line, dnode->macro_end_column);
            n->filename = dnode->filename;
            return n;
        }

        /* If neither - is plain string. */

        token->type = TokenType::String;
        token->str_value.append(dnode->token_str);

        Node *mynode = new Node(token);
        mynode->macro_begin.setLineAndColumn(dnode->macro_begin_line, dnode->macro_begin_column);
        mynode->macro_end.setLineAndColumn(dnode->macro_end_line, dnode->macro_end_column);
        mynode->filename = dnode->filename;
        return mynode;
    }
    /* DNode is list. */
    if (dnode->is_list) {
        std::vector<Node *> *list = new std::vector<Node *>;

        DNode *current_node = dnode->list_node;

        while (current_node) {
            Node *new_node = toNode(current_node);
            list->push_back(new_node);
            current_node = current_node->next_node;
        }

        Node *final_node = new Node(list);
        final_node->filename = dnode->filename;
        final_node->list_begin.setLineAndColumn(dnode->begin_line, dnode->begin_column);
        final_node->list_end.setLineAndColumn(dnode->end_line, dnode->end_column);
        final_node->macro_begin.setLineAndColumn(dnode->macro_begin_line, dnode->macro_begin_column);
        final_node->macro_end.setLineAndColumn(dnode->macro_end_line, dnode->macro_end_column);
        return final_node;
    }

    Error *e = new Error(
        ErrorInst::Generator::DNodeIsNeitherTokenNorList,
        tempnode
    );
    er->addError(e);

    return NULL;
}
}
