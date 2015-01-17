#ifndef DALE_FORM_PROC_PTRGREATERTHAN
#define DALE_FORM_PROC_PTRGREATERTHAN

namespace dale
{
bool
FormProcPtrGreaterThanParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
