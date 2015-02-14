#include "MacroProcessor.h"

#include "../Node/Node.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Form/Macro/ArrayDeref/ArrayDeref.h"
#include "../Form/Macro/StructDeref/StructDeref.h"
#include "../Form/Macro/DerefStructDeref/DerefStructDeref.h"
#include "../Form/Macro/DerefStruct/DerefStruct.h"
#include "../Form/Macro/Setv/Setv.h"

#define eq(str) !strcmp(t->str_value.c_str(), str)

#include FFI_HEADER

static int myn = 0;

namespace dale
{
typedef std::vector<Node*> symlist;

llvm::Function *pool_free_fn;
void (*pool_free_fptr)(MContext *);

MacroProcessor::MacroProcessor(Generator *gen, Context *ctx, llvm::ExecutionEngine* ee)
{
    this->gen = gen;
    this->ctx = ctx;
    this->ee = ee;
}

MacroProcessor::~MacroProcessor()
{
}

static DNode *
callmacro(int arg_count, void *gen, void *mac, DNode **dnodes,
          MContext **mc_ptr)
{
    ffi_type **args =
        (ffi_type **) malloc(arg_count * sizeof(ffi_type *));
    void **vals =
        (void **)     malloc(arg_count * sizeof(void *));
    PoolNode *pn =
        (PoolNode *)  malloc(sizeof(PoolNode));
    MContext *mc =
        (MContext *)  malloc(sizeof(MContext));
    *mc_ptr = mc;

    memset(pn, 0, sizeof(PoolNode));
    memset(mc, 0, sizeof(MContext));
    args[0] = &ffi_type_pointer;
    vals[0] = (void*) &mc;

    int actual_arg_count = arg_count - 1;
    mc->arg_count = actual_arg_count;
    mc->pool_node = pn;
    mc->generator = gen;

    int i;
    for (i = 1; i < arg_count; i++) {
        args[i] = &ffi_type_pointer;
    }
    for (i = 1; i < arg_count; i++) {
        vals[i] = (void *) &(dnodes[i - 1]);
    }

    ffi_cif cif;
    ffi_status res2 =
        ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_count,
                     &ffi_type_pointer, args);

    if (res2 != FFI_OK) {
        fprintf(stderr, "prep_cif failed, cannot run macro.\n");
        abort();
    }

    DNode *ret_node = NULL;
    //fprintf(stderr, "Ret node 1: %p\n", ret_node);
    ffi_call(&cif, (void (*)(void)) mac, (void *) &ret_node, vals);
    //fprintf(stderr, "Ret node 2: %p\n", ret_node);
    free(args);
    free(vals);

    return ret_node;
}

void
MacroProcessor::setPoolfree(void)
{
    if (!pool_free_fptr) {
        pool_free_fn = ctx->getFunction("pool-free", NULL, 0)->llvm_function;
        pool_free_fptr =
            (void (*)(MContext *mcp))
                ee->getPointerToFunction(pool_free_fn);
    }
}

Node *MacroProcessor::parseMacroCall(Node *n,
                                const char *name,
                                Function *macro_to_call)
{
    assert(n->list && "must receive a list!");

    symlist *lst = n->list;

    Node *nmc_name = (*lst)[0];

    if (!nmc_name->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        ctx->er->addError(e);
        return NULL;
    }

    Token *t = nmc_name->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            n
        );
        ctx->er->addError(e);
        return NULL;
    }

    /* Have to expand this to handle overloading itself
     * (macro_to_call is provided by PFBI, where applicable). */

    Function *mc =
        macro_to_call
        ? macro_to_call
        : ctx->getFunction(t->str_value.c_str(), NULL, NULL, 1);

    if (!mc) {
        Error *e = new Error(
            ErrorInst::Generator::MacroNotInScope,
            n,
            t->str_value.c_str()
        );
        ctx->er->addError(e);
        return NULL;
    }

    /* Used to be -1 for lst->size() here, but isn't anymore,
     * because of the implicit MContext variable that is passed
     * to each macro. */

    int size = lst->size();

    if (mc->isVarArgs()) {
        if (size < mc->numberOfRequiredArgs()) {
            char buf1[100];
            char buf2[100];
            sprintf(buf1, "%d", size);
            sprintf(buf2, "%d", (mc->numberOfRequiredArgs() - 1));
            Error *e = new Error(
                ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
                n,
                t->str_value.c_str(), buf2, buf1
            );
            ctx->er->addError(e);
            return NULL;
        }

    } else {
        if (size != mc->numberOfRequiredArgs()) {
            char buf1[100];
            char buf2[100];
            sprintf(buf1, "%d", size);
            sprintf(buf2, "%d", (mc->numberOfRequiredArgs() - 1));
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                n,
                t->str_value.c_str(), buf2, buf1
            );
            ctx->er->addError(e);
            return NULL;
        }
    }

    std::vector<Variable *>::iterator var_iter;
    var_iter = mc->parameter_types.begin();
    // Skip implicit MContext arg. */
    ++var_iter;

    std::vector<DNode *> dnodes_to_free;

    DNode *myargs[256];
    int myargs_count = 0;

    std::vector<Node *>::iterator node_iter;
    node_iter = lst->begin();
    ++node_iter;

    while (node_iter != lst->end()) {
        Node *ni = (*node_iter);
        ni->addMacroPosition(n);

        if ((*var_iter)->type->base_type == BaseType::VarArgs) {
            /* Into varargs - always pointers to DNodes. */
            DNode *new_dnode = (*node_iter)->toDNode();
            myargs[myargs_count++] = new_dnode;
            dnodes_to_free.push_back(new_dnode);
            ++node_iter;
            continue;
        }

        /* todo: Macros can have various parameter types, but the actual
         * arguments are always pointers to DNode. */

        DNode *new_dnode = (*node_iter)->toDNode();
        myargs[myargs_count++] = new_dnode;
        dnodes_to_free.push_back(new_dnode);
        ++var_iter;
        ++node_iter;
    }

    void *callmacro_fptr = (void*) &callmacro;

    void *actualmacro_fptr =
        ee->getPointerToFunction(mc->llvm_function);

    /* Cast it to the correct type. */

    DNode* (*FP)(int arg_count, void *gen, void *mac_fn, DNode
    **dnodes, MContext **mcp) =
        (DNode* (*)(int, void*, void*, DNode**, MContext**))callmacro_fptr;

    /* Get the returned dnode. */

    MContext *mcontext;
    DNode *mc_result_dnode = FP(myargs_count + 1,
                                (void *) gen,
                                (char *) actualmacro_fptr,
                                myargs,
                                &mcontext);

    /* Convert it to an int node. */

    //fprintf(stderr, "MC result dnode: %s: %p\n", name, mc_result_dnode);
    Node *mc_result_node =
        (mc_result_dnode) ? gen->units->top()->dnc->toNode(mc_result_dnode)
                          : NULL;

    /* Free the pool node. */

    pool_free_fptr(mcontext);
    free(mcontext);

    /* Add the macro position information to the nodes. */

    if (mc_result_node) {
        mc_result_node->addMacroPosition(n);
    }

    /* Finished - return the macro result node. */

    return mc_result_node;
}

Node *
MacroProcessor::parseOptionalMacroCall(Node *n)
{
    if (n->is_token) {
        return n;
    }

    if (!n->is_list) {
        return n;
    }

    symlist *lst = n->list;
    if (lst->size() == 0) {
        return n;
    }

    Node *mac_name = (*lst)[0];

    if (!mac_name->is_token) {
        return n;
    }

    Token *t = mac_name->token;

    /* Core macros. */

    Node* (*core_mac)(Context *ctx, Node *n);

    core_mac =   (eq("setv"))   ? &FormMacroSetvParse
               : (eq("@$"))     ? &FormMacroArrayDerefParse
               : (eq(":@"))     ? &FormMacroDerefStructParse
               : (eq("@:"))     ? &FormMacroStructDerefParse
               : (eq("@:@"))    ? &FormMacroDerefStructDerefParse
               : NULL;

    if (core_mac) {
        Node *new_node = core_mac(ctx, n);
        if (!new_node) {
            return NULL;
        }

        return new_node;
    }

    Function *ffn =
        ctx->getFunction(t->str_value.c_str(), NULL, 1);
    if (!ffn) {
        return n;
    }

    /* Create a temporary function for evaluating the arguments. */

    llvm::Type *llvm_return_type =
        ctx->toLLVMType(ctx->tr->type_int, NULL, false);
    if (!llvm_return_type) {
        return NULL;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[32];
    sprintf(buf, "_gen%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (gen->units->top()->module->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        gen->units->top()->module->getOrInsertFunction(new_name.c_str(), ft);
    if (!fnc) {
        fprintf(stderr, "Internal error: unable to add "
                "function ('%s') to module.\n",
                new_name.c_str());
        abort();
    }

    llvm::Function *fn =
        llvm::dyn_cast<llvm::Function>(fnc);
    if (!fn) {
        fprintf(stderr, "Internal error: unable to convert "
                "function constant to function "
                "for function '%s'.\n",
                new_name.c_str());
        abort();
    }

    std::vector<Variable *> vars;

    Function *dfn =
        new Function(ctx->tr->type_int,
                              &vars,
                              fn,
                              0,
                              new std::string(new_name),
                              0);
    dfn->linkage = Linkage::Intern;
    if (!dfn) {
        fprintf(stderr, "Internal error: unable to create new "
                "function (!) '%s'.\n",
                new_name.c_str());
        abort();
    }

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "entry",
                                 fn);

    /* Iterate over the arguments and collect the types. Make
     * backups of the existing state first. */

    std::vector<Type *> types;

    int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);

    gen->units->top()->pushGlobalFunction(dfn);
    gen->units->top()->pushGlobalBlock(block);

    ctx->activateAnonymousNamespace();

    for (std::vector<Node *>::iterator b = lst->begin() + 1,
            e = lst->end();
            b != e;
            ++b) {
        ParseResult mine;
        bool res =
            FormProcInstParse(gen, dfn, block, *b, false, false, NULL,
                              &mine);

        if (res) {
            /* Add the type. */
            types.push_back(mine.type);
            block = mine.block;
        }
        else {
            /* Add a (p DNode) to types. */
            types.push_back(ctx->tr->type_pdnode);
        }
    }
    ctx->er->popErrors(error_count);

    ctx->deactivateAnonymousNamespace();

    gen->units->top()->popGlobalFunction();
    gen->units->top()->popGlobalBlock();

    /* Remove the temporary function. */
    fn->eraseFromParent();

    /* Call getFunction with the new set of parameter types. */

    ffn = ctx->getFunction(t->str_value.c_str(), &types, 1);
    if (!ffn) {
        // No macro matching these type parameters.
        return n;
    }

    Node *mac_node = parseMacroCall(n, t->str_value.c_str(),
                                    ffn);

    if (!mac_node) {
        return NULL;
    }

    /* If a macro node was got, and it's a list containing two
     * elements, and the first element is 'do', then just return
     * the second element. */

    if ((!mac_node->is_token)
            && (mac_node->list->size() == 2)
            && (mac_node->list->at(0)->is_token)
            && (mac_node->list->at(0)
                ->token->str_value.compare("do") == 0)) {
        return parseOptionalMacroCall(mac_node->list->at(1));
    } else {
        return parseOptionalMacroCall(mac_node);
    }
}
}
