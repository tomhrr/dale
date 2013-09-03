#ifndef DALE_FORM_CAST
#define DALE_FORM_CAST

namespace dale
{
namespace Form
{
namespace Cast
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
