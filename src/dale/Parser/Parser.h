#ifndef DALE_PARSER
#define DALE_PARSER

#include <vector>

#include "../Lexer/Lexer.h"
#include "../Node/Node.h"
#include "../ErrorReporter/ErrorReporter.h"

namespace dale
{
class Parser
{
public:
    Lexer *lxr;
    ErrorReporter *erep;

    Parser(Lexer *lxr, ErrorReporter *erep, const char *myfilename);
    ~Parser();
    Node *getNextList(void);
    const char *filename;

private:
    int getNextListInternal(std::vector<Node*> *list);
    void deleteNodeList(std::vector<Node *> *list);

};
}

#endif
