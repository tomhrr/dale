#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcFuncallParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    assert(node->list && "must receive a list!");

    /* (funcall <pointer-to-function> <arg1> <arg2> ...) */

    if (!ctx->er->assertArgNums("funcall", node, 1, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;

    ParseResult fn_ptr;
    bool res =
        FormProcInstParse(units, 
            fn, block, (*lst)[1], get_address, false, NULL, &fn_ptr
        );
    if (!res) {
        return false;
    }

    if (!fn_ptr.type->points_to
            || !fn_ptr.type->points_to->is_function) {
        std::string temp;
        fn_ptr.type->toString(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            "funcall", "fn pointer", "1", temp.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    res = units->top()->fp->parseFunctionPointerCall(
               fn,
               node,
               &fn_ptr,
               2,
               NULL,
               pr
           );
    return res;
}
}
