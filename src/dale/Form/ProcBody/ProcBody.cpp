#include "ProcBody.h"
#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "../../CoreForms/CoreForms.h"
#include "../Linkage/Linkage.h"
#include "../Type/Type.h"
#include "../Proc/Inst/Inst.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace ProcBody
{
bool
parse(Generator *gen,
      Node *n,
      Element::Function *dfn,
      llvm::Function *fn,
      int skip,
      int is_anonymous)
{
    Context *ctx = gen->ctx;

    symlist *lst = n->list;

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", fn);

    gen->global_blocks.push_back(block);
    gen->global_block = block;

    llvm::BasicBlock *next  = block;
    llvm::IRBuilder<> builder(block);

    /* Add 'define' calls for the args to the function. */

    std::vector<Element::Variable *>::iterator fn_args_iter;
    fn_args_iter = dfn->parameter_types->begin();

    /* Add the variables to the context, once confirmed that it is
     * not just a declaration. */

    int mcount = 0;

    while (fn_args_iter != dfn->parameter_types->end()) {
        if ((*fn_args_iter)->type->base_type == dale::Type::VarArgs) {
            break;
        }
        int avres;
        Element::Variable *myvart = (*fn_args_iter);
        Element::Variable *myvar = new Element::Variable();
        myvar->type          = myvart->type;
        myvar->name          = myvart->name;
        myvar->internal_name = myvart->internal_name;
        myvar->value         = myvart->value;
        myvar->has_initialiser = myvart->has_initialiser;
        myvar->once_tag = myvart->once_tag;
        myvar->index = myvart->index;
        myvar->linkage = dale::Linkage::Auto;

        if (mcount >= 1 && dfn->is_macro) {
            /* Macro arguments, past the first, always have a type of
             * (p DNode), notwithstanding that the type in the
             * Element::Function can be anything (to support
             * overloading). */
            std::string mtype;
            myvar->type->toStringProper(&mtype);
            myvar->type =
                ctx->tr->getPointerType(
                    ctx->tr->getStructType("DNode")
                );
        }
        avres = ctx->ns()->addVariable(
                    myvar->name.c_str(), myvar
                );
        if (!avres) {
            Error *e = new Error(
                ErrorInst::Generator::RedefinitionOfVariable,
                n,
                myvar->name.c_str()
            );
            ctx->er->addError(e);
            return 0;
        }

        llvm::Value *original_value = myvar->value;

        /* Make CreateAlloca instructions for each argument. */
        llvm::Type *llvm_type =
            ctx->toLLVMType(myvar->type,
                           NULL,
                           false);
        if (!llvm_type) {
            return 0;
        }

        llvm::Value *new_ptr = llvm::cast<llvm::Value>(
                                   builder.CreateAlloca(llvm_type)
                               );

        myvar->value = new_ptr;

        builder.CreateStore(original_value, new_ptr);

        ++fn_args_iter;
        ++mcount;
    }

    std::vector<Node *>::iterator iter;
    iter = lst->begin();

    llvm::Value *last_value = NULL;
    Element::Type *last_type = NULL;
    Node *last_position = NULL;

    /* Skip the fn token, the linkage, the return type and the
     * arg list. */

    int count = 0;
    int size  = (int) lst->size();

    while (skip--) {
        ++count;
        ++iter;
    }

    while (iter != lst->end()) {
        Element::Type *wanted_type = NULL;
        if ((count + 1) == size) {
            wanted_type = dfn->return_type;
        }
        ParseResult p;
        bool res =
            Form::Proc::Inst::parse(gen, dfn, next, (*iter), false, false,
            wanted_type, &p);
        if (!res) {
            /* Add an option to stop on first error, which would
             * break here. */
        } else {
            next = p.block;
            if ((count + 1) == size) {
                /* Set the value of the last instruction for possible
                 * implicit return later on. */
                last_value = p.value;
                /* Set the last type so as to ensure that the final
                 * return value is valid. */
                last_type = p.type;
                Token *x = new Token(
                    0,
                    (*iter)->getBeginPos()->getLineNumber(),
                    (*iter)->getBeginPos()->getColumnNumber(),
                    (*iter)->getEndPos()->getLineNumber(),
                    (*iter)->getEndPos()->getColumnNumber()
                );
                last_position = new Node(x);
                last_position->filename = (*iter)->filename;
            } else {
                ParseResult temp;
                bool res = gen->destructIfApplicable(&p, NULL, &temp);
                if (!res) {
                    return 0;
                }
                next = temp.block;
            }
        }
        ++iter;
        ++count;
    }

    int res = 1;
    int bcount;
    int bmax;

    if (dfn->defgotos->size() > 0) {
        /* Have got deferred gotos - try to resolve. */
        std::vector<DeferredGoto *>::iterator dgiter =
            dfn->defgotos->begin();

        while (dgiter != dfn->defgotos->end()) {
            std::string *ln     = (*dgiter)->label_name;
            Element::Label *ell = dfn->getLabel(ln->c_str());
            if (!ell) {
                Error *e = new Error(
                    ErrorInst::Generator::LabelNotInScope,
                    n,
                    ln->c_str()
                );
                ctx->er->addError(e);

                delete ln;
                delete (*dgiter);

                ++dgiter;

                continue;
            }
            llvm::BasicBlock *b = ell->block;

            /* Load an llvm::IRBuilder and point it to the correct
             * spot. */
            llvm::BasicBlock *block_marker = (*dgiter)->block_marker;
            llvm::Instruction *marker      = (*dgiter)->marker;
            llvm::IRBuilder<> builder(block_marker);
            builder.SetInsertPoint(block_marker);

            if (!((*dgiter)->marker)) {
                if (block_marker->size() == 0) {
                    builder.SetInsertPoint(block_marker);
                } else {
                    llvm::Instruction *fnp =
                        block_marker->getFirstNonPHI();

                    /* Get the instruction after the first non-PHI
                     * node, and set that as the insertion point.
                     * If such an instruction doesn't exist, then
                     * the previous SetInsertPoint call will do
                     * the trick. */

                    llvm::BasicBlock::iterator bi, be;

                    for (bi = block_marker->begin(),
                            be = block_marker->end();
                            bi != be;
                            ++bi) {
                        llvm::Instruction *i = bi;
                        if (i == fnp) {
                            ++bi;
                            if (bi != be) {
                                builder.SetInsertPoint(bi);
                                break;
                            } else {
                                break;
                            }
                        }
                    }
                }
            } else {
                llvm::BasicBlock::iterator bi, be;

                for (bi = block_marker->begin(),
                        be = block_marker->end();
                        bi != be;
                        ++bi) {
                    llvm::Instruction *i = bi;
                    if (i == marker) {
                        ++bi;
                        if (bi != be) {
                            builder.SetInsertPoint(bi);
                            break;
                        } else {
                            break;
                        }
                    }
                }
            }

            /* Get the goto's namespace. */
            Namespace *goto_ns = (*dgiter)->ns;

            /* Create a vector of variables to destruct. This will
             * be the vector of all variables in the goto_namespace
             * and upwards, until either null (top of function) or
             * the other context is hit. Variables in the other
             * context are excluded. */
            std::vector<Element::Variable *> variables;
            Namespace *current_ns = goto_ns;
            while (current_ns != ell->ns) {
                current_ns->getVariables(&variables);
                current_ns = current_ns->parent_namespace;
                if (!current_ns) {
                    break;
                }
            }

            if (current_ns != ell->ns) {
                Namespace *ell_ns = ell->ns;
                /* Didn't hit the label's namespace on the way
                 * upwards. If the label's namespace, or any namespace
                 * in which the label's namespace is located, has a
                 * variable declaration with an index smaller than
                 * that of the label, then the goto is going to
                 * cross that variable declaration, in which case
                 * you want to bail. (This is because the
                 * declaration may result in a destructor being
                 * called on scope close, and the variable may not be
                 * initialised when the goto is reached.) */
                std::vector<Element::Variable *> vars_before;
                std::vector<Element::Variable *> real_vars_before;
                while (ell_ns) {
                    vars_before.clear();
                    ell_ns->getVarsBeforeIndex(ell->index, &vars_before);
                    if (vars_before.size() > 0) {
                        for
                        (std::vector<Element::Variable*>::iterator
                                vvb = vars_before.begin(),
                                vve = vars_before.end();
                                vvb != vve;
                                ++vvb) {
                            if ((*vvb)->index >= (*dgiter)->index) {
                                real_vars_before.push_back((*vvb));
                            }
                        }
                        if (real_vars_before.size() > 0) {
                            Error *e = new Error(
                                ErrorInst::Generator::GotoWillCrossDeclaration,
                                (*dgiter)->node
                            );
                            ctx->er->addError(e);
                            res = 0;
                            goto finish;
                        }
                    }
                    ell_ns = ell_ns->parent_namespace;
                }
            }

            /* Add the destructors for the collected variables. */
            for (std::vector<Element::Variable *>::iterator
                    vb = variables.begin(),
                    ve = variables.end();
                    vb != ve;
                    ++vb) {
                Element::Variable *v = (*vb);
                ParseResult pr_variable;
                ParseResult temp;
                pr_variable.set(NULL, v->type, v->value);
                bool res = gen->destructIfApplicable(&pr_variable, &builder,
                                                &temp);
                if (!res) {
                    return 0;
                }
            }

            builder.CreateBr(b);
            delete ln;
            delete (*dgiter);

            ++dgiter;
        }
    }

    /* Iterate over the blocks in the function. If the block ends
     * in a terminator, all is well. If it doesn't: if the block
     * is the last block in the function, create a return
     * instruction containing the last value evaluated, otherwise
     * create a branch instruction that moves to the next block.
     */

    bcount = 1;
    bmax   = fn->size();

    for (llvm::Function::iterator i = fn->begin(), e = fn->end();
            i != e; ++i) {
        if ((i->size() == 0) || !(i->back().isTerminator())) {
            llvm::IRBuilder<> builder(i);

            /* The underlying error here will have been
             * reported earlier, if there is no
             * last_value. */
            if (bcount == bmax) {
                if (last_value) {
                    Element::Type *got_type = last_type;

                    if (!dfn->return_type->isEqualTo(got_type)) {
                        std::string gotstr;
                        got_type->toStringProper(&gotstr);
                        std::string expstr;
                        dfn->return_type->toStringProper(&expstr);
                        Error *e = new Error(
                            ErrorInst::Generator::IncorrectReturnType,
                            last_position,
                            expstr.c_str(), gotstr.c_str()
                        );
                        ctx->er->addError(e);
                        res = 0;
                        goto finish;
                    }
                    if (dfn->return_type->base_type == dale::Type::Void) {
                        gen->scopeClose(dfn, i, NULL, true);
                        builder.CreateRetVoid();
                    } else {
                        ParseResult x;
                        x.set(i, last_type, last_value);
                        gen->scopeClose(dfn, x.block, NULL, true);
                        builder.CreateRet(x.value);
                    }
                } else {
                    gen->scopeClose(dfn, i, NULL, true);
                    builder.CreateRetVoid();
                }
            } else {
                /* Get the next block and create a branch to it. */
                ++i;
                builder.CreateBr(i);
                --i;
            }
        }
        ++bcount;
    }

    /* Iterate over the blocks in the function. Delete all
     * instructions that occur after the first terminating
     * instruction. */

    for (llvm::Function::iterator i = fn->begin(), e = fn->end();
            i != e; ++i) {
        llvm::BasicBlock::iterator bi;
        llvm::BasicBlock::iterator be;

        for (bi = i->begin(), be = i->end(); bi != be; ++bi) {
            if ((*bi).isTerminator()) {
                ++bi;
                if (bi == be) {
                    break;
                }
                int count = 0;
                while (bi != be) {
                    count++;
                    ++bi;
                }
                while (count--) {
                    i->getInstList().pop_back();
                }
                break;
            }
        }
    }


finish:

    /* Clear deferred gotos and labels. For anonymous functions,
     * these are saved and restored in Form::Proc::Inst::parse. */

    dfn->defgotos->clear();
    dfn->labels->clear();

    gen->global_blocks.pop_back();
    if (gen->global_blocks.size()) {
        gen->global_block = gen->global_blocks.back();
    } else {
        gen->global_block = NULL;
    }

    return res;
}
}
}
}
