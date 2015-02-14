#ifndef DALE_FORM_PROC_GETDNODES
#define DALE_FORM_PROC_GETDNODES

namespace dale
{
bool
FormProcGetDNodesParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}

#endif
