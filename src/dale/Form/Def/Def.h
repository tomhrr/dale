#ifndef DALE_FORM_DEF
#define DALE_FORM_DEF

namespace dale
{
namespace Form
{
namespace Def
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
