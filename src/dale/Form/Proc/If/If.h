#ifndef DALE_FORM_PROC_IF
#define DALE_FORM_PROC_IF

namespace dale
{
namespace Form
{
namespace Proc
{
namespace If
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
