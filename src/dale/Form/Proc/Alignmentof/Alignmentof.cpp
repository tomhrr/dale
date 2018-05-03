#include "Alignmentof.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Alignmentof/Alignmentof.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Type/Type.h"

namespace dale {
bool FormProcAlignmentOfParse(Units *units, Function *fn,
                              llvm::BasicBlock *block, Node *node,
                              bool get_address, bool prefixed_with_core,
                              ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("alignmentof", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *type_node = (*lst)[1];

    type_node = units->top()->mp->parsePotentialMacroCall(type_node);
    if (!type_node) {
        return false;
    }

    Type *type = FormTypeParse(units, type_node, false, false);
    if (!type) {
        return false;
    }

    return Operation::Alignmentof(ctx, block, type, pr);
}
}
