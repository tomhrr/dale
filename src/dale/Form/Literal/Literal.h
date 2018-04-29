#ifndef DALE_FORM_LITERAL
#define DALE_FORM_LITERAL

#include "../../Context/Context.h"
#include "../../Node/Node.h"
#include "../../Units/Units.h"

namespace dale {
void FormIntegerLiteralParse(Context *ctx, Type *wanted_type,
                             llvm::BasicBlock *block, Token *t,
                             ParseResult *pr);
void FormFloatingPointLiteralParse(Context *ctx, Type *wanted_type,
                                   llvm::BasicBlock *block, Token *t,
                                   ParseResult *pr);
bool FormStringLiteralParse(Units *units, Context *ctx,
                            llvm::BasicBlock *block, Node *node,
                            ParseResult *pr);
void FormBoolLiteralParse(Context *ctx, llvm::BasicBlock *block, Node *node,
                          ParseResult *pr);
void FormCharLiteralParse(Context *ctx, llvm::BasicBlock *block, Node *node,
                          ParseResult *pr);
bool FormLiteralParse(Units *units, Type *type, Node *node, int *size,
                      ParseResult *pr);
}

#endif
