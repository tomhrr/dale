#ifndef DALE_FORM_FUNCALL
#define DALE_FORM_FUNCALL

namespace dale
{
namespace Form
{
namespace Funcall
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
