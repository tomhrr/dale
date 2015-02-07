#ifndef DALE_DNODECONVERTER
#define DALE_DNODECONVERTER

#include "../ErrorReporter/ErrorReporter.h"

namespace dale
{
/*! DNodeConverter

    A very simple class that provides for converting DNodes into
    Nodes.
*/
class DNodeConverter
{
private:
    ErrorReporter *er;

public:
    /*! Construct a new DNodeConverter.
     *  @param er The error reporter.
     *
     *  This does not take ownership of the error reporter.
     */
    DNodeConverter(ErrorReporter *er);
    Node *toNode(DNode *dnode);
};
}

#endif
