#ifndef DALE_FORM_OFFSETOF
#define DALE_FORM_OFFSETOF

namespace dale
{
namespace Form
{
namespace Offsetof
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
