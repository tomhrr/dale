#ifndef DALE_FORM_PTRSUBTRACT
#define DALE_FORM_PTRSUBTRACT

namespace dale
{
namespace Form
{
namespace PtrSubtract
{
bool execute(Generator *gen,
             Element::Function *fn,
             llvm::BasicBlock *block,
             Node *node,
             bool get_address,
             bool prefixed_with_core,
             ParseResult *pr);
}
}
}

#endif
