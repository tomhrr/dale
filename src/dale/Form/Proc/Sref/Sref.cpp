#include "Sref.h"

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
bool FormProcSrefParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;
    Node *struct_node = (*lst)[1];
    Node *member_node = (*lst)[2];

    if (!ctx->er->assertArgNums(":", node, 2, 2)) {
        return false;
    }

    ParseResult struct_pr;
    bool res = FormProcInstParse(units, fn, block, struct_node, false,
                                 false, NULL, &struct_pr);
    if (!res) {
        return false;
    }

    Type *st_type = struct_pr.type;

    if (st_type->struct_name.size() == 0) {
        std::string type_str;
        st_type->toString(&type_str);
        Error *e = new Error(IncorrectArgType, struct_node, ":",
                             "a struct", "1", type_str.c_str());
        ctx->er->addError(e);
        return false;
    }

    member_node =
        units->top()->mp->parsePotentialMacroCall(member_node);
    if (!member_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom(":", member_node, "2")) {
        return false;
    }
    if (!ctx->er->assertAtomIsSymbol(":", member_node, "2")) {
        return false;
    }

    Struct *st = ctx->getStruct(st_type);
    if (!st) {
        Error *e = new Error(NotInScope, struct_node,
                             st_type->struct_name.c_str());
        ctx->er->addError(e);
        return false;
    }

    const char *member_name = member_node->token->str_value.c_str();
    int index = st->nameToIndex(member_name);

    if (index == -1) {
        Error *e = new Error(FieldDoesNotExistInStruct, member_node,
                             member_name, st_type->struct_name.c_str());
        ctx->er->addError(e);
        return false;
    }

    Type *member_type = st->indexToType(index);
    if (st_type->is_const) {
        member_type = ctx->tr->getConstType(member_type);
    }

    std::vector<llvm::Value *> indices;
    STL::push_back2(&indices, ctx->nt->getNativeInt(0),
                    ctx->nt->getNativeInt(index));

    llvm::IRBuilder<> builder(struct_pr.block);
    ParseResult value_pr;
    res = struct_pr.getAddressOfValue(ctx, &value_pr);
    if (!res) {
        return false;
    }
    struct_pr.block = value_pr.block;

    llvm::Value *vres = builder.CreateGEP(
        value_pr.getValue(ctx), llvm::ArrayRef<llvm::Value *>(indices));

    pr->set(struct_pr.block, ctx->tr->getPointerType(member_type),
            vres);

    ParseResult destruct_pr;
    res = Operation::Destruct(ctx, &struct_pr, &destruct_pr);
    if (!res) {
        return false;
    }
    pr->block = destruct_pr.block;

    return true;
}
}
