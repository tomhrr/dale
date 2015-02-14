#ifndef DALE_FORM_PROC_PTREQUALS
#define DALE_FORM_PROC_PTREQUALS

namespace dale
{
bool
FormProcPtrEqualsParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
