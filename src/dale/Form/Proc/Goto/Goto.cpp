#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Goto
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

    assert(n->list && "parseGoto must receive a list!");

    if (!ctx->er->assertArgNums("goto", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;
    Node *lname = (*lst)[1];

    if (!ctx->er->assertArgIsAtom("goto", lname, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol("goto", lname, "1")) {
        return false;
    }

    Token *t = lname->token;

    Element::Label *mylabel = fn->getLabel(t->str_value.c_str());

    if (!mylabel) {
        /* Block does not exist - add to defgotos. Also add a
         * no-op instruction to the block, so that parseLabel does
         * not add an implicit branch from the previous block to
         * the new block (which it does if the previous block does
         * not end with a terminator, which it won't in this case
         * because the goto is deferred). */

        std::string *name = new std::string;
        name->append(t->str_value.c_str());

        DeferredGoto *dg = new DeferredGoto;
        dg->label_name = name;
        dg->ns = ctx->ns();
        dg->index = ctx->ns()->lv_index;
        dg->block_marker = block;
        Node *myn = new Node();
        node->copyTo(myn);
        dg->node = myn;

        if (block->size() == 0) {
            dg->marker         = NULL;
        } else {
            llvm::Instruction *tinstr = &(block->back());
            dg->marker = tinstr;
        }

        fn->defgotos->push_back(dg);

        llvm::IRBuilder<> builder(block);
        builder.CreateBitCast(
            llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0),
            ctx->nt->getNativeIntType()
        );
    } else {
        /* Get all the variables that exist within the current
         * scope and have an index greater than the label's index.
         * Add a destructIfApplicable call for each of these
         * variables. */
        std::vector<Element::Variable *> myvars;
        ctx->ns()->getVarsAfterIndex(mylabel->index, &myvars);
        ParseResult myp;
        myp.block = block;
        for (std::vector<Element::Variable *>::iterator
                b = myvars.begin(),
                e = myvars.end();
                b != e;
                ++b) {
            Element::Variable *v = (*b);
            myp.type  = v->type;
            llvm::IRBuilder<> builder(myp.block);
            /* hasRelevantDestructor does not depend on value
             * being set. */
            llvm::Value *myv = builder.CreateLoad(v->value);
            myp.value = myv;
            ParseResult temp;
            gen->destructIfApplicable(&myp, NULL, &temp);
            myp.block = temp.block;
        }
        block = myp.block;
        llvm::IRBuilder<> builder(block);
        llvm::BasicBlock *to_block = mylabel->block;
        builder.CreateBr(to_block);
    }

    pr->set(block, ctx->tr->getBasicType(Type::Int), 
            llvm::ConstantInt::get(ctx->nt->getNativeIntType(), 0));

    /* Ugh. If setf was defined for ints, the caller of this
     * function would copy the returned zero. This meant that the
     * branch instruction was no longer the final instruction in
     * the block. This meant that parseIf (amongst other things)
     * did not treat this as terminating. Fun stuff. */
    pr->do_not_copy_with_setf = 1;

    if (!mylabel) {
        pr->treat_as_terminator = 1;
    }
    /* todo: hopefully temporary. */
    pr->treat_as_terminator = 1;

    return true;
}
}
}
}
}
