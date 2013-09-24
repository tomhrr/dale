#ifndef DALE_FORM_PROC_FUNCALL
#define DALE_FORM_PROC_FUNCALL

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Funcall
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr);
}
}
}
}

#endif
