#ifndef DALE_FORM_ALIGNMENTOF
#define DALE_FORM_ALIGNMENTOF

namespace dale
{
namespace Form
{
namespace Alignmentof
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
