#ifndef DALE_DNODECONVERTER
#define DALE_DNODECONVERTER

#include "../ErrorReporter/ErrorReporter.h"

namespace dale {
/*! DNodeConverter

    A very simple class that provides for converting DNodes into
    Nodes.
*/
class DNodeConverter {
    private:
    ErrorReporter *er;
    Node *numberAtomToNode(DNode *dnode, Node *error_node);
    Node *stringLiteralAtomToNode(DNode *dnode);
    Node *stringAtomToNode(DNode *dnode);
    Node *atomToNode(DNode *dnode, Node *error_node);
    Node *listToNode(DNode *dnode);

    public:
    /*! Construct a new DNodeConverter.
     *  @param er The error reporter.
     *
     *  This does not take ownership of the error reporter.
     */
    explicit DNodeConverter(ErrorReporter *er);
    Node *toNode(DNode *dnode);
};
}

#endif
