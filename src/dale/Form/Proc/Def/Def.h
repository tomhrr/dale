#ifndef DALE_FORM_PROC_DEF
#define DALE_FORM_PROC_DEF

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Def
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
