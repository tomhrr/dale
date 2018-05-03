#include "Sizeof.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Type/Type.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcSizeofParse(Units *units, Function *fn,
                         llvm::BasicBlock *block, Node *node,
                         bool get_address, bool prefixed_with_core,
                         ParseResult *pr) {
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

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);
    Type *type = FormTypeParse(units, type_node, false, false);

    if (!type) {
        ctx->er->popErrors(error_count_begin);
        error_count_begin =
            ctx->er->getErrorTypeCount(ErrorType::Error);

        ParseResult expr_pr;
        bool res = FormProcInstParse(units, fn, block, type_node, true,
                                     false, NULL, &expr_pr);

        if (!res) {
            ctx->er->popErrors(error_count_begin);

            res = FormProcInstParse(units, fn, block, type_node, false,
                                    false, NULL, &expr_pr);
            if (!res) {
                return false;
            }
            type = expr_pr.type;
            block = expr_pr.block;
        } else {
            type = expr_pr.type->points_to;
            block = expr_pr.block;
        }

        ParseResult destruct_pr;
        res = Operation::Destruct(ctx, &expr_pr, &destruct_pr);
        if (!res) {
            return false;
        }
        block = destruct_pr.block;
    }

    llvm::Type *llvm_type = ctx->toLLVMType(type, node, false);
    if (!llvm_type) {
        return false;
    }

    size_t size = Operation::SizeofGet(units->top(), type);
    pr->set(
        block, ctx->tr->type_size,
        llvm::ConstantInt::get(
            ctx->toLLVMType(ctx->tr->type_size, NULL, false), size));

    return true;
}
}
