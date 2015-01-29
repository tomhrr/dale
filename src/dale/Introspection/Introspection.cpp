#include "Introspection.h"
#include "../Type/Type.h"
#include "../Generator/Generator.h"
#include "../Form/Type/Type.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Utils/Utils.h"

using namespace dale;

static int myn = 0;
bool makeTemporaryGlobalFunction(
    Generator *gen,
    std::vector<DeferredGoto*> *dgs,
    std::map<std::string, Label*> *mls
)
{
    Context *ctx = gen->ctx;

    /* Create a temporary function for evaluating the arguments. */

    llvm::Type *llvm_return_type =
        ctx->toLLVMType(ctx->tr->type_int, NULL, false);
    if (!llvm_return_type) {
        return false;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[255];
    sprintf(buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (gen->mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        gen->mod->getOrInsertFunction(new_name.c_str(), ft);
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

    int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);

    gen->global_functions.push_back(dfn);
    gen->global_function = dfn;

    gen->global_blocks.push_back(block);
    gen->global_block = block;

    ctx->activateAnonymousNamespace();

    return error_count;
}

void removeTemporaryGlobalFunction(
    Generator *gen,
    int error_count,
    std::vector<DeferredGoto*> *dgs,
    std::map<std::string, Label*> *mls
)
{
    Context *ctx = gen->ctx;

    if (error_count >= 0) {
        ctx->er->popErrors(error_count);
    }

    ctx->deactivateAnonymousNamespace();
    Function *current = gen->global_function;

    gen->global_functions.pop_back();
    if (gen->global_functions.size()) {
        gen->global_function = gen->global_functions.back();
    } else {
        gen->global_function = NULL;
    }

    gen->global_blocks.pop_back();
    if (gen->global_blocks.size()) {
        gen->global_block = gen->global_blocks.back();
    } else {
        gen->global_block = NULL;
    }

    /* Remove the temporary function. */
    current->llvm_function->eraseFromParent();

    return;
}

extern "C" {
    Node *WrapNode(Node *n)
    {
        std::vector<Node *> *nodes = new std::vector<Node *>;
        nodes->push_back(n);
        Node *holder = new Node(nodes);
        return holder;
    }

    bool types_2D_equal(MContext *mc, DNode *t1, DNode *t2)
    {
        Generator *g = (dale::Generator*) mc->generator;
        Node *n = g->DNodeToIntNode(t1);
        n = g->parseOptionalMacroCall(n);
        if (!n) {
            return false;
        }

        Node *n2 = g->DNodeToIntNode(t2);
        n2 = g->parseOptionalMacroCall(n2);
        if (!n2) {
            return false;
        }

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        Type *thetype  = FormTypeParse(g, n,  false, false);
        Type *thetype2 = FormTypeParse(g, n2, false, false);

        g->ctx->er->popErrors(original_error_count);
        if (!thetype || !thetype2) {
            return 0;
        }
        return (thetype->isEqualTo(thetype2));
    }

    bool is_2D_char_2D_type(MContext *mc, DNode *t)
    {
        Generator *g = (dale::Generator*) mc->generator;
        Node *n = g->DNodeToIntNode(t);
        n = g->parseOptionalMacroCall(n);
        if (!n) {
            return false;
        }
        if (!n->token) {
            return false;
        }
        if (n->token->str_value.compare("char")) {
            return false;
        }
        return true;
    }

    static bool get_type(MContext *mc, DNode *dnode, Type **type)
    {
        Generator *g = (dale::Generator*) mc->generator;
        Node *n = g->DNodeToIntNode(dnode);
        n = g->parseOptionalMacroCall(n);
        if (!n) {
            return false;
        }

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        *type = FormTypeParse(g, n, false, false);

        g->ctx->er->popErrors(original_error_count);
        if (!*type) {
            return false;
        }

        return true;
    }

    bool is_2D_integer_2D_type(MContext *mc, DNode *t)
    {
        Type *type;
        bool res = get_type(mc, t, &type);
        if (!res) {
            return false;
        }
        return type->isIntegerType();
    }

    bool is_2D_signed_2D_integer_2D_type(MContext *mc, DNode *t)
    {
        Type *type;
        bool res = get_type(mc, t, &type);
        if (!res) {
            return false;
        }
        return type->isSignedIntegerType();
    }

    bool is_2D_unsigned_2D_integer_2D_type(MContext *mc, DNode *t)
    {
        Type *type;
        bool res = get_type(mc, t, &type);
        if (!res) {
            return false;
        }
        return (type->isIntegerType() && !(type->isSignedIntegerType()));
    }

    bool is_2D_floating_2D_point_2D_type(MContext *mc, DNode *t)
    {
        Type *type;
        bool res = get_type(mc, t, &type);
        if (!res) {
            return false;
        }
        return type->isFloatingPointType();
    }

    bool is_2D_pointer_2D_type(MContext *mc, DNode *t)
    {
        Type *type;
        bool res = get_type(mc, t, &type);
        if (!res) {
            return false;
        }
        return (type->points_to ? true : false);
    }

    bool is_2D_pointer_2D_to_2D_type(MContext *mc, DNode *t,
                                     DNode *pointee)
    {
        Generator *g = (dale::Generator*) mc->generator;
        Node *n = g->DNodeToIntNode(t);
        n = g->parseOptionalMacroCall(n);
        if (!n) {
            return false;
        }

        Node *n2 = g->DNodeToIntNode(pointee);
        n2 = g->parseOptionalMacroCall(n2);
        if (!n2) {
            return false;
        }

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        Type *thetype  = FormTypeParse(g, n,  false, false);
        Type *thetype2 = FormTypeParse(g, n2, false, false);

        g->ctx->er->popErrors(original_error_count);
        if (!thetype || !thetype2) {
            return false;
        }
        return (thetype->points_to->isEqualTo(thetype2));
    }

    DNode *pointee_2D_type(MContext *mc, DNode *t)
    {
        Type *type;
        bool res = get_type(mc, t, &type);
        if (!res) {
            return NULL;
        }
        if (!type->points_to) {
            return NULL;
        }
        return type->points_to->toNode()->toDNode();
    }

    bool must_2D_init(MContext *mc, DNode *t)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        Node *n = g->DNodeToIntNode(t);

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        Type *ptype = FormTypeParse(g, n, false, false);
        if (!ptype) {
            g->ctx->er->popErrors(original_error_count);
            return false;
        }

        if (ptype->is_const) {
            return true;
        }
        if (ptype->struct_name.size()) {
            Struct *structp =
                g->ctx->getStruct(
                    ptype->struct_name.c_str(),
                    &(ptype->namespaces)
                );
            return !!(structp->must_init);
        }
        return false;
    }

    bool is_2D_const(MContext *mc, DNode *t)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(t);

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        Type *ptype = FormTypeParse(g, n, false, false);
        if (!ptype) {
            g->ctx->er->popErrors(original_error_count);
            return false;
        }

        if (ptype->is_const) {
            return true;
        }

        return false;
    }

    bool has_2D_errors(MContext *mc, DNode *form)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        assert(n->list && "must receive a list!");

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        int made_temp = 0;
        std::vector<DeferredGoto*> dgs;
        std::map<std::string, Label*> mls;
        if (!g->global_function) {
            makeTemporaryGlobalFunction(g, &dgs, &mls);
            made_temp = 1;
        }

        ParseResult temp;

        /* POMC may succeed, but the underlying macro may return a
         * null DNode pointer. This is not necessarily an error. */
        n = g->parseOptionalMacroCall(n);
        if (n) {
            FormProcInstParse(
                g,
                g->global_function,
                g->global_block,
                n,
                false,
                false,
                NULL,
                &temp
            );
        }

        if (made_temp) {
            removeTemporaryGlobalFunction(g, -1, &dgs, &mls);
        }

        int new_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        bool has_errors =
            ((new_error_count - original_error_count) != 0);

        g->ctx->er->popErrors(original_error_count);
        return has_errors;
    }

    bool exists_2D_fn(MContext *mc, DNode *form)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        assert(n->list && "must receive a list!");

        if (!g->ctx->er->assertArgNums("exists-fn", n, 1, -1)) {
            return false;
        }

        symlist *lst = n->list;

        Node *nret_type = (*lst)[0];
        Node *nfn_name  = (*lst)[1];

        std::vector<Node *>::iterator iter = lst->begin();
        ++iter;
        ++iter;

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        Type *ret_type = FormTypeParse(g, nret_type, false,
                                               false);
        if (!ret_type) {
            g->ctx->er->popErrors(original_error_count);
            return false;
        }

        int c = 0;

        std::vector<Type *> parameter_types;
        while (iter != lst->end()) {
            Type *ptype = FormTypeParse(g, (*iter), false,
                                                false);
            if (!ptype) {
                g->ctx->er->popErrors(original_error_count);
                return false;
            }
            if (ptype->base_type == BaseType::Void) {
                break;
            }
            parameter_types.push_back(ptype);
            ++iter;
            ++c;
        }

        Function *thefn =
            g->ctx->getFunction(nfn_name->token->str_value.c_str(),
                                &parameter_types,
                                NULL,
                                0);

        g->ctx->er->popErrors(original_error_count);
        return (thefn && !thefn->is_macro);
    }

    bool exists_2D_type(MContext *mc, DNode *t)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = WrapNode(g->DNodeToIntNode(t));
        assert(n->list && "must receive a list!");

        if (!g->ctx->er->assertArgNums("exists-type", n, 0, 0)) {
            printf("wrong arg count! (existsType)\n");
            return false;
        }

        symlist *lst = n->list;

        int error_count = g->ctx->er->getErrorTypeCount(ErrorType::Error);

        Type *type = FormTypeParse(g, (*lst)[0], false,
                                           false);
        g->ctx->er->popErrors(error_count);

        return !!type;
    }

    /* Exact means exact, as in, this includes the implicit starting
     * arguments. You probably want existsMacro. */
    bool exists_2D_macro(MContext *mc, DNode *form)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        assert(n->list && "must receive a list!");

        if (!g->ctx->er->assertArgNums("exists-macro", n, 0, -1)) {
            return false;
        }

        symlist *lst = n->list;

        Node *nfn_name  = (*lst)[0];

        std::vector<Node *>::iterator iter = lst->begin();
        ++iter;

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        int c = 0;

        std::vector<Type *> parameter_types;
        while (iter != lst->end()) {
            Type *ptype = FormTypeParse(g, (*iter), false,
                                                false);
            if (!ptype) {
                g->ctx->er->popErrors(original_error_count);
                return false;
            }
            if (ptype->base_type == BaseType::Void) {
                break;
            }
            parameter_types.push_back(ptype);
            ++iter;
            ++c;
        }

        Function *thefn =
            g->ctx->getFunction(nfn_name->token->str_value.c_str(),
                                &parameter_types,
                                NULL,
                                1);

        g->ctx->er->popErrors(original_error_count);
        return (thefn && thefn->is_macro);
    }

    bool exists_2D_variable(MContext *mc, DNode *form)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);
        n = g->parseOptionalMacroCall(n);
        if (!n) {
            return false;
        }
        if (!n->token) {
            return false;
        }

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        Variable *thevar =
            g->ctx->getVariable(n->token->str_value.c_str());

        g->ctx->er->popErrors(original_error_count);
        return ((thevar) ? true : false);
    }

    int fn_2D_by_2D_args_2D_count(MContext *mc, DNode *form,
                                  const char *prefix)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        assert(n->list && "must receive a list!");

        if (!g->ctx->er->assertArgNums("fn-by-args-count", n, 0, -1)) {
            return 0;
        }

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        symlist *lst = n->list;

        std::vector<Node *>::iterator iter = lst->begin();
        int c = 0;

        std::string map_key;
        std::vector<Type *> parameter_types;
        while (iter != lst->end()) {
            Type *ptype = FormTypeParse(g, (*iter), false,
                                                false);
            if (!ptype) {
                g->ctx->er->popErrors(original_error_count);
                return 0;
            }
            ptype->toString(&map_key);
            parameter_types.push_back(ptype);
            ++iter;
            ++c;
        }

        std::map<std::string, std::vector<std::string>*>::iterator
            b = g->fn_by_args.find(map_key),
            e = g->fn_by_args.end();
        if (b != e) {
            std::vector<std::string>* fn_by_args_list = b->second;
            delete fn_by_args_list;
            g->fn_by_args.erase(b);
        }

        /* For each function that exists, see if it has an instance
         * for the provided parameter types. */

        std::set<std::string> function_names;
        bool has_prefix = prefix;
        if (!prefix) {
            prefix = "";
        }
        std::string ss_prefix(prefix);
        g->ctx->getFunctionNames(&function_names, 
                                 (has_prefix ? &ss_prefix : NULL));
        std::vector<std::string> *fn_by_args_list =
            new std::vector<std::string>;

        for (std::set<std::string>::iterator
                b = function_names.begin(),
                e = function_names.end();
                b != e;
                ++b) {
            Function *thefn =
                g->ctx->getFunction(b->c_str(),
                                    &parameter_types,
                                    NULL,
                                    0);
            if (thefn && (!(thefn->is_macro))) {
                fn_by_args_list->push_back((*b));
            }
        }

        g->fn_by_args.insert(
            std::pair<std::string, std::vector<std::string>*>(
                map_key,
                fn_by_args_list
            )
        );

        return (int) fn_by_args_list->size();
    }

    const char* fn_2D_by_2D_args_2D_name(MContext *mc, DNode *form,
                                         int index)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        assert(n->list && "must receive a list!");

        if (!g->ctx->er->assertArgNums("fn-by-args-name", n, 0, -1)) {
            return NULL;
        }

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        symlist *lst = n->list;

        std::vector<Node *>::iterator iter = lst->begin();

        std::string map_key;
        while (iter != lst->end()) {
            Type *ptype = FormTypeParse(g, (*iter), false,
                                                false);
            if (!ptype) {
                g->ctx->er->popErrors(original_error_count);
                return NULL;
            }
            ptype->toString(&map_key);
            ++iter;
        }

        std::map<std::string, std::vector<std::string>*>::iterator
            b = g->fn_by_args.find(map_key),
            e = g->fn_by_args.end();

        if (b == e) {
            return NULL;
        }

        std::vector<std::string> *fn_by_args_list = b->second;
        if (((unsigned) index) > (fn_by_args_list->size() - 1)) {
            return NULL;
        }

        return fn_by_args_list->at(index).c_str();
    }

    bool register_2D_type(MContext *mc, const char *from, const char *to)
    {
        dale::addTypeMapEntry(from, to);
        return true;
    }

    DNode *type_2D_of(MContext *mc, DNode *form)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        if (!g->global_function) {
            fprintf(stderr, "No global function set.\n");
            abort();
        }
        if (!g->global_block) {
            fprintf(stderr, "No global block set.\n");
            abort();
        }

        ParseResult *p = new ParseResult();
        bool res =
            FormProcInstParse(
                g,
                g->global_function,
                g->global_block,
                n,
                false,
                false, 
                NULL,
                p
            );

        if (!res) {
            return NULL;
        }

        DNode *mynode = p->type->toNode()->toDNode();
        return mynode;
    }

    const char *printf_2D_length(MContext *mc, DNode *t)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(t);
        if (!n->is_token) {
            return "";
        }
        std::string *str = &(n->token->str_value);

        if (!(str->compare("char"))) {
            return "hh";
        }
        if (!(str->compare("size"))) {
            return "z";
        }
        if (!(str->compare("ptrdiff"))) {
            return "t";
        }
        if (!(str->compare("long-double"))) {
            return "L";
        }
        /* The type token now should be an int or uint, followed by a
         * number. Confirm that this is so, get the number, divide it
         * by four and return the appropriate length character. */
        if (!(str->find("int"))) {
            str->erase(str->begin(), str->begin() + 3);
        } else if (!(str->find("uint"))) {
            str->erase(str->begin(), str->begin() + 4);
        } else {
            return "";
        }

        unsigned long addnum =
            strtoul(str->c_str(), NULL, DECIMAL_RADIX);

        if (STRTOUL_FAILED(addnum, str->c_str(), NULL)) {
            return "";
        }
        addnum /= 8;

        if (addnum == sizeof(short)) {
            return "h";
        }
        if (addnum == sizeof(long)) {
            return "l";
        }
        if (addnum >= sizeof(long long)) {
            return "ll";
        }
        return "";
    }

    bool type_2D_to_2D_string(MContext *mc, DNode *t, char *buf)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = WrapNode(g->DNodeToIntNode(t));

        Type *type = FormTypeParse(g, (*(n->list))[0], false,
                                           false);
        if (!type) {
            return false;
        }

        std::string temp;
        type->toSymbolString(&temp);
        if (temp.size() > 255) {
            fprintf(stderr, "Internal error: encoded type "
                    "string is too long (>255 "
                    "characters): %s.\n",
                    temp.c_str());
        }
        std::copy(temp.begin(), temp.end(), buf);
        buf[temp.size()] = '\0';

        return true;
    }

    bool type_2D_to_2D_display_2D_string(MContext *mc, DNode *t, char *buf)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = WrapNode(g->DNodeToIntNode(t));

        Type *type = FormTypeParse(g, (*(n->list))[0], false,
                                           false);
        if (!type) {
            return false;
        }

        std::string temp;
        type->toString(&temp);
        std::copy(temp.begin(), temp.end(), buf);
        buf[temp.size()] = '\0';

        return true;
    }

    void report_2D_error(MContext *mc, DNode *form, char *str)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        Error *e = new Error(
            ErrorInst::Generator::ExternalError, n, str
        );
        g->ctx->er->addError(e);
        return;
    }

    int arity(MContext *mc, DNode *name)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *fn_node = g->DNodeToIntNode(name);
        fn_node = g->parseOptionalMacroCall(fn_node);
        if (!fn_node) {
            return -1;
        }
        const char *fn_name = fn_node->token->str_value.c_str();

        Function *fn = g->ctx->getFunction(fn_name, NULL, NULL,
                                                    0);
        if (!fn) {
            return -1;
        }
        if (fn->linkage != Linkage::Extern_C) {
            return -1;
        }
        return fn->numberOfRequiredArgs();
    }

    DNode *codomain(MContext *mc, DNode *form)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *n = g->DNodeToIntNode(form);

        assert(n->list && "must receive a list!");

        if (!g->ctx->er->assertArgNums("codomain", n, 0, -1)) {
            return NULL;
        }

        symlist *lst = n->list;

        Node *nfn_name  = (*lst)[0];

        std::vector<Node *>::iterator iter = lst->begin();
        ++iter;

        int original_error_count =
            g->ctx->er->getErrorTypeCount(ErrorType::Error);

        int c = 0;

        std::vector<Type *> parameter_types;
        while (iter != lst->end()) {
            Type *ptype = FormTypeParse(g, (*iter), false,
                                                false);
            if (!ptype) {
                g->ctx->er->popErrors(original_error_count);
                return NULL;
            }
            if (ptype->base_type == BaseType::Void) {
                break;
            }
            parameter_types.push_back(ptype);
            ++iter;
            ++c;
        }

        Function *thefn =
            g->ctx->getFunction(nfn_name->token->str_value.c_str(),
                                &parameter_types,
                                NULL,
                                0);

        g->ctx->er->popErrors(original_error_count);
        if (thefn && !thefn->is_macro) {
            DNode *dn =
                thefn->return_type->toNode()->toDNode();
            return dn;
        }
        else {
            return NULL;
        } 
    }

    DNode *input_2D_type(MContext *mc, DNode *fn_name_nd, int index)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *fn_node = g->DNodeToIntNode(fn_name_nd);
        fn_node = g->parseOptionalMacroCall(fn_node);
        if (!fn_node) {
            return NULL;
        }
        const char *fn_name = fn_node->token->str_value.c_str();

        Function *fn = g->ctx->getFunction(fn_name, NULL,
                                                    NULL, 0);
        if (!fn) {
            return NULL;
        }
        if (fn->linkage != Linkage::Extern_C) {
            return NULL;
        }
        if ((int) (fn->parameter_types.size() - 1) < index) {
            return NULL;
        }

        return fn->parameter_types.at(index)->type->toNode()->toDNode();
    }

    DNode *struct_2D_member_2D_type(MContext *mc,
                                    DNode *name, int index)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *s_node = g->DNodeToIntNode(name);
        s_node = g->parseOptionalMacroCall(s_node);
        if (!s_node || !s_node->token) {
            return NULL;
        }
        const char *struct_name = s_node->token->str_value.c_str();

        Struct *st = g->ctx->getStruct(struct_name);
        if (!st) {
            return NULL;
        }
        if ((int) st->member_types.size() < index) {
            return NULL;
        }
        return
            st->member_types.at(index)->toNode()->toDNode();
    }

    const char *struct_2D_member_2D_name(MContext *mc,
                                         DNode *name,
                                         int index)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *s_node = g->DNodeToIntNode(name);
        s_node = g->parseOptionalMacroCall(s_node);
        if (!s_node || !s_node->token) {
            return NULL;
        }
        const char *struct_name = s_node->token->str_value.c_str();

        Struct *st = g->ctx->getStruct(struct_name);
        if (!st) {
            return NULL;
        }
        if ((int) st->member_types.size() < index) {
            return NULL;
        }
        return st->indexToMember(index);
    }

    int struct_2D_member_2D_count(MContext *mc, DNode *name)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;

        Node *s_node = g->DNodeToIntNode(name);
        s_node = g->parseOptionalMacroCall(s_node);
        if (!s_node || !s_node->token) {
            return -1;
        }
        const char *struct_name = s_node->token->str_value.c_str();

        Struct *st = g->ctx->getStruct(struct_name);
        if (!st) {
            return -1;
        }
        return st->member_types.size();
    }

    void *find_introspection_function(const char *name)
    {
        if (!strcmp(name, "exists-fn")) {
            return (void *) exists_2D_fn;
        } else if (!strcmp(name, "exists-type")) {
            return (void *) exists_2D_type;
        } else if (!strcmp(name, "type-of")) {
            return (void *) type_2D_of;
        } else if (!strcmp(name, "printf-length")) {
            return (void *) printf_2D_length;
        } else if (!strcmp(name, "exists-variable")) {
            return (void *) exists_2D_variable;
        } else if (!strcmp(name, "exists-macro")) {
            return (void *) exists_2D_macro;
        } else if (!strcmp(name, "report-error")) {
            return (void *) report_2D_error;
        } else if (!strcmp(name, "type-to-string")) {
            return (void *) type_2D_to_2D_string;
        } else if (!strcmp(name, "type-to-display-string")) {
            return (void *) type_2D_to_2D_display_2D_string;
        } else if (!strcmp(name, "register-type")) {
            return (void *) register_2D_type;
        } else if (!strcmp(name, "struct-member-count")) {
            return (void *) struct_2D_member_2D_count;
        } else if (!strcmp(name, "struct-member-type")) {
            return (void *) struct_2D_member_2D_type;
        } else if (!strcmp(name, "struct-member-name")) {
            return (void *) struct_2D_member_2D_name;
        } else if (!strcmp(name, "input-type")) {
            return (void *) input_2D_type;
        } else if (!strcmp(name, "is-char-type")) {
            return (void *) is_2D_char_2D_type;
        } else if (!strcmp(name, "is-integer-type")) {
            return (void *) is_2D_integer_2D_type;
        } else if (!strcmp(name, "is-signed-integer-type")) {
            return (void *) is_2D_signed_2D_integer_2D_type;
        } else if (!strcmp(name, "is-unsigned-integer-type")) {
            return (void *) is_2D_unsigned_2D_integer_2D_type;
        } else if (!strcmp(name, "is-floating-point-type")) {
            return (void *) is_2D_floating_2D_point_2D_type;
        } else if (!strcmp(name, "is-pointer-type")) {
            return (void *) is_2D_pointer_2D_type;
        } else if (!strcmp(name, "is-pointer-to-type")) {
            return (void *) is_2D_pointer_2D_to_2D_type;
        } else if (!strcmp(name, "pointee-type")) {
            return (void *) pointee_2D_type;
        } else if (!strcmp(name, "fn-by-args-count")) {
            return (void *) fn_2D_by_2D_args_2D_count;
        } else if (!strcmp(name, "fn-by-args-name")) {
            return (void *) fn_2D_by_2D_args_2D_name;
        } else if (!strcmp(name, "has-errors")) {
            return (void *) has_2D_errors;
        } else if (!strcmp(name, "is-const")) {
            return (void *) is_2D_const;
        } else if (!strcmp(name, "must-init")) {
            return (void *) must_2D_init;
        }
        return NULL;
    }
}
