#ifndef DALE_FORM_PROC_SIZEOF
#define DALE_FORM_PROC_SIZEOF

namespace dale
{
bool
FormProcSizeofParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
