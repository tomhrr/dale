#ifndef DALE_FORM_PROC_NEWSCOPE
#define DALE_FORM_PROC_NEWSCOPE

namespace dale
{
namespace Form
{
namespace Proc
{
namespace NewScope
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
