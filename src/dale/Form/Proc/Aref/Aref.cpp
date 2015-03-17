#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

using namespace dale::ErrorInst;

namespace dale
{
bool
FormProcArefParse(Units *units, Function *fn, llvm::BasicBlock *block,
                  Node *node, bool get_address, bool prefixed_with_core,
                  ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("$", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *array_node = (*lst)[1];
    Node *index_node = (*lst)[2];

    ParseResult array_pr;
    bool res = FormProcInstParse(units, fn, block, array_node, true,
                                 false, NULL, &array_pr);
    if (!res) {
        return false;
    }

    if (!array_pr.type->points_to) {
        std::string type_str;
        array_pr.type->toString(&type_str);
        Error *e = new Error(IncorrectArgType, array_node,
                             "$", "a pointer or array", "1",
                             type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    ParseResult index_pr;
    res = FormProcInstParse(units, fn, array_pr.block, index_node, false,
                            false, NULL, &index_pr);
    if (!res) {
        return false;
    }

    /* Attempt to cast index_pr to a size type, if it is not such a
     * type already. */

    if (index_pr.type->base_type != BaseType::Size) {
        ParseResult index_pr_size;
        bool res = Operation::Cast(ctx, index_pr.block, index_pr.value,
                                   index_pr.type, ctx->tr->type_size,
                                   index_node, true, &index_pr_size);
        if (!res) {
            std::string type_str;
            index_pr.type->toString(&type_str);
            Error *e = new Error(IncorrectArgType, index_node,
                                 "$", "int", "2", type_str.c_str());
            ctx->er->addError(e);
            return false;
        }
        index_pr_size.copyTo(&index_pr);
    }

    llvm::Value *index_ptr = NULL;

    llvm::IRBuilder<> builder(index_pr.block);
    if (array_pr.type->points_to->points_to) {
        llvm::Value *array_ptr = builder.CreateLoad(array_pr.value);
        std::vector<llvm::Value *> indices;
        indices.push_back(llvm::cast<llvm::Value>(index_pr.value));
        index_ptr =
            builder.Insert(
                llvm::GetElementPtrInst::Create(
                    array_ptr, llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "aref"
            );
    } else {
        std::vector<llvm::Value *> indices;
        STL::push_back2(&indices, ctx->nt->getLLVMZero(),
                        llvm::cast<llvm::Value>(index_pr.value));
        index_ptr =
            builder.Insert(
                llvm::GetElementPtrInst::Create(
                    array_pr.value, llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "aref"
            );
    }

    pr->block = index_pr.block;

    if (array_pr.type->is_array) {
        pr->type = ctx->tr->getPointerType(array_pr.type->array_type);
    } else if (array_pr.type->points_to->points_to) {
        pr->type = array_pr.type->points_to;
    } else if (array_pr.type->points_to->array_type) {
        pr->type = ctx->tr->getPointerType(
            array_pr.type->points_to->array_type
        );
    } else {
        std::string type_str;
        array_pr.type->toString(&type_str);
        Error *e = new Error(CanOnlyIndexIntoPointersAndArrays,
                             node, type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    pr->value = index_ptr;
    array_pr.block = index_pr.block;
    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &array_pr, &destruct_pr);
    if (!res) {
        return false;
    }
    index_pr.block = destruct_pr.block;
    res = Operation::Destruct(ctx, &index_pr, &destruct_pr);
    if (!res) {
        return false;
    }
    pr->block = destruct_pr.block;

    return true;
}
}
