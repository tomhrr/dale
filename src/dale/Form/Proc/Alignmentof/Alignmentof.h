#ifndef DALE_FORM_PROC_ALIGNMENTOF
#define DALE_FORM_PROC_ALIGNMENTOF

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Alignmentof
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
