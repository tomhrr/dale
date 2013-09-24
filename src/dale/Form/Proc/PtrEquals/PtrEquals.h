#ifndef DALE_FORM_PROC_PTREQUALS
#define DALE_FORM_PROC_PTREQUALS

namespace dale
{
namespace Form
{
namespace Proc
{
namespace PtrEquals
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
