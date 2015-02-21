#ifndef DALE_FORM_PROC_INST
#define DALE_FORM_PROC_INST

#include "../../../Units/Units.h"

namespace dale
{
bool
FormProcInstParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           Type *wanted_type,
           ParseResult *pr,
           bool no_copy = false);
}

#endif
