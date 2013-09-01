#ifndef DALE_FORM_PTRADD
#define DALE_FORM_PTRADD

namespace dale
{
namespace Form
{
namespace PtrAdd
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
