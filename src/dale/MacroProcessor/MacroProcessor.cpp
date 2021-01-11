#include "MacroProcessor.h"

#include <cstdio>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../Form/Macro/ArrayDeref/ArrayDeref.h"
#include "../Form/Macro/DerefStruct/DerefStruct.h"
#include "../Form/Macro/DerefStructDeref/DerefStructDeref.h"
#include "../Form/Macro/Setv/Setv.h"
#include "../Form/Macro/StructDeref/StructDeref.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Node/Node.h"
#include "../llvmUtils/llvmUtils.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/Debug.h"

#define eq(str) !strcmp(macro_name, str)

using namespace dale::ErrorInst;

namespace dale {
llvm::Function *pool_free_fn;

MacroProcessor::MacroProcessor(Units *units, Context *ctx,
                               llvm::ExecutionEngine *ee) {
    this->units = units;
    this->ctx = ctx;
    this->ee = ee;
}

MacroProcessor::~MacroProcessor() {}

void MacroProcessor::setPoolfree() {
    if (!pool_free_fn) {
        pool_free_fn =
            ctx->getFunction("pool-free", NULL, NULL, 0)->llvm_function;
    }
}

std::map<std::string, uint64_t> function_map;

DNode *callMacro(uint64_t address, std::vector<void *> *args) {
    DNode *result_dnode;
    if (args->size() == 1) {
        typedef void *(*MFN)(void *);
        MFN macro_function = (MFN)address;
        result_dnode =
            reinterpret_cast<DNode *>(macro_function((*args)[0]));
    } else if (args->size() == 2) {
        typedef void *(*MFN)(void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(
            macro_function((*args)[0], (*args)[1]));
    } else if (args->size() == 3) {
        typedef void *(*MFN)(void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(
            macro_function((*args)[0], (*args)[1], (*args)[2]));
    } else if (args->size() == 4) {
        typedef void *(*MFN)(void *, void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3]));
    } else if (args->size() == 5) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(
            macro_function((*args)[0], (*args)[1], (*args)[2],
                           (*args)[3], (*args)[4]));
    } else if (args->size() == 6) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(
            macro_function((*args)[0], (*args)[1], (*args)[2],
                           (*args)[3], (*args)[4], (*args)[5]));
    } else if (args->size() == 7) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6]));
    } else if (args->size() == 8) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7]));
    } else if (args->size() == 9) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8]));
    } else if (args->size() == 10) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8],
            (*args)[9]));
    } else if (args->size() == 11) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *,
                             void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8], (*args)[9],
            (*args)[10]));
    } else if (args->size() == 12) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *,
                             void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8], (*args)[9],
            (*args)[10], (*args)[11]));
    } else if (args->size() == 13) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *,
                             void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8], (*args)[9],
            (*args)[10], (*args)[11], (*args)[12]));
    } else if (args->size() == 14) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *,
                             void *, void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8], (*args)[9],
            (*args)[10], (*args)[11], (*args)[12], (*args)[13]));
    } else if (args->size() == 15) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8], (*args)[9],
            (*args)[10], (*args)[11], (*args)[12], (*args)[13],
            (*args)[14]));
    } else if (args->size() == 16) {
        typedef void *(*MFN)(void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *,
                             void *, void *, void *, void *, void *,
                             void *);
        MFN macro_function = (MFN)address;
        result_dnode = reinterpret_cast<DNode *>(macro_function(
            (*args)[0], (*args)[1], (*args)[2], (*args)[3], (*args)[4],
            (*args)[5], (*args)[6], (*args)[7], (*args)[8], (*args)[9],
            (*args)[10], (*args)[11], (*args)[12], (*args)[13],
            (*args)[14], (*args)[15]));
    } else {
        fprintf(stderr,
                "Internal error: need to handle more macro parameters: "
                "%lu\n",
                args->size());
        abort();
    }

    return result_dnode;
}

Node *MacroProcessor::parseMacroCall_(Node *n,
                                      Function *macro_to_call) {
    std::vector<Node *> *lst = n->list;

    Node *macro_name_node = (*lst)[0];
    if (!macro_name_node->is_token) {
        Error *e = new Error(FirstListElementMustBeAtom, n);
        ctx->er->addError(e);
        return NULL;
    }

    Token *t = macro_name_node->token;
    if (t->type != TokenType::String) {
        Error *e = new Error(FirstListElementMustBeSymbol, n);
        ctx->er->addError(e);
        return NULL;
    }

    const char *macro_name = t->str_value.c_str();
    Function *mc = macro_to_call
                       ? macro_to_call
                       : ctx->getFunction(macro_name, NULL, NULL, 1);

    if (!mc) {
        Error *e = new Error(MacroNotInScope, n, macro_name);
        ctx->er->addError(e);
        return NULL;
    }

    int size = lst->size();

    if (mc->isVarArgs()) {
        if (size < mc->numberOfRequiredArgs()) {
            Error *e =
                new Error(IncorrectMinimumNumberOfArgs, n, macro_name,
                          (mc->numberOfRequiredArgs() - 1), (size - 1));
            ctx->er->addError(e);
            return NULL;
        }
    } else {
        if (size != mc->numberOfRequiredArgs()) {
            Error *e =
                new Error(IncorrectNumberOfArgs, n, macro_name,
                          (mc->numberOfRequiredArgs() - 1), (size - 1));
            ctx->er->addError(e);
            return NULL;
        }
    }

    DNode *macro_args[256];
    int macro_args_count = 0;

    for (std::vector<Node *>::iterator b = lst->begin() + 1,
                                       e = lst->end();
         b != e; ++b) {
        Node *node = (*b);
        node->addMacroPosition(n);
        DNode *new_dnode = node->toDNode();
        macro_args[macro_args_count++] = new_dnode;
    }

    PoolNode *pn =
        reinterpret_cast<PoolNode *>(malloc(sizeof(PoolNode)));
    if (!pn) {
        error("unable to allocate memory", true);
    }
    memset(pn, 0, sizeof(PoolNode));

    MContext mcontext;
    memset(&mcontext, 0, sizeof(MContext));

    mcontext.arg_count = macro_args_count;
    mcontext.pool_node = pn;
    mcontext.units = units;

    std::vector<llvm::GenericValue> values;
    llvm::GenericValue mc_val;
    mc_val.PointerVal = &mcontext;
    values.push_back(mc_val);
    int i = 0;
    for (; i < (mc->numberOfRequiredArgs() - 1); i++) {
        llvm::GenericValue dn_val;
        dn_val.PointerVal = macro_args[i];
        values.push_back(dn_val);
    }
    if (mc->isVarArgs()) {
        llvm::GenericValue rest_val;
        rest_val.PointerVal = &(macro_args[i]);
        values.push_back(rest_val);
    }

    std::vector<void *> values2;
    values2.push_back(&mcontext);
    i = 0;
    for (; i < (mc->numberOfRequiredArgs() - 1); i++) {
        values2.push_back(macro_args[i]);
    }
    if (mc->isVarArgs()) {
        values2.push_back(&(macro_args[i]));
    }

    if (units->debug) {
        functionDebugPass(mc->llvm_function);
    }

#if D_LLVM_VERSION_ORD >= 36
    uint64_t address = 0;
    std::map<std::string, uint64_t>::iterator ii =
        function_map.find(mc->symbol);
    if (ii != function_map.end()) {
        address = ii->second;
    } else {
        address = (uint64_t)
            llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(
                mc->symbol.c_str());

        if (!address) {
            Unit *unit = new Unit(units, ctx->er, ctx->nt, ctx->tr, NULL,
                                  true, ctx, units->top()->mp,
                                  units->top()->fp, NULL, NULL);
            units->push(unit);
            std::vector<Function *> functions;
            functions.push_back(mc);
            std::vector<Variable *> variables;
            linkRetrievedObjects(units->top()->module, n, &functions,
                                 &variables);

            cloneModuleIfRequired(units->top());

            units->top()->ee->getFunctionAddress(mc->symbol.c_str());
            llvm::Function *mc_ffn =
                units->top()->ee->FindFunctionNamed(mc->symbol.c_str());
            if (!mc_ffn) {
                fprintf(stderr, "cannot refetch: '%s'\n",
                        mc->symbol.c_str());
                abort();
            }
            address = units->top()->ee->getFunctionAddress(
                mc->symbol.c_str());

            llvm::Module *mod = units->top()->module;
            units->units.pop_back();
            ctx->regetPointers(units->top()->module);
            delete mod;
        }
    }
    function_map.insert(
        std::pair<std::string, uint64_t>(mc->symbol, address));

    DNode *result_dnode = callMacro(address, &values2);
    Node *result_node =
        (result_dnode) ? units->top()->dnc->toNode(result_dnode) : NULL;

    uint64_t pf_address = 0;
    Function *pfffn = ctx->getFunction("pool-free", NULL, NULL, 0);
    llvm::Function *pf_ffn =
        units->top()->ee->FindFunctionNamed(pfffn->symbol.c_str());
    if (pf_ffn) {
        pf_address =
            units->top()->ee->getFunctionAddress(pfffn->symbol.c_str());
    } else {
        pf_address = (uint64_t)
            llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(
                pfffn->symbol.c_str());
    }

    typedef void (*FFN)(void *);
    FFN free_function = (FFN)pf_address;
    free_function(&mcontext);
#else
    DNode *result_dnode = reinterpret_cast<DNode *>(
        ee->runFunction(mc->llvm_function, values).PointerVal);

    Node *result_node =
        (result_dnode) ? units->top()->dnc->toNode(result_dnode) : NULL;

    std::vector<llvm::GenericValue> values_pf;
    values_pf.push_back(mc_val);
    ee->runFunction(pool_free_fn, values_pf);
#endif

    if (result_node) {
        result_node->addMacroPosition(n);
    }

    return result_node;
}

Node *MacroProcessor::parseMacroCall(Node *n, Function *macro_to_call) {
    int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);
    Node *result = parseMacroCall_(n, macro_to_call);
    int diff =
        ctx->er->getErrorTypeCount(ErrorType::Error) - error_count;
    if (diff) {
        Error *e = new Error(MacroExpansionError, n);
        ctx->er->addError(e);
        return NULL;
    }
    if (result && units->print_expansions) {
        const char *filename = n->filename;
        if (!filename) {
            filename = "<unknown>";
        }
        printf("%s:%d:%d: expansion: ", filename,
               n->getBeginPos()->getLineNumber(),
               n->getBeginPos()->getColumnNumber());
        n->print();
        printf(" -> ");
        result->print();
        printf("\n");
        fflush(stdout);
    }
    return result;
}

Node *MacroProcessor::parsePotentialMacroCall(Node *n, bool once) {
    if (n->is_token || !n->is_list) {
        return n;
    }

    std::vector<Node *> *lst = n->list;
    if (lst->size() == 0) {
        return n;
    }

    Node *macro_name_node = (*lst)[0];
    if (!macro_name_node->is_token) {
        return n;
    }

    const char *macro_name = macro_name_node->token->str_value.c_str();

    Node *(*core_mac)(Context * ctx, Node * n);

    core_mac =
        (eq("setv"))
            ? &FormMacroSetvParse
            : (eq("@$"))
                  ? &FormMacroArrayDerefParse
                  : (eq(":@"))
                        ? &FormMacroDerefStructParse
                        : (eq("@:"))
                              ? &FormMacroStructDerefParse
                              : (eq("@:@"))
                                    ? &FormMacroDerefStructDerefParse
                                    : NULL;

    if (core_mac) {
        return core_mac(ctx, n);
    }

    Function *ffn = ctx->getFunction(macro_name, NULL, NULL, 1);
    if (!ffn) {
        return n;
    }

    bool made_temp = false;
    Function *global_fn = units->top()->getGlobalFunction();
    if (!global_fn) {
        units->top()->makeTemporaryGlobalFunction();
        global_fn = units->top()->getGlobalFunction();
        made_temp = true;
    }

    std::vector<Type *> types;
    llvm::BasicBlock *block = &(global_fn->llvm_function->front());

    int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);
    for (std::vector<Node *>::iterator b = lst->begin() + 1,
                                       e = lst->end();
         b != e; ++b) {
        ParseResult arg_pr;
        bool res = FormProcInstParse(units, global_fn, block, *b, false,
                                     false, NULL, &arg_pr);
        if (res) {
            /* Add the type. */
            types.push_back(arg_pr.type);
            block = arg_pr.block;
        } else {
            /* Add a (p DNode) to types. */
            types.push_back(ctx->tr->type_pdnode);
        }
    }
    ctx->er->popErrors(error_count);

    if (made_temp) {
        units->top()->removeTemporaryGlobalFunction();
    }

    ffn = ctx->getFunction(macro_name, &types, NULL, 1);
    if (!ffn) {
        return n;
    }

    Node *mac_node = parseMacroCall(n, ffn);
    if (!mac_node) {
        return NULL;
    }

    /* If a macro node was got, and it's a list containing two
     * elements, and the first element is 'do', then just return the
     * second element. */

    if ((!mac_node->is_token) && (mac_node->list->size() == 2) &&
        ((*mac_node->list)[0]->is_token) &&
        ((*mac_node->list)[0]->token->str_value.compare("do") == 0)) {
        if (once) {
            return (*mac_node->list)[1];
        } else {
            return parsePotentialMacroCall((*mac_node->list)[1]);
        }
    } else {
        if (once) {
            return mac_node;
        } else {
            return parsePotentialMacroCall(mac_node);
        }
    }
}
}
