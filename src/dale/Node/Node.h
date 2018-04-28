#ifndef DALE_NODE
#define DALE_NODE

#include <vector>

#include "../Position/Position.h"
#include "../Token/Token.h"

/*! DNode

    The struct analogue to Node.  This must have the same definition
    as the DNode type defined in the drt module, so that DNodes may be
    passed between the compiler and Dale code.
*/
struct DNode {
    int is_list;
    char *token_str;
    DNode *list_node;
    DNode *next_node;
    int begin_line;
    int begin_column;
    int end_line;
    int end_column;
    int macro_begin_line;
    int macro_begin_column;
    int macro_end_line;
    int macro_end_column;
    const char *filename;
};

namespace dale {
/*! Node

    The core syntactic element class.  Each node is either a token
    (atom) node or a list of nodes.
*/
class Node {
    public:
    /*! Whether the node is a list node. */
    bool is_list;
    /*! Whether the node is a token node. */
    bool is_token;
    /*! The beginning position of the node. */
    Position list_begin;
    /*! The ending position of the node. */
    Position list_end;
    /*! The beginning position of the original macro that produced this
     * node. */
    Position macro_begin;
    /*! The ending position of the original macro that produced this
     * node. */
    Position macro_end;
    /*! The token. */
    Token *token;
    /*! The list. */
    std::vector<Node *> *list;
    /*! The name of the file from which the node was parsed. */
    const char *filename;

    /*! Construct a null node.
     *
     *  A null node is neither list nor token, and is generally only
     *  used to construct error messages that do not need to refer to
     *  a node.
     */
    Node();
    /*! Construct a token node.
     *  @param token The token.
     *
     *  This takes ownership of the token.
     */
    explicit Node(Token *token);
    /*! Construct a token node from a string.
     *  @param str The string.
     *
     *  This does not take ownership of the string.
     */
    explicit Node(const char *str);
    /*! Construct a list node.
     *  @param list The list.
     *
     *  This takes ownership of the list.
     */
    explicit Node(std::vector<Node *> *list);
    ~Node();
    /*! Print the node to the standard output.
     */
    void print();
    /*! Get the beginning position of the node.
     *
     *  This does not relinquish ownership of the position.
     */
    Position *getBeginPos();
    /*! Get the ending position of the node.
     *
     *  This does not relinquish ownership of the position.
     */
    Position *getEndPos();
    /*! Copy position and filename information to the argument node.
     *  @param other The other node.
     */
    void copyMetaTo(Node *other);
    /*! Copy all of this node's details to the argument node.
     *  @param other The other node.
     */
    void copyTo(Node *other);
    /*! Construct a DNode from this node.
     */
    DNode *toDNode();
    /*! Set the current node's macro position from the argument node.
     *  @param mp_node The node from which to take the macro position.
     *
     *  If the current node already has a macro position set, then
     *  that position is not overridden.
     */
    void addMacroPosition(Node *mp_node);
    /*! Initialise the node.
     *
     *  This should only be used by constructors.
     */
    void init();
};

Node *nullNode();
}

#endif
