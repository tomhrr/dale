#include "Dereference.h"

#include <string>
#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcDereferenceParse(Units *units, Function *fn,
                              llvm::BasicBlock *block, Node *node,
                              bool get_address, bool prefixed_with_core,
                              ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("@", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *ptr = (*lst)[1];

    ParseResult ptr_pr;
    bool res = FormProcInstParse(units, fn, block, ptr, false, false,
                                 NULL, &ptr_pr);
    if (!res) {
        return false;
    }

    Type *ptr_type = ptr_pr.type;

    if (!ptr_type->points_to) {
        std::string type_str;
        ptr_type->toString(&type_str);
        Error *e = new Error(CannotDereferenceNonPointer, node,
                             type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    if (ptr_type->points_to->base_type == BaseType::Void) {
        Error *e = new Error(CannotDereferenceVoidPointer, node);
        ctx->er->addError(e);
        return false;
    }

    pr->set(ptr_pr.block, NULL, NULL);

    if (!get_address) {
        pr->address_of_value = ptr_pr.getValue(ctx);
        pr->value_is_lvalue = true;
        pr->type_of_address_of_value = ptr_type;

        pr->set(pr->block, ptr_type->points_to, NULL);
    } else {
        pr->set(pr->block, ptr_type, ptr_pr.getValue(ctx));
    }

    /* Core dereference call results should not be copied.  Otherwise,
     * an overloaded setf that calls @ on the pointer to the
     * underlying value will not be able to work, because even (core @
     * x) will return a pointee to x, which will be copied to the
     * caller of this function. */
    if (prefixed_with_core) {
        pr->do_not_copy_with_setf = true;
    }

    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &ptr_pr, &destruct_pr);
    if (!res) {
        return false;
    }
    pr->block = destruct_pr.block;

    return true;
}
}
