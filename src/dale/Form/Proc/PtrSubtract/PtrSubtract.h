#ifndef DALE_FORM_PROC_PTRSUBTRACT
#define DALE_FORM_PROC_PTRSUBTRACT

namespace dale
{
bool
FormProcPtrSubtractParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
