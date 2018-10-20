#include "ProcBody.h"

#include <string>
#include <vector>

#include "../../CoreForms/CoreForms.h"
#include "../../Function/Function.h"
#include "../../Node/Node.h"
#include "../../Operation/CloseScope/CloseScope.h"
#include "../../Operation/Copy/Copy.h"
#include "../../Operation/Destruct/Destruct.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Units/Units.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../../llvm_Function.h"
#include "../Linkage/Linkage.h"
#include "../Proc/Inst/Inst.h"
#include "../Type/Type.h"

using namespace dale::ErrorInst;

namespace dale {
bool addVariables(Context *ctx, Node *node, Function *fn,
                  llvm::BasicBlock *block) {
    llvm::IRBuilder<> builder(block);

    bool past_first = false;
    for (std::vector<Variable *>::iterator b = fn->parameters.begin(),
                                           e = fn->parameters.end();
         b != e; ++b) {
        Variable *var = (*b);
        bool is_macro_varargs = false;
        if (var->type->base_type == BaseType::VarArgs) {
            if (!var->name.compare("rest")) {
                is_macro_varargs = true;
            } else {
                break;
            }
        }
        Variable *param_var = new Variable();
        var->copyTo(param_var);
        /* todo: It would be better if TypeRegister supported a
         * 'getReferencedType' function, to avoid the copying below.
         * */
        if (var->type->is_reference) {
            var->type->is_reference = false;
            param_var->type =
                ctx->tr->getPointerType(var->type->makeCopy());
            var->type->is_reference = true;
        } else if (var->type->is_rvalue_reference) {
            var->type->is_rvalue_reference = false;
            param_var->type =
                ctx->tr->getPointerType(var->type->makeCopy());
            var->type->is_rvalue_reference = true;
        } else {
            param_var->type = var->type;
        }
        param_var->linkage = Linkage::Auto;

        if (past_first && fn->is_macro) {
            /* Macro arguments, past the first, always have a type of
             * (p DNode), notwithstanding that the type in the
             * Function is unrestricted (to support overloading).
             * (Except for the 'rest' parameter.) */
            param_var->type = ctx->tr->type_pdnode;
            if (is_macro_varargs) {
                param_var->type =
                    ctx->tr->getPointerType(ctx->tr->type_pdnode);
            }
        }
        bool res =
            ctx->addVariable(param_var->name.c_str(), param_var);
        if (!res) {
            Error *e = new Error(RedefinitionOfVariable, node,
                                 param_var->name.c_str());
            ctx->er->addError(e);
            return false;
        }

        llvm::Type *llvm_type =
            ctx->toLLVMType(param_var->type, NULL, false);
        if (!llvm_type) {
            return false;
        }
        llvm::Value *param_var_ptr =
            llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_type));
        llvm::Value *original_value = param_var->value;
        param_var->value = param_var_ptr;
        builder.CreateStore(original_value, param_var_ptr);
        past_first = true;
    }

    return true;
}

bool addRetval(Context *ctx, Node *node, Function *fn,
               llvm::BasicBlock *block, llvm::Value *return_value) {
    Variable *var = new Variable();

    var->type = ctx->tr->getPointerType(fn->return_type);
    var->name = "retval";
    var->symbol = "retval";
    var->has_initialiser = false;
    var->once_tag = "";
    var->index = 0;
    var->linkage = Linkage::Auto;

    bool res = ctx->addVariable(var->name.c_str(), var);
    if (!res) {
        Error *e =
            new Error(RedefinitionOfVariable, node, var->name.c_str());
        ctx->er->addError(e);
        return false;
    }

    llvm::Type *llvm_type = ctx->toLLVMType(var->type, NULL, false);
    if (!llvm_type) {
        return false;
    }

    llvm::IRBuilder<> builder(block);
    llvm::Value *new_ptr =
        llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_type));
    var->value = new_ptr;
    builder.CreateStore(return_value, new_ptr);

    return true;
}

bool gotoCrossesDeclaration(DeferredGoto *dg, Label *label) {
    /* If the label's namespace, or any namespace in which the label's
     * namespace is located, has a variable declaration with an index
     * smaller than that of the label, then the goto is going to cross
     * that variable declaration.  This is an error, because the
     * declaration may result in a destructor being called on scope
     * close, and the variable may not be initialised when the goto is
     * reached. */

    std::vector<Variable *> vars_before;
    Namespace *label_ns = label->ns;

    while (label_ns) {
        label_ns->getVarsBeforeIndex(label->index, &vars_before);
        if (vars_before.size() > 0) {
            for (std::vector<Variable *>::iterator
                     b = vars_before.begin(),
                     e = vars_before.end();
                 b != e; ++b) {
                if ((*b)->index >= dg->index) {
                    return true;
                }
            }
            vars_before.clear();
        }
        label_ns = label_ns->parent_namespace;
    }

    return false;
}

bool resolveDeferredGotos(Context *ctx, Node *node, Function *fn,
                          llvm::BasicBlock *block) {
    for (std::vector<DeferredGoto *>::iterator
             b = fn->deferred_gotos.begin(),
             e = fn->deferred_gotos.end();
         b != e; ++b) {
        DeferredGoto *dg = (*b);
        const char *label_name = dg->label_name.c_str();
        Label *label = fn->getLabel(label_name);
        if (!label) {
            Error *e = new Error(LabelNotInScope, node, label_name);
            ctx->er->addError(e);
            continue;
        }

        llvm::BasicBlock *label_block = label->block;
        llvm::BasicBlock *block_marker = dg->block_marker;
        llvm::Instruction *marker = dg->marker;

        llvm::IRBuilder<> builder(block_marker);

        if (!marker && (block_marker->size() != 0)) {
            marker = block_marker->getFirstNonPHI();
        }
        if (marker) {
            llvm::BasicBlock::iterator bi =
                instructionToIterator(marker);
            ++bi;
            if (bi != block_marker->end()) {
                setInsertPoint(&builder, bi);
            }
        }

        /* Create a vector of variables to destruct.  This will
         * comprise all variables in the goto's namespace and upwards,
         * until either the namespace before the root or the label's
         * namespace is reached. */

        std::vector<Variable *> variables;

        Namespace *goto_ns = dg->ns;
        Namespace *current_ns = goto_ns;

        while (current_ns != label->ns) {
            Namespace *parent = current_ns->parent_namespace;
            if (parent && !parent->parent_namespace) {
                break;
            }
            current_ns->getVariables(&variables);
            current_ns = parent;
        }

        if (current_ns != label->ns) {
            if (gotoCrossesDeclaration(dg, label)) {
                Error *e =
                    new Error(GotoWillCrossDeclaration, dg->node);
                ctx->er->addError(e);
                return false;
            }
        }

        /* Add the destructors for the collected variables. */
        ParseResult var_pr;
        for (std::vector<Variable *>::iterator vb = variables.begin(),
                                               ve = variables.end();
             vb != ve; ++vb) {
            Variable *var = (*vb);
            ParseResult destruct_pr;
            var_pr.set(NULL, var->type, var->value);
            var_pr.do_not_destruct = false;
            bool res = Operation::Destruct(ctx, &var_pr, &destruct_pr,
                                           &builder);
            if (!res) {
                return false;
            }
        }

        builder.CreateBr(label_block);
    }

    STL::deleteElements(&(fn->deferred_gotos));
    fn->deferred_gotos.clear();
    fn->labels.clear();

    return true;
}

bool terminateBlocks(Context *ctx, Function *fn,
                     llvm::Function *llvm_fn, llvm::Value *last_value,
                     Type *last_type, Node *last_position) {
    /* Iterate over the blocks in the function. If the block ends in a
     * terminator, all is well.  If it doesn't: if the block is the
     * last block in the function, create a return instruction
     * containing the last value evaluated, otherwise create a branch
     * instruction that moves to the next block.
     */

    for (llvm::Function::iterator b = llvm_fn->begin(),
                                  e = llvm_fn->end();
         b != e; ++b) {
        if (b->size() && b->back().isTerminator()) {
            continue;
        }
        llvm::IRBuilder<> builder(&*b);
        ++b;
        if (b != e) {
            builder.CreateBr(&*b);
            --b;
            continue;
        }
        --b;

        Operation::CloseScope(ctx, fn, &*b, NULL, true);

        /* The underlying error here will have been reported earlier,
         * if there is no last_value. */
        if (fn->return_type->base_type == BaseType::Void) {
            builder.CreateRetVoid();
        } else if (!fn->hasRetval() && last_value) {
            Type *got_type = last_type;
            if (!fn->return_type->canBePassedFrom(got_type)) {
                std::string wanted;
                fn->return_type->toString(&wanted);
                std::string got;
                got_type->toString(&got);
                Error *e = new Error(IncorrectReturnType, last_position,
                                     wanted.c_str(), got.c_str());
                ctx->er->addError(e);
                return false;
            }
            builder.CreateRet(last_value);
        } else {
            builder.CreateRetVoid();
        }
    }

    return true;
}

void removePostTerminators(llvm::Function *llvm_fn) {
    /* Iterate over the blocks in the function. Delete all
     * instructions that occur after the first terminating
     * instruction. */

    for (llvm::iplist<llvm::BasicBlock>::reverse_iterator
             b = llvm_fn->getBasicBlockList().rbegin(),
             e = llvm_fn->getBasicBlockList().rend();
         b != e; ++b) {
        for (llvm::BasicBlock::iterator ib = b->begin(), ie = b->end();
             ib != ie; ++ib) {
            if (!(*ib).isTerminator()) {
                continue;
            }
            ++ib;
            if (ib == ie) {
                break;
            }
            /* Subtraction is not publicly available on the LLVM
             * iterator classes. */
            int count = 0;
            while (ib != ie) {
                ++count;
                ++ib;
            }
            while (count--) {
                b->getInstList().pop_back();
            }
            break;
        }
    }
}

static bool FormProcBodyParseInner(Units *units, Node *node, Function *fn,
                                   llvm::Function *llvm_fn, int skip,
                                   bool is_anonymous, llvm::Value *return_value) {
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*getContext(), "entry", llvm_fn);

    units->top()->pushGlobalBlock(block);

    llvm::BasicBlock *next = block;
    llvm::IRBuilder<> builder(block);

    addVariables(ctx, node, fn, block);

    if (fn->hasRetval()) {
        addRetval(ctx, node, fn, block, return_value);
    }

    llvm::Value *last_value = NULL;
    Type *last_type = NULL;
    Node *last_position = NULL;

    for (std::vector<Node *>::iterator b = (lst->begin() + skip),
                                       e = lst->end();
         b != e; ++b) {
        Type *wanted_type = NULL;
        bool is_last = ((b + 1) == e);
        if (is_last) {
            wanted_type = fn->return_type;
        }
        ParseResult res_pr;
        bool res = FormProcInstParse(units, fn, next, (*b), false,
                                     false, wanted_type, &res_pr);
        if (res) {
            next = res_pr.block;
            if (is_last) {
                bool res = Operation::Copy(units->top()->ctx, fn, (*b),
                                           &res_pr, &res_pr);
                if (!res) {
                    return false;
                }
                last_value = res_pr.getValue(ctx);
                last_type = res_pr.type;
                last_position = (*b);
            } else {
                ParseResult destruct_pr;
                bool res =
                    Operation::Destruct(ctx, &res_pr, &destruct_pr);
                if (!res) {
                    return false;
                }
                next = destruct_pr.block;
            }
        }
    }

    bool res = resolveDeferredGotos(ctx, node, fn, block);
    if (!res) {
        return false;
    }

    res = terminateBlocks(ctx, fn, llvm_fn, last_value, last_type,
                          last_position);
    if (!res) {
        return false;
    }

    removePostTerminators(llvm_fn);
    units->top()->popGlobalBlock();

    return true;
}

bool FormProcBodyParse(Units *units, Node *node, Function *fn,
                       llvm::Function *llvm_fn, int skip,
                       bool is_anonymous, llvm::Value *return_value) {
    Context *ctx = units->top()->ctx;
    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;
    ctx->activateFunctionScope(fn);

    bool ret = FormProcBodyParseInner(units, node, fn, llvm_fn, skip, is_anonymous, return_value);

    ctx->deactivateFunctionScope(fn);
    ctx->deactivateNamespace(anon_name.c_str());

    return ret;
}

}
