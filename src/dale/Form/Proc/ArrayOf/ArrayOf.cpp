#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../../Type/Type.h"
#include "../../Literal/Array/Array.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace ArrayOf
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

    symlist *lst = node->list;

    if (!ctx->er->assertArgNums("array-of", node, 3, -1)) {
        return false;
    }

    Node *newnum = gen->parseOptionalMacroCall((*lst)[1]);
    if (!newnum) {
        return false;
    }

    int size = gen->parseInteger(newnum);
    if (size == -1) {
        return false;
    }

    Element::Type *type = Form::Type::parse(gen, (*lst)[2], false, false);
    if (!type) {
        return false;
    }

    Element::Type *arrtype = ctx->tr->getArrayType(type, size);

    int size2;
    return
        Form::Literal::Array::parse(
            gen,
            fn,
            block,
            ((*lst)[3]),
            "array literal",
            arrtype,
            get_address,
            &size2,
            pr
        );
}
}
}
}
}
