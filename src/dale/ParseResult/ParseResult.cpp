#include "ParseResult.h"

namespace dale
{
ParseResult::ParseResult()
{
    block = NULL;
    type = NULL;
    value = NULL;
    treat_as_terminator = 0;
    do_not_destruct = 0;
    do_not_copy_with_setf = 0;
    freshly_copied = 0;
}

ParseResult::ParseResult(
    llvm::BasicBlock *new_block,
    Element::Type *new_type,
    llvm::Value *new_value
)
{
    treat_as_terminator = 0;
    do_not_destruct = 0;
    do_not_copy_with_setf = 0;
    freshly_copied = 0;
    block = new_block;
    type  = new_type;
    value = new_value;
}

ParseResult::~ParseResult()
{
}

int ParseResult::copyTo(ParseResult *x)
{
    x->treat_as_terminator =treat_as_terminator;
    x->do_not_destruct =do_not_destruct;
    x->do_not_copy_with_setf = do_not_copy_with_setf;
    x->block =block;
    x->type =type;
    x->value =value;
    x->freshly_copied = freshly_copied;
    return 1;
}
}
