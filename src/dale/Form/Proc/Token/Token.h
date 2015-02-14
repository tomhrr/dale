#ifndef DALE_FORM_PROC_TOKEN
#define DALE_FORM_PROC_TOKEN

namespace dale
{
bool
FormProcTokenParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           Type *wanted_type,
           ParseResult *pr);
}

#endif
