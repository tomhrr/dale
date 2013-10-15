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
    assert(node->list && "must receive a list!");
    Context *ctx = gen->ctx;

    if (!ctx->er->assertArgNums("do", node, 1, -1)) {
        return false;
    }

    symlist *lst = node->list;
    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;

    pr->block = block;
    while (node_iter != lst->end()) {
        ParseResult local_pr;
        bool res = Form::Proc::Inst::parse(gen, 
                       fn, pr->block, (*node_iter), get_address, 
                       false, NULL,
                       &local_pr
                   );
        if (!res) {
            return false;
        }
        ++node_iter;

        local_pr.copyTo(pr);
        if (node_iter != lst->end()) {
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
