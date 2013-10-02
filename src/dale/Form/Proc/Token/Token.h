#ifndef DALE_FORM_PROC_TOKEN
#define DALE_FORM_PROC_TOKEN

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Token
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           Element::Type *wanted_type,
           ParseResult *pr);
}
}
}
}

#endif
