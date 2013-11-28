#ifndef DALE_FORM_PROC_INST
#define DALE_FORM_PROC_INST

#include "../../../Generator/Generator.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Inst
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           Element::Type *wanted_type,
           ParseResult *pr,
           bool no_copy = false);
}
}
}
}

#endif
