#include "Node.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "../Utils/Utils.h"

namespace dale {
Node::Node() { init(); }

Node::Node(std::vector<Node *> *new_list) {
    init();
    is_list = true;
    list = new_list;
}

Node::Node(Token *new_token) {
    init();
    is_token = true;
    token = new_token;
}

Node::Node(const char *str) {
    init();
    is_token = true;
    token = new Token(TokenType::String);
    token->str_value.append(str);
}

Node::~Node() {
    if (is_token) {
        delete token;
    } else if (is_list) {
        for (std::vector<Node *>::iterator b = list->begin(),
                                           e = list->end();
             b != e; ++b) {
            delete (*b);
        }
        delete list;
    }
}

Position *Node::getBeginPos() {
    if (is_list || (!is_token)) {
        return &list_begin;
    } else {
        return &(token->begin);
    }
}

Position *Node::getEndPos() {
    if (is_list || (!is_token)) {
        return &list_end;
    } else {
        return &(token->end);
    }
}

void Node::print() {
    if (is_token) {
        if (token->type == TokenType::StringLiteral) {
            printf("\"%s\"", token->str_value.c_str());
        } else if (token->type == TokenType::Int ||
                   token->type == TokenType::String ||
                   token->type == TokenType::FloatingPoint) {
            printf("%s", token->str_value.c_str());
        } else {
            printf("(Unknown)");
        }
    } else if (is_list) {
        printf("(");
        for (std::vector<Node *>::iterator b = list->begin(),
                                           e = list->end();
             b != e; ++b) {
            (*b)->print();
            if ((b + 1) != e) {
                printf(" ");
            }
        }
        printf(")");
    } else {
        printf("(Not list or token)\n");
    }
}

void Node::copyTo(Node *other) {
    other->is_list = is_list;
    other->is_token = is_token;
    if (is_list) {
        std::vector<Node *> *list_copy = new std::vector<Node *>();
        for (std::vector<Node *>::iterator b = list->begin(),
                                           e = list->end();
             b != e; ++b) {
            Node *node = ((*b)->is_token) ? new Node((*b)->token)
                                          : new Node((*b)->list);
            list_copy->push_back(node);
        }
        other->list = list_copy;
    } else {
        this->token->copyTo(other->token);
    }
    copyMetaTo(other);
}

void Node::copyMetaTo(Node *other) {
    other->list_begin.setLineAndColumn(
        getBeginPos()->getLineNumber(),
        getBeginPos()->getColumnNumber());
    other->list_end.setLineAndColumn(getEndPos()->getLineNumber(),
                                     getEndPos()->getColumnNumber());
    other->macro_begin = macro_begin;
    other->macro_end = macro_end;
    other->filename = filename;
}

Node *null_node;
Node *nullNode() {
    if (null_node) {
        return null_node;
    }
    null_node = new Node();
    return null_node;
}

DNode *Node::toDNode() {
    if (is_token) {
        Token *t = token;
        DNode *dnode =
            reinterpret_cast<DNode *>(malloc(sizeof(*dnode)));
        if (!dnode) {
            error("unable to allocate memory", true);
        }

        std::string token_str;
        t->toString(&token_str);

        char *sv =
            reinterpret_cast<char *>(malloc(token_str.length() + 1));
        if (!sv) {
            error("unable to allocate memory", true);
        }
        strncpy(sv, token_str.c_str(), token_str.length() + 1);

        dnode->is_list = false;
        dnode->token_str = sv;
        dnode->list_node = NULL;
        dnode->next_node = NULL;

        dnode->begin_line = getBeginPos()->getLineNumber();
        dnode->begin_column = getBeginPos()->getColumnNumber();
        dnode->end_line = getEndPos()->getLineNumber();
        dnode->end_column = getEndPos()->getColumnNumber();
        if (macro_begin.getLineNumber()) {
            dnode->macro_begin_line = macro_begin.getLineNumber();
            dnode->macro_begin_column = macro_begin.getColumnNumber();
            dnode->macro_end_line = macro_end.getLineNumber();
            dnode->macro_end_column = macro_end.getColumnNumber();
        }
        dnode->filename = filename;

        return dnode;
    } else if (is_list) {
        DNode *top_node =
            reinterpret_cast<DNode *>(malloc(sizeof(*top_node)));
        if (!top_node) {
            error("unable to allocate memory", true);
        }
        top_node->is_list = true;
        top_node->token_str = NULL;
        top_node->next_node = NULL;
        top_node->list_node = NULL;

        DNode *current_dnode = NULL;

        std::vector<Node *> *lst = list;
        for (std::vector<Node *>::iterator b = lst->begin(),
                                           e = lst->end();
             b != e; ++b) {
            DNode *lst_dnode = (*b)->toDNode();
            if (!current_dnode) {
                top_node->list_node = lst_dnode;
                current_dnode = lst_dnode;
            } else {
                current_dnode->next_node = lst_dnode;
                current_dnode = lst_dnode;
            }
        }

        top_node->begin_line = getBeginPos()->getLineNumber();
        top_node->begin_column = getBeginPos()->getColumnNumber();
        top_node->end_line = getEndPos()->getLineNumber();
        top_node->end_column = getEndPos()->getColumnNumber();

        if (macro_begin.getLineNumber()) {
            top_node->macro_begin_line = macro_begin.getLineNumber();
            top_node->macro_begin_column =
                macro_begin.getColumnNumber();
            top_node->macro_end_line = macro_end.getLineNumber();
            top_node->macro_end_column = macro_end.getColumnNumber();
        }
        top_node->filename = filename;

        return top_node;
    }

    return NULL;
}

void Node::addMacroPosition(Node *mp_node) {
    /* Previously, macro positions would not be set on a node that
     * already had macro poisition information.  Now, that information
     * is always overridden.  Using the most-recent node information
     * tends to point the user closer to the actual error. */

    macro_begin.setLineAndColumn(
        mp_node->getBeginPos()->getLineNumber(),
        mp_node->getBeginPos()->getColumnNumber());
    macro_end.setLineAndColumn(mp_node->getEndPos()->getLineNumber(),
                               mp_node->getEndPos()->getColumnNumber());

    if (is_list) {
        for (std::vector<Node *>::iterator b = list->begin(),
                                           e = list->end();
             b != e; ++b) {
            (*b)->addMacroPosition(mp_node);
        }
    }

    return;
}

void Node::init() {
    is_list = false;
    is_token = false;

    list = NULL;
    token = NULL;
    filename = NULL;

    list_begin.zero();
    list_end.zero();
    macro_begin.zero();
    macro_end.zero();
}
}
