#ifndef DALE_FORM_LITERAL_ENUM
#define DALE_FORM_LITERAL_ENUM

#include "../../../Context/Context.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../Type/Type.h"

namespace dale {
/*! Parse an enum literal.
 *  @param units The units context.
 *  @param block The current block.
 *  @param node The node containing the enum.
 *  @param enum_obj The literal's expected enum definition.
 *  @param enum_type The literal's expected enum type.
 *  @param enum_st The literal's expected enum struct.
 *  @param get_address Whether to return the address of the enum.
 *  @param pr The parse result for the enum.
 */
bool FormLiteralEnumParse(Units *units, llvm::BasicBlock *block,
                          Node *node, Enum *enum_obj, Type *enum_type,
                          Struct *enum_st, bool get_address,
                          ParseResult *pr);
}

#endif
