#ifndef DALE_FORM_PROC_VASTART
#define DALE_FORM_PROC_VASTART

namespace dale
{
bool
FormProcVaStartParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
