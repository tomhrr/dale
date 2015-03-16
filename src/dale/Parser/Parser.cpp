#include "Parser.h"
#include <cstring>
#include <cstdlib>

namespace dale
{
Parser::Parser(Lexer *lexer, ErrorReporter *erep,
               const char *filename)
{
    this->lexer      = lexer;
    this->erep     = erep;
    this->filename = filename;
}

Parser::~Parser()
{
    delete lexer;
}

Lexer *
Parser::getLexer(void)
{
    return lexer;
}

void
Parser::deleteNodeList(std::vector<Node *> *list)
{
    erep->flush();

    for (std::vector<Node *>::iterator b = list->begin(),
                                       e = list->end();
            b != e; ++b) {
        delete (*b);
    }

    delete list;
}

Node *
Parser::getNextList(void)
{
    Token ts(TokenType::Null);
    Token te(TokenType::Null);
    Node n;
    n.filename = filename;
    Error e(ErrorInst::Null, &n);

    lexer->getNextToken(&ts, &e);

    if (e.instance != ErrorInst::Null) {
        erep->addError(e);
        return NULL;
    }

    if (ts.type == TokenType::Eof) {
        return new Node;
    }

    if (ts.type != TokenType::LeftParen) {
        e.begin = new Position(ts.begin);
        e.end   = new Position(ts.end);
        e.instance = ErrorInst::ExpectedLeftParen;
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

    lexer->getNextToken(&te, &e);

    if (e.instance != ErrorInst::Null) {
        erep->addError(e);
        deleteNodeList(list);
        return NULL;
    }

    if (te.type != TokenType::RightParen) {
        e.begin = new Position(te.begin);
        e.end   = new Position(te.end);
        e.instance = ErrorInst::MissingRightParen;
        erep->addError(e);
        deleteNodeList(list);
        return NULL;
    }

    Node *node = new Node(list);
    node->filename = filename;
    ts.begin.copyTo(node->getBeginPos());
    te.begin.copyTo(node->getEndPos());

    return node;
}

int
Parser::getNextListInternal(std::vector<Node*> *list)
{
    Token t(TokenType::Null);
    Node n;
    n.filename = filename;
    Error e(ErrorInst::Null, &n);

    do {
        lexer->getNextToken(&t, &e);
        if (e.instance == ErrorInst::Null) {
            break;
        } else {
            erep->addError(e);
            e.instance = ErrorInst::Null;
        }
    } while (1);

    if (t.type == TokenType::Eof) {
        return 2;
    }

    if (t.type == TokenType::LeftParen) {
        std::vector<Node*> *sublist = new std::vector<Node*>();
        Node *node = new Node(sublist);
        list->push_back(node);
        node->filename = filename;
        t.begin.copyTo(node->getBeginPos());
        t.end.copyTo(node->getEndPos());

        int res;
        while ((res = (getNextListInternal(sublist))) == 1) {
        }

        if (res == 0) {
            return 0;
        }

        lexer->getNextToken(&t, &e);

        if (e.instance != ErrorInst::Null) {
            erep->addError(e);
            return 0;
        }

        if (t.type != TokenType::RightParen) {
            e.begin = new Position(t.begin);
            e.end   = new Position(t.end);
            e.instance = ErrorInst::MissingRightParen;
            erep->addError(e);
            return 0;
        }
        t.begin.copyTo(node->getEndPos());

        return 1;
    }

    if (t.type == TokenType::RightParen) {
        lexer->ungetToken(&t);
        return 2;
    }

    Token *tok_ptr = new Token(TokenType::Null);
    t.copyTo(tok_ptr);
    Node *node = new Node(tok_ptr);
    node->filename = filename;

    list->push_back(node);

    return 1;
}
}
