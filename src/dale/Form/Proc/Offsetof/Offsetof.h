#ifndef DALE_FORM_PROC_OFFSETOF
#define DALE_FORM_PROC_OFFSETOF

namespace dale
{
bool
FormProcOffsetOfParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
