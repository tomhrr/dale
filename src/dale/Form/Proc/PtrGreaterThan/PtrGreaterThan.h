#ifndef DALE_FORM_PROC_PTRGREATERTHAN
#define DALE_FORM_PROC_PTRGREATERTHAN

namespace dale
{
namespace Form
{
namespace Proc
{
namespace PtrGreaterThan
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}
}
}
}

#endif
