#ifndef DALE_FORM_PROC_LABEL
#define DALE_FORM_PROC_LABEL

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Label
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
