#ifndef DALE_FORM_PROC_NULLPTR
#define DALE_FORM_PROC_NULLPTR

namespace dale
{
bool
FormProcNullPtrParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
