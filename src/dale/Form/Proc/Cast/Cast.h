#ifndef DALE_FORM_PROC_CAST
#define DALE_FORM_PROC_CAST

namespace dale
{
bool
FormProcCastParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
