#include "Move.h"

namespace dale
{
namespace Operation
{
bool
Move(Context *ctx, Function *fn, ParseResult *pr, ParseResult *ret_pr)
{
    pr->copyTo(ret_pr);
    ret_pr->value_is_lvalue = false;
    return true;
}
}
}
