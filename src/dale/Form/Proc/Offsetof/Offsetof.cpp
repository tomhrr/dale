#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Offsetof/Offsetof.h"
#include "../../Type/Type.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcOffsetOfParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("offsetof", node, 2, 2)) {
        return false;
    }

    symlist *lst = node->list;

    /* First argument must be a struct name. */

    Node *struct_name = (*lst)[1];
    struct_name = gen->parseOptionalMacroCall(struct_name);
    if (!struct_name) {
        return false;
    }

    Type *mytype = FormTypeParse(gen, struct_name,
                                           false,
                                           false);
    if (!mytype) {
        return false;
    }

    if (!mytype->struct_name.size()) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, node,
            "struct", "offsetof", "not a struct"
        );
        ctx->er->addError(e);
        return false;
    }

    Struct *str =
        ctx->getStruct(
            mytype->struct_name.c_str(),
            &(mytype->namespaces)
        );

    if (!str) {
        fprintf(stderr, "Internal error: invalid struct name.\n");
        abort();
    }

    /* Second argument is struct field. */

    bool res = Operation::Offsetof::execute(ctx, block, mytype,
                           (*lst)[2]->token->str_value.c_str(),
                           pr);
    return res;
}
}
