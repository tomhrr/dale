#include "Cast.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Type/Type.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcCastParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("cast", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *value_node = (*lst)[1];
    Node *type_node = (*lst)[2];

    ParseResult value_pr;
    bool res = FormProcInstParse(units, fn, block, value_node, false,
                                 false, NULL, &value_pr);
    if (!res) {
        return false;
    }
    llvm::Type *llvm_type =
        ctx->toLLVMType(value_pr.type, value_node, false);
    if (!llvm_type) {
        return false;
    }

    /* It is allowable to cast to a bitfield type, because there's no
     * other way to set a bitfield value. */

    Type *type = FormTypeParse(units, type_node, false, true);
    if (!type) {
        return false;
    }

    if (value_pr.type->isEqualTo(type)) {
        value_pr.copyTo(pr);
        return true;
    }

    ParseResult cast_pr;
    res = Operation::Cast(ctx, value_pr.block, value_pr.getValue(ctx),
                          value_pr.type, type, node, 0, &cast_pr);
    if (!res) {
        return false;
    }

    value_pr.block = cast_pr.block;
    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &value_pr, &destruct_pr);
    if (!res) {
        return false;
    }

    pr->set(destruct_pr.block, cast_pr.type, cast_pr.getValue(ctx));
    return true;
}
}
