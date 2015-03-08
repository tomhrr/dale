#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Function/Function.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../Inst/Inst.h"
#include "../../../llvm_Function.h"

using namespace dale::ErrorInst::Generator;

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

    ParseResult pr_array;
    bool res = FormProcInstParse(units, fn, block, array_node, true,
                                 false, NULL, &pr_array);
    if (!res) {
        return false;
    }

    if (!pr_array.type->points_to) {
        std::string type_str;
        pr_array.type->toString(&type_str);
        Error *e = new Error(IncorrectArgType, array_node,
                             "$", "a pointer or array", "1",
                             type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    ParseResult pr_index;
    res = FormProcInstParse(units, fn, pr_array.block, index_node, false,
                            false, NULL, &pr_index);
    if (!res) {
        return false;
    }

    /* Attempt to cast pr_index to a size type, if it is not such a
     * type already. */

    if (pr_index.type->base_type != BaseType::Size) {
        ParseResult pr_index_size;
        bool res = Operation::Cast(ctx, pr_index.block, pr_index.value,
                                   pr_index.type, ctx->tr->type_size,
                                   index_node, true, &pr_index_size);
        if (!res) {
            std::string type_str;
            pr_index.type->toString(&type_str);
            Error *e = new Error(IncorrectArgType, index_node,
                                 "$", "int", "2", type_str.c_str());
            ctx->er->addError(e);
            return false;
        }
        pr_index_size.copyTo(&pr_index);
    }

    llvm::Value *index_ptr = NULL;

    llvm::IRBuilder<> builder(pr_index.block);
    if (pr_array.type->points_to->points_to) {
        llvm::Value *array_ptr = builder.CreateLoad(pr_array.value);
        std::vector<llvm::Value *> indices;
        indices.push_back(llvm::cast<llvm::Value>(pr_index.value));
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
                        llvm::cast<llvm::Value>(pr_index.value));
        index_ptr =
            builder.Insert(
                llvm::GetElementPtrInst::Create(
                    pr_array.value, llvm::ArrayRef<llvm::Value*>(indices)
                ),
                "aref"
            );
    }

    pr->block = pr_index.block;

    if (pr_array.type->is_array) {
        pr->type = ctx->tr->getPointerType(pr_array.type->array_type);
    } else if (pr_array.type->points_to->points_to) {
        pr->type = pr_array.type->points_to;
    } else if (pr_array.type->points_to->array_type) {
        pr->type = ctx->tr->getPointerType(
            pr_array.type->points_to->array_type
        );
    } else {
        std::string type_str;
        pr_array.type->toString(&type_str);
        Error *e = new Error(CanOnlyIndexIntoPointersAndArrays,
                             node, type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    pr->value = index_ptr;
    pr_array.block = pr_index.block;
    ParseResult pr_destruct;
    res = Operation::Destruct(ctx, &pr_array, &pr_destruct);
    if (!res) {
        return false;
    }
    pr_index.block = pr_destruct.block;
    res = Operation::Destruct(ctx, &pr_index, &pr_destruct);
    if (!res) {
        return false;
    }
    pr->block = pr_destruct.block;

    return true;
}
}
