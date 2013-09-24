#ifndef DALE_FORM_PROC_PTRSUBTRACT
#define DALE_FORM_PROC_PTRSUBTRACT

namespace dale
{
namespace Form
{
namespace Proc
{
namespace PtrSubtract
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
