#ifndef DALE_FORM_RETURN
#define DALE_FORM_RETURN

namespace dale
{
namespace Form
{
namespace Return
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
