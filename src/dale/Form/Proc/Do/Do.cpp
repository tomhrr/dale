#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcDoParse(Units *units, Function *fn, llvm::BasicBlock *block,
                Node *node, bool get_address, bool prefixed_with_core,
                ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("do", node, 1, -1)) {
        return false;
    }

    for (std::vector<Node*>::iterator b = (node->list->begin() + 1),
                                      e = (node->list->end()   - 1);
            b != e;
            ++b) {
        ParseResult pr_local;
        bool res = FormProcInstParse(units, fn, block, *b, get_address,
                                     false, NULL, &pr_local);
        if (!res) {
            return false;
        }

        ParseResult pr_destruct;
        res = Operation::Destruct(ctx, &pr_local, &pr_destruct);
        if (!res) {
            return false;
        }
        block = pr_destruct.block;
    }

    return FormProcInstParse(units, fn, block,
                             *(node->list->end() - 1),
                             get_address, false, NULL, pr);
}
}
