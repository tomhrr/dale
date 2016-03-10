#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Move/Move.h"
#include "../../Type/Type.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcMoveParse(Units *units, Function *fn, llvm::BasicBlock *block,
                  Node *node, bool get_address, bool prefixed_with_core,
                  ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("cast", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *value_node = (*lst)[1];

    ParseResult value_pr;
    bool res = FormProcInstParse(units, fn, block, value_node, false,
                                 false, NULL, &value_pr, true);
    if (!res) {
        return false;
    }

    res = Operation::Move(ctx, fn, &value_pr, pr);
    return res;
}
}
