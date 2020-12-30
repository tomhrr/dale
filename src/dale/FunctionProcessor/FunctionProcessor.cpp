#include "FunctionProcessor.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "../Form/Proc/Def/Def.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Operation/Cast/Cast.h"
#include "../Operation/Coerce/Coerce.h"
#include "../Operation/Copy/Copy.h"
#include "../Operation/Destruct/Destruct.h"
#include "../SavePoint/SavePoint.h"
#include "../Utils/Utils.h"
#include "../llvmUtils/llvmUtils.h"

#define IMPLICIT 1

using namespace dale::ErrorInst;

namespace dale {
FunctionProcessor::FunctionProcessor(Units *units) {
    this->units = units;
}

FunctionProcessor::~FunctionProcessor() {}

void processRetval(Context *ctx, Type *return_type,
                   llvm::BasicBlock *block, ParseResult *pr,
                   std::vector<llvm::Value *> *call_args) {
    if (!return_type->is_retval) {
        return;
    }

    pr->do_not_destruct = 1;
    pr->do_not_copy_with_setf = 1;
    pr->retval_used = true;

    llvm::IRBuilder<> builder(block);

    if (pr->retval) {
        if (pr->retval_requires_init) {
            initialise(ctx, &builder, return_type, pr->retval,
                       getInitFn(ctx, return_type));
            call_args->push_back(pr->retval);
            return;
        }
    }

    llvm::Type *et = ctx->toLLVMType(return_type, NULL, false, false);
    if (!et) {
        return;
    }
    llvm::Value *retval_ptr =
        llvm::cast<llvm::Value>(builder.CreateAlloca(et));
    initialise(ctx, &builder, return_type, retval_ptr,
               getInitFn(ctx, return_type));
    call_args->push_back(retval_ptr);
    if (!pr->retval) {
        pr->retval = retval_ptr;
        pr->retval_type = ctx->tr->getPointerType(return_type);
    }
}

bool checkArgumentCount(Type *fn_ptr, Node *n, int num_args,
                        ErrorReporter *er) {
    int num_required_args = fn_ptr->numberOfRequiredArgs();

    if (fn_ptr->isVarArgs()) {
        if (num_args < num_required_args) {
            Error *e = new Error(IncorrectMinimumNumberOfArgs, n,
                                 "function pointer call",
                                 num_required_args, num_args);
            er->addError(e);
            return false;
        }
    } else {
        if (num_args != num_required_args) {
            Error *e = new Error(IncorrectNumberOfArgs, n,
                                 "function pointer call",
                                 num_required_args, num_args);
            er->addError(e);
            return false;
        }
    }

    return true;
}

bool processReferenceTypes(std::vector<llvm::Value *> *call_args,
                           std::vector<llvm::Value *> *call_args_final,
                           std::vector<Node *> *call_arg_nodes,
                           std::vector<ParseResult> *call_arg_prs,
                           Function *dfn,
                           std::vector<Type *> *parameter_types,
                           Context *ctx, bool args_cast,
                           int extra_call_args_size) {
    int caps = call_arg_prs->size();
    int pts = parameter_types->size();
    int limit = (caps > pts ? pts : caps);
    ParseResult refpr;
    for (int i = extra_call_args_size; i < limit; i++) {
        Type *pt = (*parameter_types)[i];
        ParseResult *arg_refpr = &((*call_arg_prs)[i]);
        if (pt->is_reference) {
            /* todo: this is handled as part of function resolution,
             * so it should not be necessary to have this check
             * anymore, which in turn will allow this branch and the
             * one below to be merged. */
            if (!pt->is_const && !arg_refpr->value_is_lvalue) {
                Error *e = new Error(CannotTakeAddressOfNonLvalue,
                                     (*call_arg_nodes)[i]);
                ctx->er->addError(e);
                return false;
            }
            bool res = arg_refpr->getAddressOfValue(ctx, &refpr);
            if (!res) {
                return false;
            }
            (*call_args_final)[i] = refpr.getValue(ctx);
        } else if (pt->is_rvalue_reference) {
            bool res = arg_refpr->getAddressOfValue(ctx, &refpr);
            if (!res) {
                return false;
            }
            (*call_args_final)[i] = refpr.getValue(ctx);
        } else {
            std::vector<Type *> param_types;
            param_types.push_back(ctx->tr->getPointerType(pt));
            param_types.push_back(pt);
            std::vector<bool> lvalues;
            lvalues.push_back(true);
            lvalues.push_back(false);
            Function *or_setf_move = ctx->getFunction(
                "setf-move-init", &param_types, NULL, false, &lvalues);

            if (!arg_refpr->value_is_lvalue && or_setf_move) {
                llvm::IRBuilder<> builder(arg_refpr->block);
                std::vector<llvm::Value *> or_call_args;
                llvm::Value *dst_ptr =
                    llvm::cast<llvm::Value>(builder.CreateAlloca(
                        ctx->toLLVMType(pt, NULL, false, false)));
                or_call_args.push_back(dst_ptr);
                bool res = arg_refpr->getAddressOfValue(ctx, &refpr);
                if (!res) {
                    return false;
                }
                or_call_args.push_back(refpr.getValue(ctx));

                builder.CreateCall(
                    or_setf_move->llvm_function,
                    llvm::ArrayRef<llvm::Value *>(or_call_args));
                (*call_args_final)[i] = builder.CreateLoad(dst_ptr);
            } else if (!args_cast) {
                bool res =
                    Operation::Copy(ctx, dfn, (*call_arg_nodes)[i],
                                    arg_refpr, arg_refpr);
                if (!res) {
                    return false;
                }
                (*call_args_final)[i] = arg_refpr->getValue(ctx);
            }
        }
    }

    return true;
}

bool FunctionProcessor::parseFunctionPointerCall(
    Function *dfn, Node *n, ParseResult *fn_ptr_pr, int skip,
    std::vector<llvm::Value *> *extra_call_args, ParseResult *pr) {
    Type *fn_ptr = fn_ptr_pr->type->points_to;

    llvm::FunctionType *fn_type =
        llvm::dyn_cast<llvm::FunctionType>(
            units->top()->ctx->toLLVMType(
                fn_ptr_pr->type->points_to,
                n, true, false, false
            )
        );

    llvm::BasicBlock *block = fn_ptr_pr->block;
    std::vector<llvm::Value *> empty;
    if (!extra_call_args) {
        extra_call_args = &empty;
    }

    int num_args = n->list->size() - skip + extra_call_args->size();
    bool res =
        checkArgumentCount(fn_ptr, n, num_args, units->top()->ctx->er);
    if (!res) {
        return false;
    }

    std::vector<llvm::Value *> call_args;
    std::vector<ParseResult> call_arg_prs;
    std::vector<Node *> call_arg_nodes;

    bool args_coerced = false;
    int arg_count = 1;

    std::copy(extra_call_args->begin(), extra_call_args->end(),
              std::back_inserter(call_args));

    std::vector<Type *>::iterator param_iter =
        fn_ptr->parameter_types.begin() + extra_call_args->size();

    for (std::vector<Node *>::iterator b = n->list->begin() + skip,
                                       e = n->list->end();
         b != e; ++b) {
        ParseResult arg_pr;
        bool res = FormProcInstParse(units, dfn, block, (*b), false,
                                     false, NULL, &arg_pr, true);
        if (!res) {
            return false;
        }

        call_arg_prs.push_back(arg_pr);
        call_arg_nodes.push_back(*b);
        block = arg_pr.block;

        /* If the parsed argument is not of the correct type, attempt
         * to coerce the type to the correct type. */
        if ((param_iter != fn_ptr->parameter_types.end()) &&
            (!(arg_pr.type->isEqualTo((*param_iter), 1))) &&
            ((*param_iter)->base_type != BaseType::VarArgs)) {
            ParseResult coerce_pr;
            bool coerce_result = Operation::Coerce(
                units->top()->ctx, block,
                arg_pr.getValue(units->top()->ctx), arg_pr.type,
                (*param_iter), &coerce_pr);
            if (!coerce_result) {
                std::string wanted_type;
                std::string got_type;
                (*param_iter)->toString(&wanted_type);
                arg_pr.type->toString(&got_type);

                Error *e = new Error(
                    IncorrectArgType, (*b), "function pointer call",
                    wanted_type.c_str(), arg_count, got_type.c_str());
                units->top()->ctx->er->addError(e);
                return false;
            } else {
                args_coerced = true;
                call_args.push_back(
                    coerce_pr.getValue(units->top()->ctx));
            }
        } else {
            call_args.push_back(arg_pr.getValue(units->top()->ctx));
        }

        if (param_iter != fn_ptr->parameter_types.end()) {
            ++param_iter;
            /* Skip the varargs type. */
            if (param_iter != fn_ptr->parameter_types.end()) {
                if ((*param_iter)->base_type == BaseType::VarArgs) {
                    ++param_iter;
                }
            }
        }
    }

    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args_final = call_args;
    res = processReferenceTypes(
        &call_args, &call_args_final, &call_arg_nodes, &call_arg_prs,
        dfn, &(fn_ptr->parameter_types), units->top()->ctx,
        args_coerced, extra_call_args->size());
    if (!res) {
        return false;
    }

    processRetval(units->top()->ctx, fn_ptr->return_type, block, pr,
                  &call_args_final);

    llvm::Value *call_res = builder.CreateCall(
        fn_type,
        fn_ptr_pr->getValue(units->top()->ctx),
        llvm::ArrayRef<llvm::Value *>(call_args_final));

    pr->set(block, fn_ptr->return_type, call_res);

    if (fn_ptr->return_type->is_retval) {
        if (call_args_final.back() != pr->retval) {
            ParseResult x;
            x.value_is_lvalue = false;
            x.set(block, fn_ptr->return_type,
                  builder.CreateLoad(call_args_final.back()));
            x.address_of_value = call_args_final.back();
            x.type_of_address_of_value =
                units->top()->ctx->tr->getPointerType(
                    fn_ptr->return_type);
            storeValue(units->top()->ctx, n, fn_ptr->return_type,
                       &builder, pr->retval, &x);
        }
    }

    fn_ptr_pr->block = pr->block;
    ParseResult destruct_pr;
    res =
        Operation::Destruct(units->top()->ctx, fn_ptr_pr, &destruct_pr);
    if (!res) {
        return false;
    }
    pr->block = destruct_pr.block;
    pr->do_not_destruct = false;

    return true;
}

bool isUnoverloadedMacro(Units *units, const char *name,
                         std::vector<Node *> *lst,
                         Function **macro_to_call) {
    std::map<std::string, std::vector<Function *> *>::iterator iter;
    Function *fn = NULL;
    for (std::vector<NSNode *>::reverse_iterator
             rb = units->top()->ctx->used_ns_nodes.rbegin(),
             re = units->top()->ctx->used_ns_nodes.rend();
         rb != re; ++rb) {
        iter = (*rb)->ns->functions.find(name);
        if (iter != (*rb)->ns->functions.end()) {
            fn = (*iter->second)[0];
            break;
        }
    }
    if (fn && fn->is_macro) {
        /* If the third argument is either non-existent, or a (p
         * DNode) (because typed arguments must appear before the
         * first (p DNode) argument), then short-circuit, so long
         * as the argument count is ok. */
        std::vector<Variable *>::iterator b =
            (fn->parameters.begin() + 1);
        if ((b == fn->parameters.end()) ||
            (*b)->type->isEqualTo(units->top()->ctx->tr->type_pdnode)) {
            bool use = false;
            int size = lst->size();
            if (fn->isVarArgs()) {
                use = ((fn->numberOfRequiredArgs() - 1) <= (size - 1));
            } else {
                use = ((fn->numberOfRequiredArgs() - 1) == (size - 1));
            }
            if (use) {
                *macro_to_call = fn;
                return true;
            }
        }
    }

    return false;
}

bool processExternCFunction(Context *ctx, const char *name, Node *n,
                            Function **fn_ptr, llvm::BasicBlock *block,
                            std::vector<llvm::Value *> *call_args,
                            std::vector<Type *> *call_arg_types,
                            bool *args_cast) {
    ErrorReporter *er = ctx->er;
    std::vector<llvm::Value *> call_args_final;
    std::vector<Type *> call_arg_types_final;

    Function *fn = ctx->getFunction(name, NULL, NULL, 0);

    std::vector<Variable *> parameters = fn->parameters;
    std::vector<Type *> parameter_types;
    for (std::vector<Variable *>::iterator b = parameters.begin(),
                                           e = parameters.end();
         b != e; ++b) {
        parameter_types.push_back((*b)->type);
    }

    std::string expected_args;
    typesToString(&parameter_types, &expected_args);

    std::string provided_args;
    typesToString(call_arg_types, &provided_args);

    int size = call_args->size();
    int required = fn->numberOfRequiredArgs();

    if (size < required) {
        Error *e =
            new Error(FunctionNotInScope, n, name,
                      provided_args.c_str(), expected_args.c_str());
        er->addError(e);
        return false;
    }
    if (!fn->isVarArgs() && size != required) {
        Error *e =
            new Error(FunctionNotInScope, n, name,
                      provided_args.c_str(), expected_args.c_str());
        er->addError(e);
        return false;
    }

    for (int i = 0; i < required; i++) {
        Variable *parameter = parameters[i];
        llvm::Value *value = (*call_args)[i];
        Type *type = (*call_arg_types)[i];

        if (type->isEqualTo(parameter->type, 1)) {
            call_args_final.push_back(value);
            call_arg_types_final.push_back(type);
            continue;
        }
        if (!parameter->type->isIntegerType() &&
            parameter->type->base_type != BaseType::Bool) {
            Error *e =
                new Error(FunctionNotInScope, n, name,
                          provided_args.c_str(), expected_args.c_str());
            er->addError(e);
            return false;
        }
        if (!type->isIntegerType() &&
            type->base_type != BaseType::Bool) {
            Error *e =
                new Error(FunctionNotInScope, n, name,
                          provided_args.c_str(), expected_args.c_str());
            er->addError(e);
            return false;
        }

        ParseResult cast_result;
        bool res =
            Operation::Cast(ctx, block, value, type, parameter->type, n,
                            IMPLICIT, &cast_result);
        if (!res) {
            Error *e =
                new Error(FunctionNotInScope, n, name,
                          provided_args.c_str(), expected_args.c_str());
            er->addError(e);
            return false;
        }

        block = cast_result.block;
        call_args_final.push_back(cast_result.getValue(ctx));
        call_arg_types_final.push_back(cast_result.type);
    }

    *call_args = call_args_final;
    *call_arg_types = call_arg_types_final;
    *args_cast = true;

    *fn_ptr = fn;

    return true;
}

bool processVarArgsFunction(Context *ctx, Function *fn,
                            std::vector<llvm::Value *> *call_args,
                            std::vector<Type *> *call_arg_types,
                            llvm::IRBuilder<> *builder) {
    int required = fn->numberOfRequiredArgs();
    int call_args_count = call_args->size();

    Type *type_int = ctx->tr->type_int;
    Type *type_uint = ctx->tr->type_uint;
    llvm::Type *llvm_type_int = ctx->toLLVMType(type_int, NULL, false);
    llvm::Type *llvm_type_uint =
        ctx->toLLVMType(type_uint, NULL, false);

    for (int i = required; i < call_args_count; i++) {
        llvm::Value *value = (*call_args)[i];
        Type *type = (*call_arg_types)[i];

        if (type->base_type == BaseType::Float) {
            value = builder->CreateFPExt(
                value, llvm::Type::getDoubleTy(*getContext()));
            type = ctx->tr->type_double;
        } else if (type->isIntegerType()) {
            int real_size =
                ctx->nt->internalSizeToRealSize(type->getIntegerSize());

            if (real_size < ctx->nt->getNativeIntSize()) {
                if (type->isSignedIntegerType()) {
                    value = builder->CreateSExt(value, llvm_type_int);
                    type = ctx->tr->type_int;
                } else {
                    value = builder->CreateZExt(value, llvm_type_uint);
                    type = ctx->tr->type_uint;
                }
            }
        }

        (*call_args)[i] = value;
        (*call_arg_types)[i] = type;
    }

    return true;
}

void addNotFoundError(std::vector<Type *> *call_arg_types,
                      const char *name, Node *n, Function *closest_fn,
                      bool has_others, ErrorReporter *er) {
    if (!has_others) {
        Error *e = new Error(NotInScope, n, name);
        er->addError(e);
        return;
    }

    std::string args;
    typesToString(call_arg_types, &args);

    if (closest_fn) {
        Type *first_passed =
            (call_arg_types->size() ? call_arg_types->at(0) : NULL);
        Type *first_closest =
            (closest_fn->is_macro)
                ? (closest_fn->parameters.size() > 1)
                      ? closest_fn->parameters.at(1)->type
                      : NULL
                : (closest_fn->parameters.size())
                      ? closest_fn->parameters.at(0)->type
                      : NULL;
        bool first_matches =
            (first_passed && first_closest &&
             first_passed->isEqualTo(first_closest, true));
        int error_type =
            first_matches
                ? OverloadedFunctionOrMacroNotInScopeWithClosestFirstMatches
                : OverloadedFunctionOrMacroNotInScopeWithClosest;
        std::string expected;
        typesToString(closest_fn->parameters.begin() +
                          (closest_fn->is_macro ? 1 : 0),
                      closest_fn->parameters.end(), &expected);
        Error *e = new Error(error_type, n, name, args.c_str(),
                             expected.c_str());
        er->addError(e);
    } else {
        Error *e = new Error(OverloadedFunctionOrMacroNotInScope, n,
                             name, args.c_str());
        er->addError(e);
    }
}

bool FunctionProcessor::parseFunctionCall(
    Function *dfn, llvm::BasicBlock *block, Node *n, const char *name,
    bool get_address, Function **macro_to_call, ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    ErrorReporter *er = ctx->er;

    if (get_address) {
        Error *e = new Error(CannotTakeAddressOfNonLvalue, n);
        er->addError(e);
        return false;
    }

    std::vector<Node *> *lst = n->list;
    Node *proc_name_node = (*lst)[0];
    if (!proc_name_node->is_token) {
        Error *e =
            new Error(FirstListElementMustBeAtom, proc_name_node);
        er->addError(e);
        return false;
    }

    Token *proc_name_token = proc_name_node->token;
    if (proc_name_token->type != TokenType::String) {
        Error *e =
            new Error(FirstListElementMustBeSymbol, proc_name_node);
        er->addError(e);
        return false;
    }

    const char *proc_name = proc_name_token->str_value.c_str();

    /* The processing further down is only required when the
     * function/macro name is overloaded.  For now, short-circuit for
     * macros that are not overloaded, since those are most common,
     * and avoiding the later work in those cases makes things much
     * quicker. */

    if (!ctx->isOverloadedFunction(proc_name)) {
        if (isUnoverloadedMacro(units, name, lst, macro_to_call)) {
            return false;
        }
    }

    std::vector<Error *> errors;

    std::vector<llvm::Value *> call_args;
    std::vector<Node *> call_arg_nodes;
    std::vector<ParseResult> call_arg_prs;
    std::vector<Type *> call_arg_types;

    /* If the underlying procedure is a macro, then the state prior to
     * argument evaluation needs to be restored before the macro is
     * called, hence the save point. */

    SavePoint sp(ctx, dfn, block);

    std::vector<Type *> array_types;

    for (std::vector<Node *>::iterator b = (lst->begin() + 1),
                                       e = lst->end();
         b != e; ++b) {
        call_arg_nodes.push_back(*b);
        int error_count = er->getErrorTypeCount(ErrorType::Error);

        ParseResult arg_pr;
        bool res = FormProcInstParse(units, dfn, block, (*b), false,
                                     false, NULL, &arg_pr, true);

        int diff =
            er->getErrorTypeCount(ErrorType::Error) - error_count;

        if (!res || diff) {
            /* May be a macro call (could be an unparseable
             * argument).  Pop and store errors for the time being
             * and treat this argument as a (p DNode). */

            if (diff) {
                errors.insert(errors.end(),
                              er->errors.begin() + error_count,
                              er->errors.end());
                er->errors.erase(er->errors.begin() + error_count,
                                 er->errors.end());
            }

            call_args.push_back(NULL);
            array_types.push_back(NULL);
            call_arg_types.push_back(ctx->tr->type_pdnode);
            continue;
        }

        block = arg_pr.block;
        if (arg_pr.type->is_array) {
            array_types.push_back(arg_pr.type);
            arg_pr = ParseResult(block, arg_pr.type_of_address_of_value,
                                 arg_pr.address_of_value);
        } else if (arg_pr.type_of_address_of_value &&
                   arg_pr.type_of_address_of_value->points_to &&
                   (arg_pr.type_of_address_of_value->points_to
                        ->is_array)) {
            array_types.push_back(
                arg_pr.type_of_address_of_value->points_to);
        } else {
            array_types.push_back(NULL);
        }
        call_args.push_back(arg_pr.getValue(ctx));
        call_arg_types.push_back(arg_pr.type);
        call_arg_prs.push_back(arg_pr);
    }

    /* Retrieve the function (if present) from the context, based on
     * the argument types. */

    std::vector<bool> lvalues;
    for (std::vector<ParseResult>::iterator b = call_arg_prs.begin(),
                                            e = call_arg_prs.end();
         b != e; ++b) {
        lvalues.push_back((*b).value_is_lvalue);
    }

    Function *closest_fn = NULL;
    Function *fn =
        ctx->getFunction(proc_name, &call_arg_types, &closest_fn, false,
                         &lvalues, &array_types);

    /* If the function is a macro, set macro_to_call and return false.
     * (It's the caller's responsibility to handle processing of
     * macros.) */

    if (fn && fn->is_macro) {
        sp.restore();
        *macro_to_call = fn;
        return false;
    }

    /* If the function is not a macro, and errors were encountered
     * during argument processing, then this function has been
     * loaded in error (it will be a normal function taking a (p
     * DNode) argument, but the argument is not a real (p DNode)
     * value).  Replace all the errors and return NULL. */

    if (errors.size() && fn && !fn->is_macro) {
        for (std::vector<Error *>::iterator b = errors.begin(),
                                            e = errors.end();
             b != e; ++b) {
            er->addError(*b);
        }
        return false;
    }

    bool args_cast = false;

    if (!fn) {
        /* If no function was found, and there are errors related to
         * argument parsing, then push those errors back onto the
         * reporter and return.  (May change this later to be a bit
         * more friendly.  If there are any macros or functions with
         * the same name, this should show the overload failure,
         * rather than the parsing failure errors). */
        if (errors.size()) {
            for (std::vector<Error *>::iterator b = errors.begin(),
                                                e = errors.end();
                 b != e; ++b) {
                er->addError(*b);
            }
            return false;
        }

        /* If there's an extern-C function with this name, try casting
         * things accordingly. */
        if (ctx->existsExternCFunction(proc_name)) {
            bool res = processExternCFunction(
                ctx, proc_name, n, &fn, block, &call_args,
                &call_arg_types, &args_cast);
            if (!res) {
                return false;
            }
        } else if (!strcmp(proc_name, "destroy")) {
            /* Return a no-op ParseResult if the function name is
             * 'destroy' and no candidate exists, because it's tedious
             * to have to check in generic code whether a particular
             * value can be destroyed or not. */
            pr->set(block, ctx->tr->type_void, NULL);
            return true;
        } else {
            bool has_others = ctx->existsNonExternCFunction(proc_name);

            addNotFoundError(&call_arg_types, proc_name, n, closest_fn,
                             has_others, er);
            return false;
        }
    }

    /* If the function is CTO, but this function is not, report an
     * error. */
    if (fn->cto && !(dfn->cto || dfn->is_macro)) {
        Error *e = new Error(CTOFromNonCTO, n);
        er->addError(e);
        return false;
    }

    llvm::IRBuilder<> builder(block);

    /* If this function is varargs, promote arguments accordingly. */

    if (fn->isVarArgs()) {
        args_cast = true;
        processVarArgsFunction(ctx, fn, &call_args, &call_arg_types,
                               &builder);
    }

    /* Iterate over the types of the found function. For the reference
     * types, replace the call argument with its address. */

    std::vector<Type *> parameter_types;
    for (std::vector<Variable *>::iterator b = fn->parameters.begin(),
                                           e = fn->parameters.end();
         b != e; ++b) {
        parameter_types.push_back((*b)->type);
    }
    std::vector<llvm::Value *> call_args_final = call_args;
    bool res = processReferenceTypes(
        &call_args, &call_args_final, &call_arg_nodes, &call_arg_prs,
        dfn, &parameter_types, ctx, args_cast, 0);
    if (!res) {
        return false;
    }

    /* Make the necessary retval adjustments. */
    processRetval(ctx, fn->return_type, block, pr, &call_args_final);

    /* Finally: actually call the function. */
    llvm::Value *call_res = builder.CreateCall(
        fn->llvm_function,
        llvm::ArrayRef<llvm::Value *>(call_args_final));
    pr->set(block, fn->return_type, call_res);

    if (fn->return_type->is_retval) {
        if (call_args_final.back() != pr->retval) {
            ParseResult x;
            x.value_is_lvalue = false;
            x.set(block, fn->return_type,
                  builder.CreateLoad(call_args_final.back()));
            x.address_of_value = call_args_final.back();
            x.type_of_address_of_value =
                units->top()->ctx->tr->getPointerType(fn->return_type);
            storeValue(units->top()->ctx, n, fn->return_type, &builder,
                       pr->retval, &x);
        }
    }

    /* If the return type of the function is one that should be copied
     * with an overridden setf, that will occur in the function, so
     * prevent the value from being re-copied here (because no
     * corresponding destructor call will occur). */

    pr->do_not_copy_with_setf = 1;
    pr->do_not_destruct = false;

    return true;
}
}
