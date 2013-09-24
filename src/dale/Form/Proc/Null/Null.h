#ifndef DALE_FORM_PROC_NULL
#define DALE_FORM_PROC_NULL

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Null
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
