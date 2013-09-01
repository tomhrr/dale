#ifndef DALE_FORM_SREF
#define DALE_FORM_SREF

namespace dale
{
namespace Form
{
namespace Sref
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
