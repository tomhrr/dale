#include "ParseResult.h"

namespace dale
{
ParseResult::ParseResult()
{
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
    /* It is assumed that only type is owned by this object (i.e.
     * that block and value get cleaned up elsewhere). */
    //delete type;
    /*
    std::string type_str;
    type->toStringProper(&type_str);
    fprintf(stderr, "ParseResult destruction: "
                    "name: %s, type: %s\n",
                    type_str.c_str(),
                    value->getName().str().c_str());
    */
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
