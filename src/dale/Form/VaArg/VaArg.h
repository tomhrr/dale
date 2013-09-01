#ifndef DALE_FORM_VAARG
#define DALE_FORM_VAARG

namespace dale
{
namespace Form
{
namespace VaArg
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
