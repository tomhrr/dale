#ifndef DALE_FORM_NULLPTR
#define DALE_FORM_NULLPTR

namespace dale
{
namespace Form
{
namespace NullPtr
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
