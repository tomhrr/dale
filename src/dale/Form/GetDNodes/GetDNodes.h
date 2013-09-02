#ifndef DALE_FORM_GETDNODES
#define DALE_FORM_GETDNODES

namespace dale
{
namespace Form
{
namespace GetDNodes
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
