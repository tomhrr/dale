#ifndef DALE_FORM_VASTART
#define DALE_FORM_VASTART

namespace dale
{
namespace Form
{
namespace VaStart
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
