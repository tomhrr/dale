#ifndef DALE_FORM_PARAMETER
#define DALE_FORM_PARAMETER

#include "../../Units/Units.h"

namespace dale {
/*! Parse a parameter form.
 *  @param units The units context.
 *  @param var An empty variable for storing the argument details.
 *  @param node The node containing the argument definition.
 *  @param allow_anon_structs Whether anonymous struct types may be
 * used.
 *  @param allow_bitfields Whether bitfield types may be used.
 *  @param allow_refs Whether reference types may be used.
 *  @param allow_anonymous Whether types alone may be used.
 */
bool FormParameterParse(Units *units, Variable *var, Node *node,
                        bool allow_anon_structs, bool allow_bitfields,
                        bool allow_refs, bool allow_anonymous);
}

#endif
