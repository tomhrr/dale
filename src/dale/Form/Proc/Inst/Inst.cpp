#include "Inst.h"

#include <algorithm>
#include <cstdio>
#include <vector>

#include "../../../CoreForms/CoreForms.h"
#include "../../../Function/Function.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Copy/Copy.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"

#include "../../Function/Function.h"
#include "../../Initialiser/Array/Array.h"
#include "../../Initialiser/Struct/Struct.h"
#include "../../TopLevel/Import/Import.h"
#include "../../Type/Type.h"
#include "../Funcall/Funcall.h"
#include "../Token/Token.h"

using namespace dale::ErrorInst;

namespace dale {
static int anon_count = 0;

bool createAnonymousFunction(Units *units, llvm::BasicBlock *block,
                             Node *n, Function **anon_fn_ref,
                             ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    int preindex = ctx->lv_index;

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    char buf[16];
    snprintf(buf, sizeof(buf), "_anon_%d", anon_count++);
    FormFunctionParse(units, n, buf, anon_fn_ref, Linkage::Intern, 1);
    Function *anon_fn = *anon_fn_ref;

    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);

    if (error_count_begin != error_count_end) {
        return false;
    }

    Type *fn_type = new Type();
    fn_type->is_function = 1;
    fn_type->return_type = anon_fn->return_type;

    for (std::vector<Variable *>::iterator
             b = anon_fn->parameters.begin(),
             e = anon_fn->parameters.end();
         b != e; ++b) {
        fn_type->parameter_types.push_back((*b)->type);
    }

    pr->set(block, ctx->tr->getPointerType(fn_type),
            llvm::cast<llvm::Value>(anon_fn->llvm_function));

    std::vector<Variable *> vars;
    ctx->ns()->getVarsAfterIndex(preindex, &vars);
    for (std::vector<Variable *>::iterator b = vars.begin(),
                                           e = vars.end();
         b != e; ++b) {
        (*b)->index = 0;
    }

    return true;
}

bool createWantedStructLiteral(Units *units, Function *fn,
                               llvm::BasicBlock *block, Node *n,
                               bool get_address, Type *wanted_type,
                               ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    Struct *st = ctx->getStruct(wanted_type);
    assert(st && "cannot load struct");

    bool res = FormInitialiserStructParse(
        units, fn, block, n, wanted_type->struct_name.c_str(), st,
        wanted_type, get_address, pr);
    return res;
}

bool createStructLiteral(Units *units, Function *fn,
                         llvm::BasicBlock *block, Node *n,
                         bool get_address, Type *wanted_type,
                         ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    std::vector<Node *> *lst = n->list;
    const char *name = (*lst)[0]->token->str_value.c_str();

    Struct *st = ctx->getStruct(name);
    assert(st && "cannot load struct");

    Type *struct_type = FormTypeParse(units, (*lst)[0], false, false);
    assert(struct_type && "struct type does not exist");

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    bool res =
        FormInitialiserStructParse(units, fn, block, (*lst)[1], "flsp",
                                   st, struct_type, get_address, pr);
    if (!res) {
        ctx->er->popErrors(error_count_begin);
    }
    return res;
}

bool parsePotentialProcCall(Units *units, Function *fn,
                            llvm::BasicBlock *block, Node *n,
                            bool get_address, Type *wanted_type,
                            ParseResult *pr, Error **backup_error) {
    Context *ctx = units->top()->ctx;
    std::vector<Node *> *lst = n->list;
    Token *t = (*lst)[0]->token;

    Function *fn_exists =
        ctx->getFunction(t->str_value.c_str(), NULL, NULL, 0);
    Function *mac_exists =
        ctx->getFunction(t->str_value.c_str(), NULL, NULL, 1);

    if (!fn_exists && !mac_exists) {
        return true;
    }

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    /* A function (or macro) with this name exists.  Call
     * parseFunctionCall: if it returns a PR, then great.  If it
     * returns no PR, but sets macro_to_call, then pass off to
     * parseMacroCall.  If it returns no PR, then pop the errors and
     * continue, but only if there is one error, and it's related to
     * an overloaded function not being present. */

    Function *macro_to_call = NULL;

    bool res = units->top()->fp->parseFunctionCall(
        fn, block, n, t->str_value.c_str(), get_address, &macro_to_call,
        pr);
    if (res) {
        return true;
    }

    if (macro_to_call) {
        Node *mac_node =
            units->top()->mp->parseMacroCall(n, macro_to_call);
        if (!mac_node) {
            return false;
        }
        bool res =
            FormProcInstParse(units, fn, block, mac_node, get_address,
                              false, wanted_type, pr);
        delete mac_node;
        return res;
    }

    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);

    if (error_count_end != (error_count_begin + 1)) {
        return false;
    }

    *backup_error = ctx->er->popLastError();
    if ((*backup_error)->getType() != ErrorType::Error) {
        ctx->er->addError(*backup_error);
        return false;
    }

    if (((*backup_error)->instance !=
         OverloadedFunctionOrMacroNotInScope) &&
        ((*backup_error)->instance !=
         OverloadedFunctionOrMacroNotInScopeWithClosest)) {
        ctx->er->addError(*backup_error);
        return false;
    }

    return true;
}

bool parseInternal(Units *units, Function *fn, llvm::BasicBlock *block,
                   Node *n, bool get_address, bool prefixed_with_core,
                   Type *wanted_type, ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (n->is_token) {
        return FormProcTokenParse(units, fn, block, n, get_address,
                                  prefixed_with_core, wanted_type, pr);
    }

    if (n->list->size() == 0) {
        Error *e = new Error(NoEmptyLists, n);
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *> *lst = n->list;
    Node *adjusted = n;

    Node *first = (*lst)[0];
    if (!first->is_token) {
        first = units->top()->mp->parsePotentialMacroCall(first);
        if (!first) {
            return false;
        }
        lst = new std::vector<Node *>(*(n->list));
        adjusted = new Node(lst);
        (*lst)[0] = first;
    }

    /* If the first node is a token, and it equals "fn", then
     * create an anonymous function and return a pointer to it. */

    if (first->is_token && !first->token->str_value.compare("fn")) {
        Function *anon_fn = NULL;
        bool result = createAnonymousFunction(units, block, adjusted,
                                              &anon_fn, pr);
        if (!result) {
            return false;
        }
        if (anon_fn->cto && (!fn->cto && !fn->is_macro)) {
            Error *e = new Error(CTOAnonymousFromNonCTO, adjusted);
            ctx->er->addError(e);
            return false;
        }
        return result;
    }

    /* If wanted_type is present and is a struct, then use
     * FormInitialiserStructParse, if the first list element is a list.
     */

    if (wanted_type && (wanted_type->struct_name.size()) &&
        (!first->is_token)) {
        return createWantedStructLiteral(units, fn, block, adjusted,
                                         get_address, wanted_type, pr);
    }

    if (!first->is_token) {
        Node *n2 = units->top()->mp->parsePotentialMacroCall(first);
        if (n2 && n2->is_token) {
            first = n2;
        } else {
            ParseResult fp_pr;
            bool res = FormProcInstParse(
                units, fn, block, n2, get_address, false, NULL, &fp_pr);
            if (res && fp_pr.type->points_to &&
                fp_pr.type->points_to->is_function) {
                return units->top()->fp->parseFunctionPointerCall(
                    fn, adjusted, &fp_pr, 1, NULL, pr);
            } else {
                Error *e = new Error(
                    FirstListElementMustBeAtomOrMacroOrFP, adjusted);
                ctx->er->addError(e);
                return false;
            }
        }
    }

    Token *t = first->token;
    if (t->type != TokenType::String) {
        Error *e = new Error(FirstListElementMustBeSymbol, adjusted);
        ctx->er->addError(e);
        return false;
    }

    /* If the first element matches a struct name, then make a
     * struct literal from the remainder of the form. */

    Struct *st = ctx->getStruct(t->str_value.c_str());
    if (st && (lst->size() == 2)) {
        bool res = createStructLiteral(units, fn, block, adjusted,
                                       get_address, wanted_type, pr);
        if (res) {
            return true;
        }
    }

    /* If the first element is 'array', and an array type has been
     * requested, handle that specially. */

    if (wanted_type && wanted_type->is_array &&
        (!strcmp(t->str_value.c_str(), "array"))) {
        int size;
        bool res = FormInitialiserArrayParse(units, fn, block, adjusted,
                                             wanted_type, get_address,
                                             &size, pr);
        return res;
    }

    /* Check that a macro/function exists with the relevant name.
       This can be checked by passing NULL in place of the types.
       If the form begins with 'core', though, skip this part. If
       any errors occur here, then pop them from the reporter and
       keep going - only if the rest of the function fails, should
       the errors be restored. */

    Error *backup_error = NULL;

    prefixed_with_core = !(t->str_value.compare("core"));

    if (!prefixed_with_core) {
        pr->type = NULL;
        bool res = parsePotentialProcCall(units, fn, block, adjusted,
                                          get_address, wanted_type, pr,
                                          &backup_error);
        if (!res) {
            return false;
        } else if (pr->type) {
            return true;
        }
    } else {
        if (!ctx->er->assertArgNums("core", adjusted, 1, -1)) {
            return false;
        }

        std::vector<Node *> *but_one = new std::vector<Node *>;
        but_one->insert(but_one->begin(), lst->begin() + 1, lst->end());
        lst = but_one;

        adjusted = new Node(but_one);
        first = (*lst)[0];

        if (!first->is_token) {
            first = units->top()->mp->parsePotentialMacroCall(first);
            if (!first) {
                return false;
            }
        }
        if (!first->is_token) {
            Error *e = new Error(UnexpectedElement, first, "atom",
                                 "core form name", "list");
            ctx->er->addError(e);
            return false;
        }

        t = first->token;
        if (t->type != TokenType::String) {
            Error *e =
                new Error(UnexpectedElement, first, "symbol",
                          "core form name", first->token->tokenType());
            ctx->er->addError(e);
            return false;
        }
    }

    /* Standard core forms. */

    standard_core_form_t core_fn =
        CoreForms::getStandard(t->str_value.c_str());
    if (core_fn) {
        return core_fn(units, fn, block, adjusted, get_address,
                       prefixed_with_core, pr);
    }

    /* Macro core forms. */

    macro_core_form_t core_mac =
        CoreForms::getMacro(t->str_value.c_str());
    if (core_mac) {
        Node *new_node = core_mac(ctx, adjusted);
        if (!new_node) {
            return false;
        }
        return FormProcInstParse(units, fn, block, new_node,
                                 get_address, false, wanted_type, pr);
    }

    if (!strcmp(t->str_value.c_str(), "import")) {
        pr->set(block, NULL, NULL);
        return FormTopLevelImportParse(units, adjusted);
    }

    /* Not core form/macro, nor function. If the string token is
     * 'destroy', then treat this as a no-op (because it's annoying to
     * have to check, in macros, whether destroy happens to be
     * implemented over a particular type). */

    if (!(t->str_value.compare("destroy"))) {
        pr->set(block, ctx->tr->type_void, NULL);
        return true;
    }

    /* If a backup error was set earlier, then return it now. */

    if (backup_error) {
        ctx->er->addError(backup_error);
        return false;
    }

    /* If nothing else is applicable: parse the first element of the
     * list.  If it is a function pointer, then go to funcall.  If it
     * is a struct, see if it is a function object. */

    int last_error_count = ctx->er->getErrorTypeCount(ErrorType::Error);
    ParseResult try_fp;
    bool res =
        FormProcInstParse(units, fn, block, (*lst)[0], get_address,
                          false, wanted_type, &try_fp);
    if (!res) {
        /* If this fails, and there is one extra error, and the error
         * is 'variable not in scope', then change it to 'not in
         * scope' (it could be intended as either a variable, a macro
         * or a fn). */
        int new_error_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);
        if (new_error_count == (last_error_count + 1)) {
            Error *e = ctx->er->popLastError();
            if (e->instance == VariableNotInScope) {
                e->instance = NotInScope;
            }
            ctx->er->addError(e);
        }
        return false;
    }

    block = try_fp.block;
    if (try_fp.type->points_to && try_fp.type->points_to->is_function) {
        Node *funcall_str_node = new Node("funcall");
        funcall_str_node->filename = ctx->er->current_filename;
        lst->insert(lst->begin(), funcall_str_node);
        bool res = FormProcFuncallParse(units, fn, block, adjusted,
                                        get_address, false, pr);
        return res;
    }

    /* Try (invoke lst), and return the result if it is successful. */

    std::vector<Node *> *invoke_lst = new std::vector<Node *>();
    Token *invoke = new Token(TokenType::String);
    invoke->str_value.append("invoke");
    Node *token_node = new Node(invoke);
    invoke_lst->push_back(token_node);
    std::copy(lst->begin(), lst->end(),
              std::back_inserter(*invoke_lst));
    Node invoke_node(invoke_lst);
    (*lst->begin())->copyMetaTo(token_node);
    (*lst->begin())->copyMetaTo(&invoke_node);

    last_error_count = ctx->er->getErrorTypeCount(ErrorType::Error);
    res = FormProcInstParse(units, fn, block, &invoke_node, false,
                            false, NULL, pr);
    invoke_lst->erase(invoke_lst->begin() + 1, invoke_lst->end());
    if (res) {
        return true;
    }

    /* If the call is not successful, but there is an 'invoke'
     * function/macro that has a matching first argument, then
     * return that error instead. */
    Error *err = ctx->er->popLastError();
    if (err->instance ==
        OverloadedFunctionOrMacroNotInScopeWithClosestFirstMatches) {
        ctx->er->addError(err);
        return false;
    }

    ctx->er->popErrors(last_error_count);

    Error *e = new Error(NotInScope, adjusted, t->str_value.c_str());
    ctx->er->addError(e);
    return false;
}

bool FormProcInstParse(Units *units, Function *fn,
                       llvm::BasicBlock *block, Node *node,
                       bool get_address, bool prefixed_with_core,
                       Type *wanted_type, ParseResult *pr,
                       bool no_copy) {
    bool res = parseInternal(units, fn, block, node, get_address,
                             prefixed_with_core, wanted_type, pr);

    if (!res) {
        return false;
    }
    if (fn->is_setf_fn) {
        return true;
    }

    return true;
}
}
