#ifndef DALE_FORM_VAEND
#define DALE_FORM_VAEND

namespace dale
{
namespace Form
{
namespace VaEnd
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
