#ifndef DALE_FORM_PROC_AREF
#define DALE_FORM_PROC_AREF

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Aref
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
