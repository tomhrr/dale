#include "Parser.h"
#include <cstring>
#include <cstdlib>

namespace dale
{
Parser::Parser(Lexer *new_lxr, ErrorReporter *new_erep,
               const char *myfilename)
{
    lxr  = new_lxr;
    erep = new_erep;
    filename = strdup(myfilename);
}

Parser::~Parser()
{
    delete lxr;
    free((void*) filename);
}

void Parser::deleteNodeList(std::vector<Node *> *list)
{
    erep->flush();

    std::vector<Node *>::iterator iter =
        list->begin();

    while (iter != list->end()) {
        delete (*iter);
        ++iter;
    }

    delete list;
}

Node *Parser::getNextList(void)
{
    Token ts(TokenType::Null, 0, 0, 0, 0);
    Token te(TokenType::Null, 0, 0, 0, 0);
    Error e(erep->current_filename,
            ErrorInst::Lexer::Null, 0, 0, 0, 0);

    /* Pop the first token - should be a left parenthesis. */
    lxr->getNextToken(&ts, &e);

    if (e.instance != ErrorInst::Lexer::Null) {
        erep->addError(e);
        return NULL;
    }

    if (ts.type == TokenType::Eof) {
        return new Node;
    }

    if (ts.type != TokenType::LeftParen) {
        e.begin = new Position(ts.begin);
        e.end   = new Position(ts.end);
        e.instance = ErrorInst::Parser::ExpectedLeftParen;
        erep->addError(e);
        return NULL;
    }

    int res;
    std::vector<Node*> *list = new std::vector<Node*>;
    while ((res = getNextListInternal(list)) == 1) {
    }

    if (res == 0) {
        deleteNodeList(list);
        return NULL;
    }

    /* Pop the next token - should be a right parenthesis. */

    lxr->getNextToken(&te, &e);

    if (e.instance != ErrorInst::Lexer::Null) {
        erep->addError(e);
        deleteNodeList(list);
        return NULL;
    }

    if (te.type != TokenType::RightParen) {
        e.begin = new Position(te.begin);
        e.end   = new Position(te.end);
        e.instance = ErrorInst::Parser::MissingRightParen;
        erep->addError(e);
        deleteNodeList(list);
        return NULL;
    }

    Node *node = new Node(list);
    node->filename = filename;
    node->getBeginPos()->line_number   = ts.begin.getLineNumber();
    node->getBeginPos()->column_number = ts.begin.getColumnNumber();
    node->getEndPos()->line_number     = te.begin.getLineNumber();
    node->getEndPos()->column_number   = te.begin.getColumnNumber();

    return node;
}

int Parser::getNextListInternal(std::vector<Node*> *list)
{
    /* Get token */

    Token t(TokenType::Null, 0, 0, 0, 0);
    Error e(erep->current_filename,
            ErrorInst::Lexer::Null, 0, 0, 0, 0);

    do {
        lxr->getNextToken(&t, &e);
        if (e.instance == ErrorInst::Lexer::Null) {
            break;
        } else {
            erep->addError(e);
            e.instance = ErrorInst::Lexer::Null;
        }
    } while (1);

    if (t.type == TokenType::Eof) {
        return 2;
    }

    if (t.type == TokenType::LeftParen) {
        /* new list - create list node, create new list, set list
         * node to point to new list, recall */
        std::vector<Node*> *sublist = new std::vector<Node*>;
        Node *node = new Node(sublist);
        list->push_back(node);
        node->filename = filename;
        node->getBeginPos()->line_number   = t.begin.getLineNumber();
        node->getBeginPos()->column_number = t.begin.getColumnNumber();
        node->getEndPos()->line_number     = t.end.getLineNumber();
        node->getEndPos()->column_number   = t.end.getColumnNumber();

        int res;

        while ((res = getNextListInternal(sublist)) == 1) {
        }

        if (res == 0) {
            return 0;
        }

        lxr->getNextToken(&t, &e);

        if (e.instance != ErrorInst::Lexer::Null) {
            erep->addError(e);
            return 0;
        }

        if (t.type != TokenType::RightParen) {
            e.begin = new Position(t.begin);
            e.end   = new Position(t.end);
            e.instance = ErrorInst::Parser::MissingRightParen;
            erep->addError(e);
            return 0;
        }

        node->getEndPos()->line_number     = t.begin.getLineNumber();
        node->getEndPos()->column_number   = t.begin.getColumnNumber();

        return 1;
    }

    if (t.type == TokenType::RightParen) {
        lxr->ungetToken(&t);
        return 2;
    }

    Token *tok_ptr = new Token(TokenType::Null, 0, 0, 0, 0);
    t.copyTo(tok_ptr);
    Node *node = new Node(tok_ptr);
    node->filename = filename;

    list->push_back(node);

    return 1;
}
}
