#ifndef DALE_FORM_PROC_LABEL
#define DALE_FORM_PROC_LABEL

namespace dale
{
bool
FormProcLabelParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
