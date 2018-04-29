#include "DNodeConverter.h"

#include <cstring>
#include <vector>

#include "../Utils/Utils.h"

using namespace dale::ErrorInst;

namespace dale {
DNodeConverter::DNodeConverter(ErrorReporter *er) { this->er = er; }

static void setNodeMacroPosition(Node *node, DNode *dnode) {
    node->macro_begin.setLineAndColumn(dnode->macro_begin_line,
                                       dnode->macro_begin_column);
    node->macro_end.setLineAndColumn(dnode->macro_end_line,
                                     dnode->macro_end_column);
}

static void setNodePosition(Node *node, DNode *dnode) {
    node->list_begin.setLineAndColumn(dnode->begin_line,
                                      dnode->begin_column);
    node->list_end.setLineAndColumn(dnode->end_line, dnode->end_column);
    setNodeMacroPosition(node, dnode);
}

static Token *dnodeToNullToken(DNode *dnode) {
    return new Token(TokenType::Null, dnode->begin_line,
                     dnode->begin_column, dnode->end_line,
                     dnode->end_column);
}

Node *DNodeConverter::numberAtomToNode(DNode *dnode, Node *error_node) {
    Token *token = dnodeToNullToken(dnode);

    token->str_value.append(dnode->token_str);
    Node *n = new Node(token);
    setNodeMacroPosition(n, dnode);

    if (strchr(dnode->token_str, '.')) {
        if (!isSimpleFloat(dnode->token_str)) {
            Error *e = new Error(InvalidFloatingPointNumber, n);
            delete token;
            er->addError(e);
            return NULL;
        } else {
            token->type = TokenType::FloatingPoint;
        }
    } else {
        if (!isSimpleInt(dnode->token_str)) {
            Error *e = new Error(InvalidInteger, n);
            delete token;
            er->addError(e);
            return NULL;
        } else {
            token->type = TokenType::Int;
        }
    }
    n->filename = dnode->filename;
    return n;
}

Node *DNodeConverter::stringLiteralAtomToNode(DNode *dnode) {
    Token *token = dnodeToNullToken(dnode);

    token->type = TokenType::StringLiteral;
    token->str_value.append(dnode->token_str + 1,
                            (strlen(dnode->token_str) - 2));

    Node *n = new Node(token);
    setNodeMacroPosition(n, dnode);
    n->filename = dnode->filename;
    return n;
}

Node *DNodeConverter::stringAtomToNode(DNode *dnode) {
    Token *token = dnodeToNullToken(dnode);

    token->type = TokenType::String;
    token->str_value.append(dnode->token_str);

    Node *mynode = new Node(token);
    setNodeMacroPosition(mynode, dnode);
    mynode->filename = dnode->filename;
    return mynode;
}

Node *DNodeConverter::atomToNode(DNode *dnode, Node *error_node) {
    if (!dnode->token_str || (strlen(dnode->token_str) == 0)) {
        Error *e = new Error(DNodeHasNoString, error_node);
        er->addError(e);
        return NULL;
    }

    char c = (dnode->token_str)[0];
    char d = (dnode->token_str)[1];
    int len = strlen(dnode->token_str);
    if ((((len > 1) && (c == '-') && (isdigit(d)))) || (isdigit(c))) {
        return numberAtomToNode(dnode, error_node);
    } else if ((c == '"') &&
               (dnode->token_str)[strlen(dnode->token_str) - 1] ==
                   '"') {
        return stringLiteralAtomToNode(dnode);
    } else {
        return stringAtomToNode(dnode);
    }
}

Node *DNodeConverter::listToNode(DNode *dnode) {
    std::vector<Node *> *list = new std::vector<Node *>;

    DNode *current_node = dnode->list_node;
    while (current_node) {
        Node *new_node = toNode(current_node);
        list->push_back(new_node);
        current_node = current_node->next_node;
    }

    Node *final_node = new Node(list);
    final_node->filename = dnode->filename;
    setNodePosition(final_node, dnode);
    return final_node;
}

Node *DNodeConverter::toNode(DNode *dnode) {
    Node error_node;
    setNodePosition(&error_node, dnode);
    error_node.filename = dnode->filename;

    if (!dnode->is_list) {
        return atomToNode(dnode, &error_node);
    } else if (dnode->is_list) {
        return listToNode(dnode);
    } else {
        Error *e = new Error(DNodeIsNeitherTokenNorList, &error_node);
        er->addError(e);
        return NULL;
    }
}
}
