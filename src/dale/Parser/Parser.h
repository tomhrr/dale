#ifndef DALE_PARSER
#define DALE_PARSER

#include <vector>

#include "../ErrorReporter/ErrorReporter.h"
#include "../Lexer/Lexer.h"
#include "../Node/Node.h"

namespace dale {
/*! Parser

    Self-explanatory.  Only returns node lists, since tokens may not
    appear at the top level.
*/
class Parser {
    private:
    /*! The lexer for this parser. */
    Lexer *lexer;
    /*! The error reporter for this parser. */
    ErrorReporter *erep;
    /*! The filename of the file being parsed. */
    const char *filename;

    /*! Populate the list with nodes.
     *  @param list The node list buffer.
     *
     *  This does not take ownership of the list.  Ownership of nodes
     *  added to the list is passed to the caller.  A return value of
     *  1 indicates that parsing may continue, a return value of 0
     *  indicates that an error occurred, and a return value of 2
     *  indicates EOF.
     */
    int getNextNodeInternal(std::vector<Node *> *list);
    /*! Delete the node list.
     *  @param list The node list.
     *
     *  Deletes each individual node, as well as the node list.
     */
    void deleteNodeList(std::vector<Node *> *list);
    /*! Fetch tokens until a right parenthesis or EOF is reached.
     *  @param token The storage for the token.
     *  @param error The storage for any error.
     */
    void getUntilRightParenOrEof(Token *token, Error *error);

    public:
    /*! Construct a new parser.
     *  @param lexer The lexer for the parser.
     *  @param erep The error reporter for the parser.
     *  @param filename The filename of the file being parsed.
     *
     *  This takes ownership of the lexer.
     */
    Parser(Lexer *lexer, ErrorReporter *erep, const char *filename);
    ~Parser();
    /*! Get the lexer.
     *
     *  This does not relinquish ownership of the lexer.
     */
    Lexer *getLexer();
    /*! Get the next node.
     *
     *  Ownership of the new node is passed to the caller.
     */
    Node *getNextNode();
    /*! Get the next list node.
     *
     *  Ownership of the new node list is passed to the caller.
     */
    Node *getNextList();
};
}

#endif
