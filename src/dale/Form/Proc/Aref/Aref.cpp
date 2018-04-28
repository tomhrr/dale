#include "Aref.h"

#include <string>
#include <vector>

#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../../Utils/Utils.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcArefParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("$", node, 2, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *array_node = (*lst)[1];
    Node *index_node = (*lst)[2];

    ParseResult initial_array_pr;
    bool res = FormProcInstParse(units, fn, block, array_node, false,
                                 false, NULL, &initial_array_pr);
    if (!res) {
        return false;
    }

    if (!(initial_array_pr.type->array_type ||
          initial_array_pr.type->points_to)) {
        std::string type_str;
        initial_array_pr.type->toString(&type_str);
        Error *e =
            new Error(IncorrectArgType, array_node, "$",
                      "a pointer or array", "1", type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    ParseResult array_pr;
    bool is_array;
    if (initial_array_pr.type->array_type) {
        initial_array_pr.getAddressOfValue(ctx, &array_pr);
        array_pr.type =
            ctx->tr->getPointerType(initial_array_pr.type->array_type);
        is_array = true;
    } else {
        initial_array_pr.copyTo(&array_pr);
        is_array = false;
    }

    ParseResult index_pr;
    res = FormProcInstParse(units, fn, array_pr.block, index_node,
                            false, false, NULL, &index_pr);
    if (!res) {
        return false;
    }
    if (!index_pr.type->isIntegerType()) {
        std::string type_str;
        index_pr.type->toString(&type_str);
        Error *e = new Error(IncorrectType, index_node, "integer",
                             type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    llvm::IRBuilder<> builder(index_pr.block);
    std::vector<llvm::Value *> indices;
    if (!is_array) {
        indices.push_back(
            llvm::cast<llvm::Value>(index_pr.getValue(ctx)));
    } else {
        STL::push_back2(
            &indices, ctx->nt->getLLVMZero(),
            llvm::cast<llvm::Value>(index_pr.getValue(ctx)));
    }
    llvm::Value *array_value = array_pr.getValue(ctx);
    if (array_pr.type->is_array) {
        pr->type = ctx->tr->getPointerType(array_pr.type->array_type);
    } else {
        pr->type = array_pr.type;
    }

    llvm::Value *index_ptr =
        builder.Insert(createGEP(array_value, indices));

    pr->block = index_pr.block;

    pr->set(pr->block, pr->type, index_ptr);
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
