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
FormProcArrayOfParse(Units *units, Function *fn, llvm::BasicBlock *block,
                     Node *node, bool get_address, bool prefixed_with_core,
                     ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("array-of", node, 3, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *size_node = (*lst)[1];
    Node *type_node = (*lst)[2];
    Node *data_node = (*lst)[3];

    Node *array_size = units->top()->mp->parsePotentialMacroCall(size_node);
    if (!array_size) {
        return false;
    }

    int size = FormLiteralIntegerParse(array_size, ctx->er);
    if (size == -1) {
        return false;
    }

    Type *type = FormTypeParse(units, type_node, false, false);
    if (!type) {
        return false;
    }

    Type *array_type = ctx->tr->getArrayType(type, size);

    int unused_size;
    return FormLiteralArrayParse(units, fn, block, data_node,
                                 "array literal", array_type,
                                 get_address, &unused_size, pr);
}
}
