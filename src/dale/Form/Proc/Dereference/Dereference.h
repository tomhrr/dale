#ifndef DALE_FORM_PROC_DEREFERENCE
#define DALE_FORM_PROC_DEREFERENCE

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Dereference
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
