#ifndef DALE_NODE
#define DALE_NODE

#include "../Token/Token.h"
#include "../Position/Position.h"

#include <vector>

struct DNode
{
    int    is_list;
    char  *token_str;
    DNode *list_node;
    DNode *next_node;
    int    begin_line;
    int    begin_column;
    int    end_line;
    int    end_column;
    int    macro_begin_line;
    int    macro_begin_column;
    int    macro_end_line;
    int    macro_end_column;
    const char  *filename;
};

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
    DNode *toDNode(void);
    void addMacroPosition(Node *mp_node);
    ~Node(void);
};

Node *nullNode(void);
}

#endif
