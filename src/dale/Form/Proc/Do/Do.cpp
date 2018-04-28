#include "Do.h"

#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

namespace dale {
bool FormProcDoParse(Units *units, Function *fn,
                     llvm::BasicBlock *block, Node *node,
                     bool get_address, bool prefixed_with_core,
                     ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (node->list->size() == 1) {
        pr->set(block, ctx->tr->type_void, NULL);
        return true;
    }

    for (std::vector<Node *>::iterator b = (node->list->begin() + 1),
                                       e = (node->list->end() - 1);
         b != e; ++b) {
        ParseResult local_pr;
        bool res = FormProcInstParse(units, fn, block, *b, get_address,
                                     false, NULL, &local_pr);
        if (!res) {
            return false;
        }

        ParseResult destruct_pr;
        res = Operation::Destruct(ctx, &local_pr, &destruct_pr);
        if (!res) {
            return false;
        }
        block = destruct_pr.block;
    }

    return FormProcInstParse(units, fn, block, *(node->list->end() - 1),
                             get_address, false, NULL, pr);
}
}
