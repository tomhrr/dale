#ifndef DALE_FORM_ARRAYOF
#define DALE_FORM_ARRAYOF

namespace dale
{
namespace Form
{
namespace ArrayOf
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
