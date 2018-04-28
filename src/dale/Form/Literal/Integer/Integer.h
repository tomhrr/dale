#ifndef DALE_FORM_LITERAL_INTEGER
#define DALE_FORM_LITERAL_INTEGER

#include "../../../ErrorReporter/ErrorReporter.h"

namespace dale {
/*! Parse an integer literal.
 *  @param node The node containing the integer.
 *  @param er The error reporter.
 */
int FormLiteralIntegerParse(Node *node, ErrorReporter *er);
}

#endif
