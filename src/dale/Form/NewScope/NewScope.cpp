#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace NewScope
{
bool execute(Generator *gen,
             Element::Function *fn,
             llvm::BasicBlock *block,
             Node *node,
             bool get_address,
             bool prefixed_with_core,
             ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    symlist *lst = node->list;

    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;

    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;

    pr->block = block;
    bool success = true;
    while (node_iter != lst->end()) {
        ParseResult local_pr;
        bool res = gen->parseFunctionBodyInstr(
                       fn, pr->block, (*node_iter), get_address, NULL,
                       &local_pr
                   );
        ++node_iter;
        if (!res) {
            success = false;
            continue;
        }

        if (node_iter != lst->end()) {
            ParseResult pr_value;
            bool res = 
                gen->destructIfApplicable(&local_pr, NULL, &pr_value);
            if (!res) {
                return false;
            }
            pr->block = pr_value.block;
        } else {
            local_pr.copyTo(pr);
        }
    }

    gen->scopeClose(fn, block, NULL);
    ctx->deactivateNamespace(anon_name.c_str());

    return success;
}
}
}
}
