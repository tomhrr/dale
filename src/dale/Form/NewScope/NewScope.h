#ifndef DALE_FORM_NEWSCOPE
#define DALE_FORM_NEWSCOPE

namespace dale
{
namespace Form
{
namespace NewScope
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
