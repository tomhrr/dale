#ifndef DALE_FORM_PROC_NULL
#define DALE_FORM_PROC_NULL

namespace dale
{
bool
FormProcNullParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
