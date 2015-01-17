#ifndef DALE_FORM_PROC_DEF
#define DALE_FORM_PROC_DEF

namespace dale
{
bool
FormProcDefParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
