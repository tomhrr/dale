#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Null
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

    if (!ctx->er->assertArgNums("null", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;

    /* Take the second value, parseMacroCall, see if it's a list,
     * if it is a list and the form is : or $, then show an error
     * about that value never being null. */

    Node *arg = (*lst)[1];
    if (arg->is_list) {
        arg = gen->parseOptionalMacroCall(arg);
        if (!arg) {
            return false;
        }
        if (arg->is_list) {
            Node *first = arg->list->at(0);
            if (first->is_token) {
                const char *v = first->token->str_value.c_str();
                if ((!strcmp(v, ":")) || (!strcmp(v, "$"))) {
                    Error *e = new Error(
                        ErrorInst::Generator::ValueWillNeverBeNull,
                        arg
                    );
                    ctx->er->addError(e);
                    return false;
                }
            }
        }
    }

    ParseResult pr_value;
    bool res = 
        gen->parseFunctionBodyInstr(fn, block, arg, false, NULL, &pr_value);

    if (!res) {
        return false;
    }
    if (!ctx->er->assertIsPointerType("null", arg, pr_value.type, "1")) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_value.block);
    llvm::Value *vres =
        builder.CreatePtrToInt(pr_value.value, ctx->nt->getNativeIntType());

    llvm::Value *icmpres = llvm::cast<llvm::Value>(
                               builder.CreateICmpEQ(vres,
                                       ctx->nt->getLLVMZero())
                           );

    pr->set(pr_value.block, ctx->tr->getBasicType(Type::Bool), icmpres);
    if (gen->hasRelevantDestructor(&pr_value)) {
        ParseResult temp;
        bool res = gen->destructIfApplicable(&pr_value, NULL, &temp);
        if (!res) {
            return false;
        }
        pr->block = temp.block;
    }

    return true;
}
}
}
}
