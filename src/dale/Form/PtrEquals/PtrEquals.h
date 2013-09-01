#ifndef DALE_FORM_PTREQUALS
#define DALE_FORM_PTREQUALS

namespace dale
{
namespace Form
{
namespace PtrEquals
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
