#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Dereference
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

    assert(node->list && "parseDereference must receive a list!");

    symlist *lst = node->list;

    if (!ctx->er->assertArgNums("@", node, 1, 1)) {
        return false;
    }

    ParseResult p;
    bool res =
        Form::Proc::Inst::parse(gen, fn, block, (*lst)[1], false, 
                                    false, NULL,
                                    &p);

    if (!res) {
        return false;
    }

    /* It should be a pointer. */

    if (!(p.type->points_to)) {
        std::string temp;
        p.type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::CannotDereferenceNonPointer,
            node,
            temp.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    if (p.type->points_to->base_type == Type::Void) {
        Error *e = new Error(
            ErrorInst::Generator::CannotDereferenceVoidPointer,
            node
        );
        ctx->er->addError(e);
        return false;
    }

    /* If getAddress is false (the usual case), append a load
     * instruction, otherwise just return the value. */

    pr->set(p.block, NULL, NULL);

    if (!get_address) {
        llvm::IRBuilder<> builder(p.block);
        llvm::Value *res =
            llvm::cast<llvm::Value>(builder.CreateLoad(p.value));

        pr->type  = p.type->points_to;
        pr->value = res;
    } else {
        pr->type  = p.type;
        pr->value = p.value;
    }

    /* If this isn't here, then an overloaded setf that calls @
     * on the pointer to the underlying value will not be able to
     * work, because even (core @ x) will return a pointee to x,
     * which will be copied to the caller of this function. */
    if (prefixed_with_core) {
        pr->do_not_copy_with_setf = 1;
    }

    ParseResult temp;
    res = gen->destructIfApplicable(&p, NULL, &temp);
    if (!res) {
        return false;
    }
    pr->block = temp.block;

    return true;
}
}
}
}
}
