#include "NewScope.h"

#include <string>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/CloseScope/CloseScope.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Do/Do.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcNewScopeParse(Units *units, Function *fn,
                           llvm::BasicBlock *block, Node *node,
                           bool get_address, bool prefixed_with_core,
                           ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("new-scope", node, 1, -1)) {
        return false;
    }

    ctx->activateAnonymousNamespace();
    std::string ns_name = ctx->ns()->name;

    bool res = FormProcDoParse(units, fn, block, node, get_address,
                               prefixed_with_core, pr);

    Operation::CloseScope(ctx, fn, block, NULL, false);
    ctx->deactivateNamespace(ns_name.c_str());

    return res;
}
}
