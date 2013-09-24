#ifndef DALE_FORM_PROC_DO
#define DALE_FORM_PROC_DO

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Do
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
