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
void
linkVariablesToFunction(std::vector<Variable *> *vars, llvm::Function *llvm_fn)
{
    llvm::Function::arg_iterator llvm_arg_iter = llvm_fn->arg_begin();
    for (std::vector<Variable *>::iterator b = vars->begin(),
                                           e = vars->end();
            b != e;
            ++b) {
        if ((*b)->type->base_type == BaseType::VarArgs) {
            if (!(*b)->name.compare("rest")) {
                llvm::Value *llvm_param = llvm_arg_iter;
                ++llvm_arg_iter;
                llvm_param->setName((*b)->name.c_str());
                (*b)->value = llvm_param;
            }
            break;
        }
        llvm::Value *llvm_param = llvm_arg_iter;
        ++llvm_arg_iter;
        llvm_param->setName((*b)->name.c_str());
        (*b)->value = llvm_param;
    }
}
}
