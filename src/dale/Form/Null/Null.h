#ifndef DALE_FORM_NULL
#define DALE_FORM_NULL

namespace dale
{
namespace Form
{
namespace Null
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
