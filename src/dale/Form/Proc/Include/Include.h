#ifndef DALE_FORM_PROC_INCLUDE
#define DALE_FORM_PROC_INCLUDE

#include "../../../Units/Units.h"

namespace dale {
bool FormProcIncludeParse(Units *units, Function *fn,
                          llvm::BasicBlock *block, Node *node,
                          bool get_address, bool prefixed_with_core,
                          ParseResult *pr);
}

#endif
