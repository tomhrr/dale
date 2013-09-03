#ifndef DALE_FORM_DO
#define DALE_FORM_DO

namespace dale
{
namespace Form
{
namespace Do
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
