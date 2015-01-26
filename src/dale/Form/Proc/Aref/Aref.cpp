#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Cast/Cast.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

namespace dale
{
bool
FormProcArefParse(Generator *gen,
           Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("$", node, 2, 2)) {
        return false;
    }

    symlist *lst = node->list;
    ParseResult pr_array;
    bool res = 
        FormProcInstParse(gen, fn, block, (*lst)[1], true,
                                    false, 
                                    NULL, &pr_array);
    if (!res) {
        return false;
    }

    if (!(pr_array.type->points_to)) {
        std::string temp;
        pr_array.type->toString(&temp);
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            ((*lst)[1]),
            "$", "a pointer or array", "1", temp.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    ParseResult pr_index;
    res =
        FormProcInstParse(gen, fn, pr_array.block, (*lst)[2], false,
                               false, NULL, &pr_index);
    if (!res) {
        return false;
    }

    /* Attempt to cast pr_index to a size type, if it is not such
     * a type already. */

    if (pr_index.type->base_type != BaseType::Size) {
        ParseResult newt;
        bool res =
            Operation::Cast::execute(ctx,
                                     pr_index.block,
                                     pr_index.value,
                                     pr_index.type, 
                                     ctx->tr->type_size,
                                     (*lst)[2],
                                     true,
                                     &newt);
        
        if (!res) {
            std::string temp;
            pr_index.type->toString(&temp);
            Error *e = new Error(
                ErrorInst::Generator::IncorrectArgType,
                ((*lst)[2]),
                "$", "int", "2", temp.c_str()
            );
            ctx->er->addError(e);
            return false;
        }
        newt.copyTo(&pr_index);
    }

    llvm::IRBuilder<> builder(pr_index.block);

    llvm::Value *proper_ptr;
    llvm::Value *vres;

    if (pr_array.type->points_to->points_to) {
        proper_ptr = builder.CreateLoad(pr_array.value);
        std::vector<llvm::Value *> indices;
        indices.push_back(llvm::cast<llvm::Value>(pr_index.value));
        vres = builder.Insert(
                  llvm::GetElementPtrInst::Create(
                      proper_ptr,
                      llvm::ArrayRef<llvm::Value*>(indices)
                  ),
                  "asdf"
              );
    } else {
        std::vector<llvm::Value *> indices;
        stl::push_back2(&indices, ctx->nt->getLLVMZero(),
                        llvm::cast<llvm::Value>(pr_index.value));
        vres = builder.Insert(
                  llvm::GetElementPtrInst::Create(
                      pr_array.value,
                      llvm::ArrayRef<llvm::Value*>(indices)
                  ),
                  "asdf"
              );
    }

    pr->block = pr_index.block;

    /* pr_array returns a pointer - that's what you want to
     * return, here. */
    if (pr_array.type->is_array) {
        pr->type = ctx->tr->getPointerType(pr_array.type->array_type);
    } else {
        if (pr_array.type->points_to->points_to) {
            pr->type = pr_array.type->points_to;
        } else if (pr_array.type->points_to->array_type) {
            pr->type = ctx->tr->getPointerType(
                pr_array.type->points_to->array_type
            );
        } else {
            std::string typestr;
            pr_array.type->toString(&typestr);
            Error *e = new Error(
                ErrorInst::Generator::CanOnlyIndexIntoPointersAndArrays,
                node,
                typestr.c_str()
            );
            ctx->er->addError(e);
            return false;
        }
    }
    pr->value = vres;

    pr_array.block = pr_index.block;
    ParseResult temp;
    res = gen->destructIfApplicable(&pr_array, NULL, &temp);
    if (!res) {
        return false;
    }
    pr_index.block = temp.block;

    res = gen->destructIfApplicable(&pr_index, NULL, &temp);
    if (!res) {
        return false;
    }
    pr->block = temp.block;

    return true;
}
}
