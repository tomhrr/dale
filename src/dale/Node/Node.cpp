#include "Node.h"

#include <cstdio>

namespace dale
{
Node::Node()
{
    is_list  = 0;
    is_token = 0;

    list       = NULL;
    token      = NULL;
    filename   = NULL;
    list_begin.setLineAndColumn(0,0);
    list_end.setLineAndColumn(0,0);
    macro_begin.setLineAndColumn(0,0);
    macro_end.setLineAndColumn(0,0);
}

Node::Node(int empty)
{
    is_list  = 0;
    is_token = 0;

    list       = NULL;
    token      = NULL;
    filename   = NULL;
    list_begin.setLineAndColumn(0,0);
    list_end.setLineAndColumn(0,0);
    macro_begin.setLineAndColumn(0,0);
    macro_end.setLineAndColumn(0,0);
}

Node::Node(std::vector<Node*> *new_list)
{
    is_list  = 1;
    is_token = 0;

    list       = new_list;
    token      = NULL;
    filename   = NULL;
    list_begin.setLineAndColumn(0,0);
    list_end.setLineAndColumn(0,0);
    macro_begin.setLineAndColumn(0,0);
    macro_end.setLineAndColumn(0,0);
}

Node::Node(Token *new_token)
{
    is_list  = 0;
    is_token = 1;

    list       = NULL;
    token      = new_token;
    filename   = NULL;
    list_begin.setLineAndColumn(0,0);
    list_end.setLineAndColumn(0,0);
    macro_begin.setLineAndColumn(0,0);
    macro_end.setLineAndColumn(0,0);
}

Position *Node::getBeginPos(void)
{
    if (is_list || (!is_token)) {
        return &list_begin;
    } else {
        return &(token->begin);
    }
}

Position *Node::getEndPos(void)
{
    if (is_list || (!is_token)) {
        return &list_end;
    } else {
        return &(token->end);
    }
}

Node::~Node(void)
{
    /* It is assumed that nodes take ownership of their respective
     * lists/tokens. */

    if (is_token) {
        delete token;
    } else if (is_list) {
        // Iterate over the list, delete each node in turn.
        std::vector<Node *>::iterator iter = list->begin();

        while (iter != list->end()) {
            delete (*iter);
            ++iter;
        }
        delete list;
    }
}

void Node::print(void)
{
    if (is_token) {
        if (token->type == TokenType::StringLiteral) {
            printf("\"%s\" ", token->str_value.c_str());
        } else if (token->type == TokenType::Int) {
            //printf("%d ", token->int_value);

            // Numbers are all stored in strings now.
            printf("%s ", token->str_value.c_str());
        } else if (token->type == TokenType::String) {
            printf("%s ", token->str_value.c_str());
        } else if (token->type == TokenType::FloatingPoint) {
            printf("%s ", token->str_value.c_str());
        } else {
            printf("(Unknown) ");
        }
    } else if (is_list) {
        std::vector<Node *>::iterator iter =
            list->begin();

        printf("(");
        while (iter != list->end()) {
            (*iter)->print();
            iter++;
        }
        printf(")");
    } else {
        printf("(Not list or token)\n");
    }
}

/* todo: not sure why the string isn't copied, if present. */
void Node::copyTo(Node *other)
{
    other->is_list = is_list;
    other->is_token = is_token;
    copyMetaTo(other);
}

void Node::copyMetaTo(Node *other)
{
    other->list_begin.setLineAndColumn(
        getBeginPos()->getLineNumber(),
        getBeginPos()->getColumnNumber()
    );
    other->list_end.setLineAndColumn(
        getEndPos()->getLineNumber(),
        getEndPos()->getColumnNumber()
    );
    other->macro_begin = macro_begin;
    other->macro_end = macro_end;
    other->filename = filename;
}

Node *null_node;
Node *nullNode(void)
{
    if (null_node) {
        return null_node;
    }
    null_node = new Node(0);
    return null_node;
}
}
