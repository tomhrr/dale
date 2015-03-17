#include "Utils.h"

#include "../../Units/Units.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Function/Function.h"
#include "../../Operation/Destruct/Destruct.h"
#include "../Linkage/Linkage.h"
#include "../Type/Type.h"
#include "../Struct/Struct.h"
#include "../Proc/Inst/Inst.h"
#include "../../llvm_Function.h"

namespace dale {
bool
FormProcessValue(Units *units, Function *fn, llvm::BasicBlock *block,
                 Node *var_value_node, bool get_address, Type *wanted_type,
                 ParseResult *pr)
{
    Context *ctx = units->top()->ctx;
    std::vector<Node *> *var_value_node_list = var_value_node->list;
    Variable *var_value = NULL;
    llvm::IRBuilder<> builder(block);

    if (var_value_node->is_token
            && (var_value = ctx->getVariable(
                    var_value_node->token->str_value.c_str()))) {
        pr->set(block, var_value->type,
                builder.CreateLoad(var_value->value));
        pr->do_not_destruct = true;
    } else if (var_value_node->is_list
            && (var_value_node_list->size() == 2)
            && ((*var_value_node_list)[0]->is_token)
            && (!(*var_value_node_list)[0]->token->str_value.compare("@"))
            && ((*var_value_node_list)[1]->is_token)
            && (var_value = ctx->getVariable(
                    (*var_value_node_list)[1]->token->str_value.c_str()))) {
        pr->set(block, var_value->type->points_to,
                builder.CreateLoad(builder.CreateLoad(var_value->value)));
        pr->do_not_destruct = true;
    } else {
        bool res =
            FormProcInstParse(units,
                fn, block, var_value_node, get_address, false,
                wanted_type, pr
            );
        if (!res) {
            return false;
        }
    }

    return true;
}
}
