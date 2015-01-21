#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcSrefParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    symlist *lst = node->list;

    if (!ctx->er->assertArgNums(":", node, 2, 2)) {
        return false;
    }

    get_address = true;

    int original_error_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    ParseResult pr_struct;
    bool res =
        FormProcInstParse(gen, fn, block, (*lst)[1], get_address,
                               false,
                               NULL, &pr_struct);

    if (!res) {
        /* If the error message is 'cannot take address of
         * non-lvalue', make get_address false and go from there. */
        if (ctx->er->getErrorTypeCount(ErrorType::Error) ==
                original_error_count + 1) {
            Error *e = ctx->er->popLastError();
            if (e->instance ==
                    ErrorInst::Generator::CannotTakeAddressOfNonLvalue) {
                res =
                    FormProcInstParse(gen, 
                        fn, block, (*lst)[1], false, 
                        false, NULL, &pr_struct
                    );
                if (!res) {
                    ctx->er->addError(e);
                    return false;
                }
                get_address = false;
            } else {
                ctx->er->addError(e);
                return false;
            }
        } else {
            return false;
        }
    }

    /* If get_address is false, allocate space for the struct and
     * modify pr_struct so that the rest of the code continues to
     * work. */

    if (!get_address) {
        llvm::IRBuilder<> builder(pr_struct.block);
        llvm::Type *llvm_type =
            ctx->toLLVMType(pr_struct.type, NULL, false, false);
        if (!llvm_type) {
            return false;
        }
        llvm::Value *store =
            builder.CreateAlloca(llvm_type);
        builder.CreateStore(pr_struct.value, store);
        pr_struct.type = ctx->tr->getPointerType(pr_struct.type);
        pr_struct.value = store;
    }

    /* Able to assume points_to here, because get_address was set
     * to true in the preceding pfbi call. */

    int is_const = pr_struct.type->points_to->is_const;

    if (pr_struct.type->points_to->struct_name.size() == 0) {
        std::string temp;
        pr_struct.type->points_to->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            ":", "a struct", "1", temp.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    if (ctx->getEnum(pr_struct.type->points_to->struct_name.c_str())) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            ":", "a struct", "1", "an enum"
        );
        ctx->er->addError(e);
        return false;
    }

    Node *ref = (*lst)[2];
    ref = gen->parseOptionalMacroCall(ref);
    if (!ref) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom(":", ref, "2")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol(":", ref, "2")) {
        return false;
    }

    Token *t = ref->token;

    Struct *structp =
        ctx->getStruct(
            pr_struct.type->points_to->struct_name.c_str(),
            &(pr_struct.type->points_to->namespaces)
        );

    if (!structp) {
        Error *e = new Error(
            ErrorInst::Generator::NotInScope,
            ((*lst)[1]),
            pr_struct.type->points_to->struct_name.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    int index = structp->memberToIndex(t->str_value.c_str());

    if (index == -1) {
        Error *e = new Error(
            ErrorInst::Generator::FieldDoesNotExistInStruct,
            ((*lst)[2]),
            t->str_value.c_str(),
            pr_struct.type->points_to->struct_name.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    Type *eltype = structp->indexToType(index);
    if (is_const) {
        eltype = ctx->tr->getConstType(eltype);
    }

    std::vector<llvm::Value *> indices;
    stl::push_back2(&indices, ctx->nt->getNativeInt(0),
                              ctx->nt->getNativeInt(index));

    llvm::IRBuilder<> builder(pr_struct.block);
    llvm::Value *vres =
        builder.CreateGEP(pr_struct.value,
                          llvm::ArrayRef<llvm::Value*>(indices));

    /* This has changed - sref will always return a pointer,
     * regardless of get_address (it is as if get_address was always
     * enabled). */

    pr->set(pr_struct.block, ctx->tr->getPointerType(eltype), vres);

    ParseResult temp;
    res = gen->destructIfApplicable(&pr_struct, NULL, &temp);
    if (!res) {
        return false;
    }
    pr->block = temp.block;

    return true;
}
}
