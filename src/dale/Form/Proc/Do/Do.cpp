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
FormProcDoParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = units->top()->ctx;
    assert(node->list && "must receive a list!");
    if (!ctx->er->assertArgNums("do", node, 1, -1)) {
        return false;
    }

    pr->block = block;

    for (std::vector<Node*>::iterator b = (node->list->begin() + 1),
                                      e = node->list->end();
            b != e;
            ++b) {
        bool is_last = ((b + 1) == e);

        ParseResult local_pr;
        ParseResult *used_pr = (is_last ? pr : &local_pr);
        bool res = FormProcInstParse(units, 
                       fn, pr->block, *b, get_address, 
                       false, NULL,
                       used_pr
                   );
        if (!res) {
            return false;
        }

        pr->block = used_pr->block;
        if (!is_last) {
            ParseResult pr_value;
            bool res = Operation::Destruct(ctx, &local_pr, &pr_value);
            if (!res) {
                return false;
            }
            pr->block = pr_value.block;
        }
    }

    return true;
}
}
