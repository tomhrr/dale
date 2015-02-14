#ifndef DALE_FORM_PROC_GOTO
#define DALE_FORM_PROC_GOTO

namespace dale
{
bool
FormProcGotoParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
