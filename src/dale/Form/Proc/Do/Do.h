#ifndef DALE_FORM_PROC_DO
#define DALE_FORM_PROC_DO

namespace dale
{
bool
FormProcDoParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
