#ifndef DALE_FORM_PROC_ARRAYOF
#define DALE_FORM_PROC_ARRAYOF

namespace dale
{
namespace Form
{
namespace Proc
{
namespace ArrayOf
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
