#include "MacroProcessor.h"

#include "../Node/Node.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Form/Macro/ArrayDeref/ArrayDeref.h"
#include "../Form/Macro/StructDeref/StructDeref.h"
#include "../Form/Macro/DerefStructDeref/DerefStructDeref.h"
#include "../Form/Macro/DerefStruct/DerefStruct.h"
#include "../Form/Macro/Setv/Setv.h"
#include FFI_HEADER

#define eq(str) !strcmp(macro_name, str)

using namespace dale::ErrorInst;

namespace dale
{
llvm::Function *pool_free_fn;
void (*pool_free_fptr)(MContext *);

MacroProcessor::MacroProcessor(Units *units, Context *ctx,
                               llvm::ExecutionEngine* ee)
{
    this->units = units;
    this->ctx = ctx;
    this->ee = ee;
}

MacroProcessor::~MacroProcessor()
{
}

static DNode *
callmacro(int arg_count, void *units, void *mac, DNode **dnodes, MContext *mc)
{
    ffi_type *args[arg_count];
    void *vals[arg_count];

    args[0] = &ffi_type_pointer;
    vals[0] = (void*) &mc;

    for (int i = 1; i < arg_count; i++) {
        args[i] = &ffi_type_pointer;
        vals[i] = (void *) &(dnodes[i - 1]);
    }

    ffi_cif cif;
    ffi_status res = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_count,
                                  &ffi_type_pointer, args);
    _unused(res);
    assert((res == FFI_OK) && "prep_cif failed, cannot run macro");

    DNode *ret_node = NULL;
    ffi_call(&cif, (void (*)()) mac, (void *) &ret_node, vals);

    return ret_node;
}

void
MacroProcessor::setPoolfree()
{
    if (!pool_free_fptr) {
        pool_free_fn = ctx->getFunction("pool-free", NULL, 0)->llvm_function;
        pool_free_fptr =
            (void (*)(MContext *mcp))
                ee->getPointerToFunction(pool_free_fn);
    }
}

Node *
MacroProcessor::parseMacroCall_(Node *n, Function *macro_to_call)
{
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
    Function *mc =
        macro_to_call
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
            Error *e = new Error(IncorrectMinimumNumberOfArgs,
                                 n, macro_name,
                                 (mc->numberOfRequiredArgs() - 1),
                                 size);
            ctx->er->addError(e);
            return NULL;
        }
    } else {
        if (size != mc->numberOfRequiredArgs()) {
            Error *e = new Error(IncorrectNumberOfArgs,
                                 n, macro_name,
                                 (mc->numberOfRequiredArgs() - 1),
                                 size);
            ctx->er->addError(e);
            return NULL;
        }
    }

    DNode *macro_args[256];
    int macro_args_count = 0;

    for (std::vector<Node *>::iterator b = lst->begin() + 1,
                                       e = lst->end();
            b != e;
            ++b) {
        Node *node = (*b);
        node->addMacroPosition(n);
        DNode *new_dnode = node->toDNode();
        macro_args[macro_args_count++] = new_dnode;
    }

    PoolNode *pn = (PoolNode *) malloc(sizeof(PoolNode));
    if (!pn) {
        error("unable to allocate memory", true);
    }
    memset(pn, 0, sizeof(PoolNode));

    MContext mcontext;
    memset(&mcontext, 0, sizeof(MContext));

    mcontext.arg_count = macro_args_count;
    mcontext.pool_node = pn;
    mcontext.units     = units;

    void *callmacro_fptr = (void*) &callmacro;
    void *macro_fptr     = ee->getPointerToFunction(mc->llvm_function);

    DNode* (*callmacro_fptr_typed)
        (int arg_count, void *units, void *mac_fn, DNode **dnodes,
         MContext *mcp) =
            (DNode* (*)(int, void*, void*, DNode**, MContext*)) callmacro_fptr;

    DNode *result_dnode =
        callmacro_fptr_typed(macro_args_count + 1, (void *) units,
                             (char *) macro_fptr, macro_args, &mcontext);

    Node *result_node =
        (result_dnode) ? units->top()->dnc->toNode(result_dnode) : NULL;

    pool_free_fptr(&mcontext);

    if (result_node) {
        result_node->addMacroPosition(n);
    }

    return result_node;
}

Node *
MacroProcessor::parseMacroCall(Node *n, Function *macro_to_call)
{
    Node *result = parseMacroCall_(n, macro_to_call);
    if (result && units->print_expansions) {
        const char *filename = n->filename;
        if (!filename) {
            filename = "<unknown>";
        }
        printf("%s:%d:%d: expansion: ",
               filename,
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

Node *
MacroProcessor::parsePotentialMacroCall(Node *n)
{
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

    Node *(*core_mac)(Context *ctx, Node *n);

    core_mac =   (eq("setv")) ? &FormMacroSetvParse
               : (eq("@$"))   ? &FormMacroArrayDerefParse
               : (eq(":@"))   ? &FormMacroDerefStructParse
               : (eq("@:"))   ? &FormMacroStructDerefParse
               : (eq("@:@"))  ? &FormMacroDerefStructDerefParse
               : NULL;

    if (core_mac) {
        return core_mac(ctx, n);
    }

    Function *ffn = ctx->getFunction(macro_name, NULL, 1);
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
            b != e;
            ++b) {
        ParseResult arg_pr;
        bool res =
            FormProcInstParse(units, global_fn, block, *b, false, false, NULL,
                              &arg_pr);
        if (res) {
            /* Add the type. */
            types.push_back(arg_pr.type);
            block = arg_pr.block;
        }
        else {
            /* Add a (p DNode) to types. */
            types.push_back(ctx->tr->type_pdnode);
        }
    }
    ctx->er->popErrors(error_count);

    if (made_temp) {
        units->top()->removeTemporaryGlobalFunction();
    }

    ffn = ctx->getFunction(macro_name, &types, 1);
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

    if ((!mac_node->is_token)
            && (mac_node->list->size() == 2)
            && ((*mac_node->list)[0]->is_token)
            && ((*mac_node->list)[0]
                ->token->str_value.compare("do") == 0)) {
        return parsePotentialMacroCall((*mac_node->list)[1]);
    } else {
        return parsePotentialMacroCall(mac_node);
    }
}
}
