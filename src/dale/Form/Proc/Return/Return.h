#ifndef DALE_FORM_PROC_RETURN
#define DALE_FORM_PROC_RETURN

namespace dale
{
bool
FormProcReturnParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
