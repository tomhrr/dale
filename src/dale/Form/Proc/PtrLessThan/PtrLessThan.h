#ifndef DALE_FORM_PROC_PTRLESSTHAN
#define DALE_FORM_PROC_PTRLESSTHAN

namespace dale
{
bool
FormProcPtrLessThanParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
