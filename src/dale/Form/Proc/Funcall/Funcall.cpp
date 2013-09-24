#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Funcall
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    /* (funcall <pointer-to-function> <arg1> <arg2> ...) */

    if (!ctx->er->assertArgNums("funcall", node, 1, -1)) {
        return false;
    }

    symlist *lst = node->list;

    ParseResult fn_ptr;
    bool res =
        gen->parseFunctionBodyInstr(
            fn, block, (*lst)[1], get_address, NULL, &fn_ptr
        );
    if (!res) {
        return false;
    }

    if (!fn_ptr.type->points_to
            || !fn_ptr.type->points_to->is_function) {
        std::string temp;
        fn_ptr.type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            "funcall", "fn pointer", "1", temp.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    res = gen->parseFuncallInternal(
               fn,
               node,
               get_address,
               &fn_ptr,
               2,
               NULL,
               pr
           );
    return res;
}
}
}
}
}
