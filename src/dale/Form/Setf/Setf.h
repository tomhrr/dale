#ifndef DALE_FORM_SETF
#define DALE_FORM_SETF

namespace dale
{
namespace Form
{
namespace Setf
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
