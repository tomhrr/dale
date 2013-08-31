#ifndef DALE_FORM_IF
#define DALE_FORM_IF

namespace dale
{
namespace Form
{
namespace If
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
