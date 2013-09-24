#ifndef DALE_FORM_PROC_SIZEOF
#define DALE_FORM_PROC_SIZEOF

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Sizeof
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
