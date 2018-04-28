#ifndef FORM_UTILS
#define FORM_UTILS

#include <vector>

#include "../../Function/Function.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Units/Units.h"
#include "../../llvm_Function.h"
#include "../Type/Type.h"

namespace dale {
/*! Link a list of variables to the corresponding LLVM function
 * arguments.
 *  @param vars The variables.
 *  @param llvm_fn The LLVM function.
 *
 *  The variable names are used as the LLVM argument names, and
 *  the LLVM argument values are used as the variable values.
 */
void linkVariablesToFunction(std::vector<Variable *> *vars,
                             llvm::Function *llvm_fn);
/*! Create a GEP instruction.
 *  @param value The value to be indexed into.
 *  @param indices The indices for indexing.
 *  @param type The pointee type (optional).
 */
llvm::Instruction *createGEP(llvm::Value *value,
                             llvm::ArrayRef<llvm::Value *> indices,
                             llvm::Type *type = NULL);
/*! Create a constant GEP instruction.
 *  @param value The value to be indexed into.
 *  @param indices The indices for indexing.
 *  @param type The pointee type (optional).
 */
llvm::Constant *createConstantGEP(llvm::Constant *value,
                                  llvm::ArrayRef<llvm::Value *> indices,
                                  llvm::Type *type = NULL);
}

#endif
