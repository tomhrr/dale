#ifndef DALE_FORM_PROC_VAARG
#define DALE_FORM_PROC_VAARG

namespace dale
{
namespace Form
{
namespace Proc
{
namespace VaArg
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
