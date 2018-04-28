#ifndef DALE_FORM_TYPE
#define DALE_FORM_TYPE

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../Type/Type.h"
#include "../../Units/Units.h"

namespace dale {
/*! Parse a type definition form.
 *  @param units The units context.
 *  @param node The node containing the definition.
 *  @param allow_anon_structs Whether anonymous struct types may be
 * used.
 *  @param allow_bitfields Whether bitfield types may be used.
 *  @param allow_refs Whether reference types may be used.
 *  @param allow_retvals Whether retval types may be used.
 */
Type *FormTypeParse(Units *units, Node *node, bool allow_anon_structs,
                    bool allow_bitfields, bool allow_refs = false,
                    bool allow_retvals = false);
}

#endif
