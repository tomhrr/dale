#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../Inst/Inst.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Do
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;
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
        bool res = Form::Proc::Inst::parse(gen, 
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
            bool res = gen->destructIfApplicable(&local_pr, NULL, &pr_value);
            if (!res) {
                return false;
            }
            pr->block = pr_value.block;
        }
    }

    return true;
}
}
}
}
}
