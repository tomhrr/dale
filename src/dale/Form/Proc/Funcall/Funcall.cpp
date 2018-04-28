#include "Funcall.h"

#include <string>
#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcFuncallParse(Units *units, Function *fn,
                          llvm::BasicBlock *block, Node *node,
                          bool get_address, bool prefixed_with_core,
                          ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("funcall", node, 1, -1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *fp_node = (*lst)[1];

    ParseResult fp_pr;
    bool res = FormProcInstParse(units, fn, block, fp_node, get_address,
                                 false, NULL, &fp_pr);
    if (!res) {
        return false;
    }

    if (!fp_pr.type->points_to || !fp_pr.type->points_to->is_function) {
        std::string type_str;
        fp_pr.type->toString(&type_str);
        Error *e = new Error(IncorrectArgType, fp_node, "funcall",
                             "function pointer", "1", type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    return units->top()->fp->parseFunctionPointerCall(fn, node, &fp_pr,
                                                      2, NULL, pr);
}
}
