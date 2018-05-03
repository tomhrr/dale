#include "ArrayOf.h"

#include <vector>

#include "../../../Form/TopLevel/GlobalVariable/GlobalVariable.h"
#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Initialiser/Array/Array.h"
#include "../../Type/Type.h"
#include "../../Value/Value.h"

namespace dale {
bool FormProcArrayOfParse(Units *units, Function *fn,
                          llvm::BasicBlock *block, Node *node,
                          bool get_address, bool prefixed_with_core,
                          ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("array-of", node, 3, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *size_node = (*lst)[1];
    Node *type_node = (*lst)[2];
    Node *data_node = (*lst)[3];

    Type *type = FormTypeParse(units, type_node, false, false);
    if (!type) {
        return false;
    }

    llvm::Constant *size_value = NULL;
    int unused_size;
    size_value = FormValueParse(units, ctx->tr->type_int, size_node,
                                &unused_size);
    if (!size_value) {
        return false;
    }
    llvm::ConstantInt *size_value_int =
        llvm::dyn_cast<llvm::ConstantInt>(size_value);
    if (!size_value_int) {
        Error *e = new Error(ErrorInst::UnableToParseIntegerNoString,
                             size_node);
        ctx->er->addError(e);
        return false;
    }
    int size = size_value_int->getZExtValue();

    Type *array_type = ctx->tr->getArrayType(type, size);

    return FormInitialiserArrayParse(units, fn, block, data_node,
                                     array_type, get_address,
                                     &unused_size, pr);
}
}
