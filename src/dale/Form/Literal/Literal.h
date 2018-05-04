#ifndef DALE_FORM_LITERAL
#define DALE_FORM_LITERAL

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../Units/Units.h"

namespace dale {
/*! Convert an integer token into a parse result.
 *  @param ctx The context.
 *  @param wanted_type The type of the integer.
 *  @param block The current basic block.
 *  @param t The integer token.
 *  @param pr The parse result for the integer.
 */
void FormIntegerLiteralParse(Context *ctx, Type *wanted_type,
                             llvm::BasicBlock *block, Token *t,
                             ParseResult *pr);
/*! Convert a floating point token into a parse result.
 *  @param ctx The context.
 *  @param wanted_type The type of the floating point value.
 *  @param block The current basic block.
 *  @param t The floating point token.
 *  @param pr The parse result for the floating point value.
 */
void FormFloatingPointLiteralParse(Context *ctx, Type *wanted_type,
                                   llvm::BasicBlock *block, Token *t,
                                   ParseResult *pr);
/*! Convert a string literal token into a parse result.
 *  @param units The units context.
 *  @param ctx The context.
 *  @param block The current basic block.
 *  @param t The string literal node.
 *  @param pr The parse result for the string literal.
 */
void FormStringLiteralParse(Units *units, Context *ctx,
                            llvm::BasicBlock *block, Node *node,
                            ParseResult *pr);
/*! Convert a boolean string literal token into a parse result.
 *  @param ctx The context.
 *  @param block The current basic block.
 *  @param t The boolean literal node.
 *  @param pr The parse result for the boolean literal.
 */
void FormBoolLiteralParse(Context *ctx, llvm::BasicBlock *block,
                          Node *node, ParseResult *pr);
/*! Convert a char literal token into a parse result.
 *  @param ctx The context.
 *  @param block The current basic block.
 *  @param t The char literal node.
 *  @param pr The parse result for the char literal.
 */
void FormCharLiteralParse(Context *ctx, llvm::BasicBlock *block,
                          Node *node, ParseResult *pr);
/*! Convert any type of literal into a parse result.
 *  @param units The units context.
 *  @param type The type of the literal.
 *  @param t The literal node.
 *  @param pr The parse result for the literal.
 *
 *  This can also be used to parse arrays and structs.
 */
bool FormLiteralParse(Units *units, Type *type, Node *node,
                      ParseResult *pr);
}

#endif
