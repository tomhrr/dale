#include "Node.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

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

DNode *
Node::toDNode(void)
{
    if (is_token) {
        Token *t     = token;
        DNode *dnode = (DNode*)malloc(sizeof(*dnode));

        std::string ttostr;
        t->valueToString(&ttostr);

        char *sv = (char*)malloc(ttostr.length() + 1);

        strncpy(sv, ttostr.c_str(), ttostr.length()+1);

        dnode->is_list   = 0;
        dnode->token_str = sv;
        dnode->list_node = NULL;
        dnode->next_node = NULL;

        dnode->begin_line   = getBeginPos()->getLineNumber();
        dnode->begin_column = getBeginPos()->getColumnNumber();
        dnode->end_line     = getEndPos()->getLineNumber();
        dnode->end_column   = getEndPos()->getColumnNumber();
        if (macro_begin.getLineNumber()) {
            dnode->macro_begin_line = macro_begin.getLineNumber();
            dnode->macro_begin_column =
                macro_begin.getColumnNumber();
            dnode->macro_end_line = macro_end.getLineNumber();
            dnode->macro_end_column =
                macro_end.getColumnNumber();
        }
        dnode->filename = filename;

        return dnode;
    }

    /* If node is list - for each element, call self, link the
     * nodes together. */

    if (is_list) {
        DNode *top_node = (DNode*)malloc(sizeof(*top_node));
        top_node->is_list   = 1;
        top_node->token_str = NULL;
        top_node->next_node = NULL;

        DNode *current_dnode = NULL;

        std::vector<Node*> *lst = list;

        std::vector<Node *>::iterator node_iter;
        node_iter = lst->begin();

        while (node_iter != lst->end()) {
            DNode *temp_node = (*node_iter)->toDNode();

            if (!current_dnode) {
                top_node->list_node = temp_node;
                current_dnode = temp_node;
            } else {
                current_dnode->next_node = temp_node;
                current_dnode            = temp_node;
            }

            ++node_iter;
        }

        top_node->begin_line   = getBeginPos()->getLineNumber();
        top_node->begin_column = getBeginPos()->getColumnNumber();
        top_node->end_line     = getEndPos()->getLineNumber();
        top_node->end_column   = getEndPos()->getColumnNumber();

        if (macro_begin.getLineNumber()) {
            top_node->macro_begin_line = macro_begin.getLineNumber();
            top_node->macro_begin_column =
                macro_begin.getColumnNumber();
            top_node->macro_end_line = macro_end.getLineNumber();
            top_node->macro_end_column =
                macro_end.getColumnNumber();
        }
        top_node->filename = filename;

        return top_node;
    }

    return NULL;
}

void
Node::addMacroPosition(Node *mp_node)
{
    if (!(macro_begin.getLineNumber())) {
        macro_begin.setLineAndColumn(
            mp_node->getBeginPos()->getLineNumber(),
            mp_node->getBeginPos()->getColumnNumber()
        );
        macro_end.setLineAndColumn(
            mp_node->getEndPos()->getLineNumber(),
            mp_node->getEndPos()->getColumnNumber()
        );
    }

    if (is_list) {
        for (std::vector<Node *>::iterator
                b = list->begin(),
                e = list->end();
                b != e;
                ++b) {
            (*b)->addMacroPosition(mp_node);
        }
    }

    return;
}
}
