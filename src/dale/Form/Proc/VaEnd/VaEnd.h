#ifndef DALE_FORM_PROC_VAEND
#define DALE_FORM_PROC_VAEND

namespace dale
{
bool
FormProcVaEndParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
