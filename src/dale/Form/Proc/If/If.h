#ifndef DALE_FORM_PROC_IF
#define DALE_FORM_PROC_IF

namespace dale
{
bool
FormProcIfParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
