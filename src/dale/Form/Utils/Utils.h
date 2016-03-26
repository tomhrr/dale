#ifndef FORM_UTILS
#define FORM_UTILS

#include "../../Units/Units.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Function/Function.h"
#include "../Type/Type.h"
#include "../../llvm_Function.h"

namespace dale
{
/*! Link a list of variables to the corresponding LLVM function arguments.
 *  @param vars The variables.
 *  @param llvm_fn The LLVM function.
 *
 *  The variable names are used as the LLVM argument names, and
 *  the LLVM argument values are used as the variable values.
 */
void linkVariablesToFunction(std::vector<Variable *> *vars,
                             llvm::Function *llvm_fn);
}

#endif
