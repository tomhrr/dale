#ifndef DALE_FORM_INITIALISER_STRUCT
#define DALE_FORM_INITIALISER_STRUCT

#include "../../../Context/Context.h"
#include "../../../Node/Node.h"
#include "../../../Units/Units.h"

namespace dale {
/*! Parse a struct initialiser.
 *  @param units The units context.
 *  @param fn The current function.
 *  @param block The current block.
 *  @param node The node containing the struct.
 *  @param struct_name The name of the struct.
 *  @param st The literal's expected struct definition.
 *  @param st_type The literal's expected struct type.
 *  @param get_address Whether to return the address of the struct.
 *  @param pr The parse result for the array.
 */
bool FormInitialiserStructParse(Units *units, Function *fn,
                                llvm::BasicBlock *block, Node *node,
                                const char *struct_name, Struct *st,
                                Type *st_type, bool get_address,
                                ParseResult *pr);
}

#endif
