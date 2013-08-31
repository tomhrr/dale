#ifndef DALE_FORM_GOTO
#define DALE_FORM_GOTO

namespace dale
{
namespace Form
{
namespace Goto
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
