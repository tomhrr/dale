#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../Type/Type.h"
#include "../../Literal/Array/Array.h"
#include "../../Literal/Integer/Integer.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcArrayOfParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    assert(node->list && "must receive a list!");

    symlist *lst = node->list;

    if (!ctx->er->assertArgNums("array-of", node, 3, -1)) {
        return false;
    }

    Node *newnum = units->top()->mp->parsePotentialMacroCall((*lst)[1]);
    if (!newnum) {
        return false;
    }

    int size = FormLiteralIntegerParse(newnum, ctx->er);
    if (size == -1) {
        return false;
    }

    Type *type = FormTypeParse(units, (*lst)[2], false, false);
    if (!type) {
        return false;
    }

    Type *arrtype = ctx->tr->getArrayType(type, size);

    int size2;
    return
        FormLiteralArrayParse(
            units,
            fn,
            block,
            ((*lst)[3]),
            "array literal",
            arrtype,
            get_address,
            &size2,
            pr
        );
}
}
