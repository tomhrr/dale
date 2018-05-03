#include "Offsetof.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Offsetof/Offsetof.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Type/Type.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcOffsetOfParse(Units *units, Function *fn,
                           llvm::BasicBlock *block, Node *node,
                           bool get_address, bool prefixed_with_core,
                           ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("offsetof", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *struct_node = (*lst)[1];
    Node *member_node = (*lst)[2];

    struct_node =
        units->top()->mp->parsePotentialMacroCall(struct_node);
    if (!struct_node) {
        return false;
    }

    Type *type = FormTypeParse(units, struct_node, false, false);
    if (!type) {
        return false;
    }

    if (!type->struct_name.size()) {
        Error *e = new Error(UnexpectedElement, node, "struct",
                             "offsetof", "not a struct");
        ctx->er->addError(e);
        return false;
    }

    member_node =
        units->top()->mp->parsePotentialMacroCall(member_node);
    if (!member_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("offsetof", member_node, "2")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("offsetof", member_node, "2")) {
        return false;
    }

    return Operation::Offsetof(units->top(), block, type,
                               member_node->token->str_value.c_str(),
                               pr);
}
}
