#ifndef DALE_FORM_PROC_GETDNODES
#define DALE_FORM_PROC_GETDNODES

namespace dale
{
namespace Form
{
namespace Proc
{
namespace GetDNodes
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
