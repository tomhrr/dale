#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../Type/Type.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcCastParse(Units *units, Function *fn, llvm::BasicBlock *block,
                  Node *node, bool get_address, bool prefixed_with_core,
                  ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("cast", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *value_node = (*lst)[1];
    Node *type_node  = (*lst)[2];

    ParseResult pr_value;
    bool res = FormProcInstParse(units, fn, block, value_node, false,
                                 false, NULL, &pr_value);
    if (!res) {
        return false;
    }

    /* It is allowable to cast to a bitfield type, because there's no
     * other way to set a bitfield value. */

    Type *type = FormTypeParse(units, type_node, false, true);
    if (!type) {
        return false;
    }

    if (pr_value.type->isEqualTo(type)) {
        pr_value.copyTo(pr);
        return true;
    }

    ParseResult pr_cast;
    res = Operation::Cast(ctx, pr_value.block, pr_value.value,
                          pr_value.type, type, node, 0, &pr_cast);
    if (!res) {
        return false;
    }

    pr_value.block = pr_cast.block;
    ParseResult pr_destruct;
    res = Operation::Destruct(ctx, &pr_value, &pr_destruct);
    if (!res) {
        return false;
    }

    pr->set(pr_destruct.block, pr_cast.type, pr_cast.value);
    return true;
}
}
