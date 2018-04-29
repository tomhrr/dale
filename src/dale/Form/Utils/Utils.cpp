#include "Utils.h"

#include <vector>

#include "../../Function/Function.h"
#include "../../Node/Node.h"
#include "../../Operation/Destruct/Destruct.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Units/Units.h"
#include "../../llvm_Function.h"
#include "../Linkage/Linkage.h"
#include "../Proc/Inst/Inst.h"
#include "../Struct/Struct.h"
#include "../Type/Type.h"

namespace dale {
void linkVariablesToFunction(std::vector<Variable *> *vars,
                             llvm::Function *llvm_fn) {
    llvm::Function::arg_iterator llvm_arg_iter = llvm_fn->arg_begin();
    for (std::vector<Variable *>::iterator b = vars->begin(),
                                           e = vars->end();
         b != e; ++b) {
        if ((*b)->type->base_type == BaseType::VarArgs) {
            if (!(*b)->name.compare("rest")) {
                llvm::Value *llvm_param = &*llvm_arg_iter;
                ++llvm_arg_iter;
                llvm_param->setName((*b)->name.c_str());
                (*b)->value = llvm_param;
            }
            break;
        }
        llvm::Value *llvm_param = &*llvm_arg_iter;
        ++llvm_arg_iter;
        llvm_param->setName((*b)->name.c_str());
        (*b)->value = llvm_param;
    }
}

llvm::Instruction *createGEP(llvm::Value *value,
                             llvm::ArrayRef<llvm::Value *> indices,
                             llvm::Type *type) {
#if D_LLVM_VERSION_ORD <= 36
    return llvm::GetElementPtrInst::Create(value, indices);
#else
    if (!type) {
        type = value->getType()->getPointerElementType();
    }
    return llvm::GetElementPtrInst::Create(type, value, indices);
#endif
}

llvm::Constant *createConstantGEP(llvm::Constant *value,
                                  llvm::ArrayRef<llvm::Value *> indices,
                                  llvm::Type *type) {
#if D_LLVM_VERSION_ORD <= 36
    return llvm::ConstantExpr::getGetElementPtr(value, indices);
#else
    if (!type) {
        type = value->getType()->getPointerElementType();
    }
    return llvm::ConstantExpr::getGetElementPtr(type, value, indices);
#endif
}
}
