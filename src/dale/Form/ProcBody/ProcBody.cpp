#include "ProcBody.h"
#include "../../Units/Units.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Function/Function.h"
#include "../../CoreForms/CoreForms.h"
#include "../../Operation/Destruct/Destruct.h"
#include "../../Operation/CloseScope/CloseScope.h"
#include "../Linkage/Linkage.h"
#include "../Type/Type.h"
#include "../Proc/Inst/Inst.h"
#include "../../llvm_Function.h"

using namespace dale::ErrorInst::Generator;

namespace dale
{
bool
addVariables(Context *ctx, Node *node, Function *fn, llvm::BasicBlock *block)
{
    llvm::IRBuilder<> builder(block);

    bool past_first = false;
    for (std::vector<Variable *>::iterator b = fn->parameter_types.begin(),
                                           e = fn->parameter_types.end();
            b != e;
            ++b) {
        Variable *var = (*b);
        if (var->type->base_type == BaseType::VarArgs) {
            break;
        }
        Variable *param_var = new Variable();
        var->copyTo(param_var);
        param_var->type =
            (var->type->is_reference)
                ? ctx->tr->getPointerType(var->type)
                : var->type;
        param_var->linkage = Linkage::Auto;

        if (past_first && fn->is_macro) {
            /* Macro arguments, past the first, always have a type of
             * (p DNode), notwithstanding that the type in the
             * Function is unrestricted (to support overloading). */
            param_var->type = ctx->tr->type_pdnode;
        }
        bool res = ctx->ns()->addVariable(param_var->name.c_str(), param_var);
        if (!res) {
            Error *e = new Error(RedefinitionOfVariable, node,
                                 param_var->name.c_str());
            ctx->er->addError(e);
            return false;
        }

        llvm::Type *llvm_type = ctx->toLLVMType(param_var->type, NULL, false);
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

bool
addRetval(Context *ctx, Node *node, Function *fn,
          llvm::BasicBlock *block, llvm::Value *return_value)
{
    Variable *var = new Variable();

    var->type            = ctx->tr->getPointerType(fn->return_type);
    var->name            = "retval";
    var->internal_name   = "retval";
    var->has_initialiser = false;
    var->once_tag        = "";
    var->index           = 0;
    var->linkage         = Linkage::Auto;

    bool res = ctx->ns()->addVariable(var->name.c_str(), var);
    if (!res) {
        Error *e = new Error(RedefinitionOfVariable, node,
                             var->name.c_str());
        ctx->er->addError(e);
        return false;
    }

    llvm::Type *llvm_type = ctx->toLLVMType(var->type, NULL,
                                            false);
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

bool
gotoCrossesDeclaration(DeferredGoto *dg, Label *label)
{
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
        vars_before.clear();
        label_ns->getVarsBeforeIndex(label->index, &vars_before);
        if (vars_before.size() > 0) {
            for
            (std::vector<Variable*>::iterator
                    vvb = vars_before.begin(),
                    vve = vars_before.end();
                    vvb != vve;
                    ++vvb) {
                if ((*vvb)->index >= dg->index) {
                    return true;
                }
            }
        }
        label_ns = label_ns->parent_namespace;
    }

    return false;
}

bool
resolveDeferredGotos(Context *ctx, Node *node, Function *fn,
                     llvm::BasicBlock *block)
{
    for (std::vector<DeferredGoto *>::iterator b = fn->deferred_gotos.begin(),
                                               e = fn->deferred_gotos.end();
            b != e;
            ++b) {
        DeferredGoto *dg = (*b);
        const char *label_name = dg->label_name.c_str();
        Label *label = fn->getLabel(label_name);
        if (!label) {
            Error *e = new Error(LabelNotInScope, node, label_name);
            ctx->er->addError(e);
            continue;
        }

        llvm::BasicBlock *block        = label->block;
        llvm::BasicBlock *block_marker = dg->block_marker;
        llvm::Instruction *marker      = dg->marker;

        llvm::IRBuilder<> builder(block_marker);

        if (!marker && (block_marker->size() != 0)) {
            marker = block_marker->getFirstNonPHI();
        }
        if (marker) {
            llvm::BasicBlock::iterator bi = marker;
            ++bi;
            if (bi != block_marker->end()) {
                builder.SetInsertPoint(bi);
            }
        }

        /* Create a vector of variables to destruct.  This will
         * comprise all variables in the goto's namespace and upwards,
         * until either null (top of function) or the label's
         * namespace is reached. */

        std::vector<Variable *> variables;

        Namespace *goto_ns = dg->ns;
        Namespace *current_ns = goto_ns;

        while (current_ns != label->ns) {
            current_ns->getVariables(&variables);
            current_ns = current_ns->parent_namespace;
            if (!current_ns) {
                break;
            }
        }

        if (current_ns != label->ns) {
            if (gotoCrossesDeclaration(dg, label)) {
                Error *e = new Error(GotoWillCrossDeclaration, dg->node);
                ctx->er->addError(e);
                return false;
            }
        }

        /* Add the destructors for the collected variables. */
        ParseResult pr_var;
        for (std::vector<Variable *>::iterator vb = variables.begin(),
                                               ve = variables.end();
                vb != ve;
                ++vb) {
            Variable *var = (*vb);
            ParseResult pr_destruct;
            pr_var.set(NULL, var->type, var->value);
            bool res = Operation::Destruct(ctx, &pr_var, &pr_destruct,
                                           &builder);
            if (!res) {
                return false;
            }
        }

        builder.CreateBr(block);
    }

    STL::deleteElements(&(fn->deferred_gotos));
    fn->deferred_gotos.clear();
    fn->labels.clear();

    return true;
}

bool
terminateBlocks(Context *ctx, Function *fn, llvm::Function *llvm_fn,
                llvm::Value *last_value, Type *last_type,
                Node *last_position)
{
    /* Iterate over the blocks in the function. If the block ends in a
     * terminator, all is well.  If it doesn't: if the block is the
     * last block in the function, create a return instruction
     * containing the last value evaluated, otherwise create a branch
     * instruction that moves to the next block.
     */

    int bcount = 1;
    int bmax = llvm_fn->size();
    for (llvm::Function::iterator b = llvm_fn->begin(),
                                  e = llvm_fn->end();
            b != e;
            ++b) {
        if ((b->size() == 0) || !(b->back().isTerminator())) {
            llvm::IRBuilder<> builder(b);

            /* The underlying error here will have been
             * reported earlier, if there is no
             * last_value. */
            if ((bcount == bmax) && !fn->hasRetval()) {
                if (last_value) {
                    Type *got_type = last_type;

                    if (!fn->return_type->isEqualTo(got_type)) {
                        std::string gotstr;
                        got_type->toString(&gotstr);
                        std::string expstr;
                        fn->return_type->toString(&expstr);
                        Error *e = new Error(IncorrectReturnType,
                                             last_position,
                                             expstr.c_str(), gotstr.c_str());
                        ctx->er->addError(e);
                        return false;
                    }
                    if (fn->return_type->base_type == BaseType::Void) {
                        Operation::CloseScope(ctx, fn, b, NULL, true);
                        builder.CreateRetVoid();
                    } else {
                        ParseResult x;
                        x.set(b, last_type, last_value);
                        Operation::CloseScope(ctx, fn, x.block, NULL, true);
                        builder.CreateRet(x.value);
                    }
                } else {
                    Operation::CloseScope(ctx, fn, b, NULL, true);
                    builder.CreateRetVoid();
                }
            } else {
                if (bcount == bmax) {
                    Operation::CloseScope(ctx, fn, b, NULL, true);
                    builder.CreateRetVoid();
                } else {
                    /* Get the next block and create a branch to it. */
                    ++b;
                    builder.CreateBr(b);
                    --b;
                }
            }
        }
        ++bcount;
    }

    return true;
}

void
removePostTerminators(llvm::Function *llvm_fn)
{
    /* Iterate over the blocks in the function. Delete all
     * instructions that occur after the first terminating
     * instruction. */

    for (llvm::Function::iterator b = llvm_fn->begin(),
                                  e = llvm_fn->end();
            b != e;
            ++b) {
        for (llvm::BasicBlock::iterator ib = b->begin(),
                                        ie = b->end();
                ib != ie;
                ++ib) {
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

bool
FormProcBodyParse(Units *units, Node *node, Function *fn,
                  llvm::Function *llvm_fn, int skip, bool is_anonymous,
                  llvm::Value *return_value)
{
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *lst = node->list;

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", llvm_fn);

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
            b != e;
            ++b) {
        Type *wanted_type = NULL;
        bool is_last = ((b + 1) == e);
        if (is_last) {
            wanted_type = fn->return_type;
        }
        ParseResult pr_res;
        bool res = FormProcInstParse(units, fn, next, (*b), false, false,
                                     wanted_type, &pr_res);
        if (res) {
            next = pr_res.block;
            if (is_last) {
                last_value = pr_res.value;
                last_type = pr_res.type;
                last_position = (*b);
            } else {
                ParseResult pr_destruct;
                bool res = Operation::Destruct(ctx, &pr_res, &pr_destruct);
                if (!res) {
                    return false;
                }
                next = pr_destruct.block;
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
}
