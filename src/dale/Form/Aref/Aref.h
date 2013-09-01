#ifndef DALE_FORM_AREF
#define DALE_FORM_AREF

namespace dale
{
namespace Form
{
namespace Aref
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
