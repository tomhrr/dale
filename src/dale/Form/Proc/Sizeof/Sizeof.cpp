#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../Type/Type.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Sizeof
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

    if (!ctx->er->assertArgNums("sizeof", node, 1, -1)) {
        return false;
    }

    symlist *lst = node->list;

    /* Get the type to which it is being cast. */

    Node *thing = (*lst)[1];
    thing = gen->parseOptionalMacroCall(thing);
    if (!thing) {
        return false;
    }

    Element::Type *type = Form::Type::parse(gen, (*lst)[1], false, false);

    if (!type) {
        ctx->er->popLastError();

        int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);

        ParseResult expr_res;
        bool res =
            gen->parseFunctionBodyInstr(
                fn, block, (*lst)[1], true, NULL, &expr_res
            );

        if (!res) {
            ctx->er->popErrors(error_count);

            bool res =
                gen->parseFunctionBodyInstr(
                    fn, block, (*lst)[1], false, NULL, &expr_res
                );
            if (!res) {
                return false;
            }
            type  = expr_res.type;
            block = expr_res.block;

            ParseResult temp;
            res = gen->destructIfApplicable(&expr_res, NULL, &temp);
            if (!res) {
                return false;
            }
            block = temp.block;
        } else {
            type =
                (expr_res.type->points_to)
                ? expr_res.type->points_to
                : expr_res.type;
            block = expr_res.block;
            ParseResult temp;
            bool res = gen->destructIfApplicable(&expr_res, NULL, &temp);
            if (!res) {
                return false;
            }
            block = temp.block;
        }
    }


    bool res = Operation::Sizeof::execute(ctx, block, type, pr);

    return res;
}
}
}
}
}
