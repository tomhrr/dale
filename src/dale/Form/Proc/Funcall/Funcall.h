#ifndef DALE_FORM_PROC_FUNCALL
#define DALE_FORM_PROC_FUNCALL

namespace dale
{
bool
FormProcFuncallParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
