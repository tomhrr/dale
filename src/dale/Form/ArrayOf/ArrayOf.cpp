#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace ArrayOf
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

    if (!gen->assertArgNums("array-of", node, 3, -1)) {
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

    Element::Type *type = gen->parseType((*lst)[2], false, false);
    if (!type) {
        return false;
    }

    Element::Type *arrtype = ctx->tr->getArrayType(type, size);

    int size2;
    bool res =
        gen->parseArrayLiteral(
            fn,
            block,
            ((*lst)[3]),
            "array literal",
            arrtype,
            get_address,
            &size2,
            pr
        );

    return res;
}
}
}
}
