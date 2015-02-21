#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Alignmentof/Alignmentof.h"
#include "../../Type/Type.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcAlignmentOfParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    assert(node->list && "must receive a list!");

    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("alignmentof", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;

    Node *mytype = (*lst)[1];
    mytype = units->top()->mp->parseOptionalMacroCall(mytype);
    if (!mytype) {
        return false;
    }
    Type *type = FormTypeParse(units, mytype, false,
                                         false);
    if (!type) {
        return false;
    }

    bool res = Operation::Alignmentof(ctx, block, type, pr);
    return res;
}
}
