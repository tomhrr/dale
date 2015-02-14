#ifndef DALE_FORM_PROC_ALIGNMENTOF
#define DALE_FORM_PROC_ALIGNMENTOF

namespace dale
{
bool
FormProcAlignmentOfParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
