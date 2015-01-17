#ifndef DALE_FORM_PROC_NEWSCOPE
#define DALE_FORM_PROC_NEWSCOPE

namespace dale
{
bool
FormProcNewScopeParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
