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
FormProcNullParse(Units *units,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("null", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;

    /* Take the second value, parsePotentialMacroCall, see if it's a
     * list, if it is a list and the form is : or $, then show an
     * error about that value never being null. */

    Node *arg = (*lst)[1];
    if (arg->is_list) {
        arg = units->top()->mp->parsePotentialMacroCall(arg);
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
        FormProcInstParse(units, fn, block, arg, false, 
                                    false, NULL, &pr_value);

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

    pr->set(pr_value.block, ctx->tr->type_bool, icmpres);
    ParseResult temp;
    res = Operation::Destruct(ctx, &pr_value, &temp);
    if (!res) {
        return false;
    }
    pr->block = temp.block;

    return true;
}
}
