#ifndef DALE_NODE
#define DALE_NODE

#include "../Token/Token.h"
#include "../Position/Position.h"

#include <vector>

namespace dale
{
class Node
{
public:
    int                  is_list;
    int                  is_token;
    Position             macro_begin;
    Position             macro_end;
    Position             list_begin;
    Position             list_end;
    Token               *token;
    std::vector<Node *> *list;
    const char*          filename;

    Node(void);
    Node(int empty);
    Node(Token *token);
    Node(std::vector<Node *> *list);
    void print(void);
    Position *getBeginPos(void);
    Position *getEndPos(void);
    void copyMetaTo(Node *other);
    void copyTo(Node *other);
    ~Node(void);
};
}

#endif
