#ifndef DALE_FORM_SIZEOF
#define DALE_FORM_SIZEOF

namespace dale
{
namespace Form
{
namespace Sizeof
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
