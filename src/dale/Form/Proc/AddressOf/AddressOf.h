#ifndef DALE_FORM_PROC_ADDRESSOF
#define DALE_FORM_PROC_ADDRESSOF

namespace dale
{
bool
FormProcAddressOfParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
