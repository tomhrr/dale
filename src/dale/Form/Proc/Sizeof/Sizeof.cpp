#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../Type/Type.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcSizeofParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->units->top()->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("sizeof", node, 1, -1)) {
        return false;
    }

    symlist *lst = node->list;

    /* Get the type to which it is being cast. */

    Node *thing = (*lst)[1];
    thing = gen->units->top()->mp->parseOptionalMacroCall(thing);
    if (!thing) {
        return false;
    }

    Type *type = FormTypeParse(gen, (*lst)[1], false, false);

    if (!type) {
        ctx->er->popLastError();

        int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);

        ParseResult expr_res;
        bool res =
            FormProcInstParse(gen, 
                fn, block, (*lst)[1], true, false, NULL, &expr_res
            );

        if (!res) {
            ctx->er->popErrors(error_count);

            bool res =
                FormProcInstParse(gen, 
                    fn, block, (*lst)[1], false, false, NULL, &expr_res
                );
            if (!res) {
                return false;
            }
            type  = expr_res.type;
            block = expr_res.block;

            ParseResult temp;
            res = Operation::Destruct(ctx, &expr_res, &temp);
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
            bool res = Operation::Destruct(ctx, &expr_res, &temp);
            if (!res) {
                return false;
            }
            block = temp.block;
        }
    }


    bool res = Operation::Sizeof(ctx, block, type, pr);

    return res;
}
}
