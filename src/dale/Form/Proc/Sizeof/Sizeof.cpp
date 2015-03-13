#include "../../../Units/Units.h"
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
FormProcSizeofParse(Units *units, Function *fn, llvm::BasicBlock *block,
                    Node *node, bool get_address, bool prefixed_with_core,
                    ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("sizeof", node, 1, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *type_node = (*lst)[1];

    type_node = units->top()->mp->parsePotentialMacroCall(type_node);
    if (!type_node) {
        return false;
    }

    /* The process here is: try to parse a type; failing that, try to
     * parse the form with get_address turned on; failing that, try to
     * parse the form with get_address turned off. */

    int error_count_begin = ctx->er->getErrorTypeCount(ErrorType::Error);
    Type *type = FormTypeParse(units, type_node, false, false);

    if (!type) {
        ctx->er->popErrors(error_count_begin);
        error_count_begin = ctx->er->getErrorTypeCount(ErrorType::Error);

        ParseResult pr_expr;
        bool res = FormProcInstParse(units, fn, block, type_node,
                                     true, false, NULL, &pr_expr);

        if (!res) {
            ctx->er->popErrors(error_count_begin);

            res = FormProcInstParse(units, fn, block, type_node,
                                    false, false, NULL, &pr_expr);
            if (!res) {
                return false;
            }
            type  = pr_expr.type;
            block = pr_expr.block;
        } else {
            type  = pr_expr.type->points_to;
            block = pr_expr.block;
        }

        ParseResult pr_destruct;
        res = Operation::Destruct(ctx, &pr_expr, &pr_destruct);
        if (!res) {
            return false;
        }
        block = pr_destruct.block;
    }

    return Operation::Sizeof(ctx, block, type, pr);
}
}
