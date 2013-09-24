#ifndef DALE_FORM_PROC_PTRADD
#define DALE_FORM_PROC_PTRADD

namespace dale
{
namespace Form
{
namespace Proc
{
namespace PtrAdd
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
