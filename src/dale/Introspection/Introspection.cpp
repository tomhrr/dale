#include "Introspection.h"
#include "../Type/Type.h"
#include "../Generator/Generator.h"
#include "../Form/Type/Type.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Utils/Utils.h"

using namespace dale;

static int function_count = 0;

bool
makeTemporaryGlobalFunction(Generator *gen,
                            std::vector<DeferredGoto*> *dgs,
                            std::map<std::string, Label*> *mls)
{
    Context *ctx = gen->ctx;

    llvm::Type *llvm_return_type =
        ctx->toLLVMType(ctx->tr->type_int, NULL, false);
    if (!llvm_return_type) {
        return false;
    }

    std::vector<llvm::Type*> empty_args;
    llvm::FunctionType *ft =
        getFunctionType(llvm_return_type, empty_args, false);

    char buf[32];
    sprintf(buf, "_intro%d", function_count++);

    std::string new_name;
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (gen->mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *llvm_fnc =
        gen->mod->getOrInsertFunction(new_name.c_str(), ft);
    if (!llvm_fnc) {
        fprintf(stderr, "Internal error: unable to add "
                "function ('%s') to module.\n",
                new_name.c_str());
        abort();
    }

    llvm::Function *llvm_fn = llvm::dyn_cast<llvm::Function>(llvm_fnc);
    if (!llvm_fn) {
        fprintf(stderr, "Internal error: unable to convert "
                "function constant to function "
                "for function '%s'.\n",
                new_name.c_str());
        abort();
    }

    std::vector<Variable *> vars;
    Function *fn =
        new Function(ctx->tr->type_int, &vars, llvm_fn, 0,
                     new std::string(new_name), 0);
    fn->linkage = Linkage::Intern;
    if (!fn) {
        fprintf(stderr, "Internal error: unable to create new "
                "function '%s'.\n",
                new_name.c_str());
        abort();
    }

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", llvm_fn);
    int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);

    gen->pushGlobalFunction(fn);
    gen->pushGlobalBlock(block);

    ctx->activateAnonymousNamespace();

    return error_count;
}

void
removeTemporaryGlobalFunction(Generator *gen, int error_count,
                              std::vector<DeferredGoto*> *dgs,
                              std::map<std::string, Label*> *mls)
{
    Context *ctx = gen->ctx;
    if (error_count >= 0) {
        ctx->er->popErrors(error_count);
    }
    ctx->deactivateAnonymousNamespace();
    Function *current = gen->global_function;
    gen->popGlobalFunction();
    gen->popGlobalBlock();
    current->llvm_function->eraseFromParent();
}

extern "C" {
Node *
WrapNode(Node *n)
{
    std::vector<Node *> *nodes = new std::vector<Node *>;
    nodes->push_back(n);
    Node *holder = new Node(nodes);
    return holder;
}

bool
types_2D_equal(MContext *mc, DNode *t1, DNode *t2)
{
    Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(t1);
    n = g->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    Node *n2 = g->getUnit()->dnc->toNode(t2);
    n2 = g->parseOptionalMacroCall(n2);
    if (!n2) {
        return false;
    }

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *type1 = FormTypeParse(g, n,  false, false);
    Type *type2 = FormTypeParse(g, n2, false, false);

    g->ctx->er->popErrors(original_error_count);
    if (!type1 || !type2) {
        return 0;
    }
    return (type1->isEqualTo(type2));
}

bool
is_2D_char_2D_type(MContext *mc, DNode *t)
{
    Generator *g = (dale::Generator*) mc->generator;
    Node *n = g->getUnit()->dnc->toNode(t);
    n = g->parseOptionalMacroCall(n);
    return (n && n->token && !n->token->str_value.compare("char"));
}

static bool
get_type(MContext *mc, DNode *dnode, Type **type)
{
    Generator *g = (dale::Generator*) mc->generator;
    Node *n = g->getUnit()->dnc->toNode(dnode);
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

bool
is_2D_integer_2D_type(MContext *mc, DNode *t)
{
    Type *type;
    bool res = get_type(mc, t, &type);
    if (!res) {
        return false;
    }
    return type->isIntegerType();
}

bool
is_2D_signed_2D_integer_2D_type(MContext *mc, DNode *t)
{
    Type *type;
    bool res = get_type(mc, t, &type);
    if (!res) {
        return false;
    }
    return type->isSignedIntegerType();
}

bool
is_2D_unsigned_2D_integer_2D_type(MContext *mc, DNode *t)
{
    Type *type;
    bool res = get_type(mc, t, &type);
    if (!res) {
        return false;
    }
    return (type->isIntegerType() && !(type->isSignedIntegerType()));
}

bool
is_2D_floating_2D_point_2D_type(MContext *mc, DNode *t)
{
    Type *type;
    bool res = get_type(mc, t, &type);
    if (!res) {
        return false;
    }
    return type->isFloatingPointType();
}

bool
is_2D_pointer_2D_type(MContext *mc, DNode *t)
{
    Type *type;
    bool res = get_type(mc, t, &type);
    if (!res) {
        return false;
    }
    return (type->points_to ? true : false);
}

bool
is_2D_pointer_2D_to_2D_type(MContext *mc, DNode *t, DNode *pointee)
{
    Generator *g = (dale::Generator*) mc->generator;
    Node *n = g->getUnit()->dnc->toNode(t);
    n = g->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    Node *n2 = g->getUnit()->dnc->toNode(pointee);
    n2 = g->parseOptionalMacroCall(n2);
    if (!n2) {
        return false;
    }

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *type  = FormTypeParse(g, n,  false, false);
    Type *type2 = FormTypeParse(g, n2, false, false);

    g->ctx->er->popErrors(original_error_count);
    if (!type || !type2) {
        return false;
    }
    return (type->points_to->isEqualTo(type2));
}

DNode *
pointee_2D_type(MContext *mc, DNode *t)
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

bool
must_2D_init(MContext *mc, DNode *t)
{
    dale::Generator *g = (dale::Generator*) mc->generator;
    Node *n = g->getUnit()->dnc->toNode(t);

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
        Struct *st =
            g->ctx->getStruct(
                ptype->struct_name.c_str(),
                &(ptype->namespaces)
            );
        return !!(st->must_init);
    }
    return false;
}

bool
is_2D_const(MContext *mc, DNode *t)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(t);

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

bool
has_2D_errors(MContext *mc, DNode *form)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    bool made_temp = false;
    std::vector<DeferredGoto*> dgs;
    std::map<std::string, Label*> mls;
    if (!g->global_function) {
        makeTemporaryGlobalFunction(g, &dgs, &mls);
        made_temp = true;
    }

    ParseResult pr;

    /* POMC may succeed, but the underlying macro may return a null
     * DNode pointer.  This is not necessarily an error. */
    n = g->parseOptionalMacroCall(n);
    if (n) {
        FormProcInstParse(g, g->global_function, g->global_block,
                          n, false, false, NULL, &pr);
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

bool
exists_2D_fn(MContext *mc, DNode *form)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);

    if (!g->ctx->er->assertArgNums("exists-fn", n, 1, -1)) {
        return false;
    }

    symlist *lst = n->list;

    Node *node_return_type   = (*lst)[0];
    Node *node_function_name = (*lst)[1];

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *return_type = FormTypeParse(g, node_return_type, false, false);
    if (!return_type) {
        g->ctx->er->popErrors(original_error_count);
        return false;
    }

    std::vector<Type *> parameter_types;

    std::vector<Node *>::iterator iter = lst->begin();
    std::advance(iter, 2);

    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(g, (*iter), false, false);
        if (!parameter_type) {
            g->ctx->er->popErrors(original_error_count);
            return false;
        }
        if (parameter_type->base_type == BaseType::Void) {
            break;
        }
        parameter_types.push_back(parameter_type);
        ++iter;
    }

    Function *fn =
        g->ctx->getFunction(node_function_name->token->str_value.c_str(),
                            &parameter_types, NULL, 0);

    g->ctx->er->popErrors(original_error_count);
    return (fn && !fn->is_macro);
}

bool
exists_2D_type(MContext *mc, DNode *t)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = WrapNode(g->getUnit()->dnc->toNode(t));

    if (!g->ctx->er->assertArgNums("exists-type", n, 0, 0)) {
        return false;
    }

    symlist *lst = n->list;
    int error_count = g->ctx->er->getErrorTypeCount(ErrorType::Error);
    Type *type = FormTypeParse(g, (*lst)[0], false, false);
    g->ctx->er->popErrors(error_count);

    return !!type;
}

bool
exists_2D_macro(MContext *mc, DNode *form)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);

    if (!g->ctx->er->assertArgNums("exists-macro", n, 0, -1)) {
        return false;
    }

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    symlist *lst = n->list;
    Node *function_name = (*lst)[0];

    std::vector<Type *> parameter_types;

    std::vector<Node *>::iterator iter = lst->begin();
    ++iter;

    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(g, (*iter), false, false);
        if (!parameter_type) {
            g->ctx->er->popErrors(original_error_count);
            return false;
        }
        if (parameter_type->base_type == BaseType::Void) {
            break;
        }
        parameter_types.push_back(parameter_type);
        ++iter;
    }

    Function *fn =
        g->ctx->getFunction(function_name->token->str_value.c_str(),
                            &parameter_types, NULL, 1);

    g->ctx->er->popErrors(original_error_count);
    return (fn && fn->is_macro);
}

bool
exists_2D_variable(MContext *mc, DNode *form)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);
    n = g->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!n->token) {
        return false;
    }

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    Variable *var =
        g->ctx->getVariable(n->token->str_value.c_str());

    g->ctx->er->popErrors(original_error_count);
    return (var ? true : false);
}

int
fn_2D_by_2D_args_2D_count(MContext *mc, DNode *form, const char *prefix)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);

    if (!g->ctx->er->assertArgNums("fn-by-args-count", n, 0, -1)) {
        return 0;
    }

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    symlist *lst = n->list;

    std::vector<Node *>::iterator iter = lst->begin();

    std::string map_key;
    std::vector<Type *> parameter_types;
    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(g, (*iter), false, false);
        if (!parameter_type) {
            g->ctx->er->popErrors(original_error_count);
            return 0;
        }
        parameter_type->toString(&map_key);
        parameter_types.push_back(parameter_type);
        ++iter;
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
        Function *fn = g->ctx->getFunction(b->c_str(), &parameter_types,
                                           NULL, 0);
        if (fn && !fn->is_macro) {
            fn_by_args_list->push_back(*b);
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

const char*
fn_2D_by_2D_args_2D_name(MContext *mc, DNode *form, int index)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);

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
        Type *parameter_type = FormTypeParse(g, (*iter), false, false);
        if (!parameter_type) {
            g->ctx->er->popErrors(original_error_count);
            return NULL;
        }
        parameter_type->toString(&map_key);
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

bool
register_2D_type(MContext *mc, const char *from, const char *to)
{
    dale::addTypeMapEntry(from, to);
    return true;
}

DNode *
type_2D_of(MContext *mc, DNode *form)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);

    bool made_temp = false;
    std::vector<DeferredGoto*> dgs;
    std::map<std::string, Label*> mls;
    if (!g->global_function) {
        makeTemporaryGlobalFunction(g, &dgs, &mls);
        made_temp = true;
    }

    ParseResult pr;
    bool res =
        FormProcInstParse(g, g->global_function, g->global_block,
                          n, false, false, NULL, &pr);

    if (made_temp) {
        removeTemporaryGlobalFunction(g, -1, &dgs, &mls);
    }

    if (!res) {
        return NULL;
    }

    return pr.type->toNode()->toDNode();
}

const char *
printf_2D_length(MContext *mc, DNode *t)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(t);
    if (!n->is_token) {
        return "";
    }
    std::string *str = &(n->token->str_value);

    if (!str->compare("char")) {
        return "hh";
    }
    if (!str->compare("size")) {
        return "z";
    }
    if (!str->compare("ptrdiff")) {
        return "t";
    }
    if (!str->compare("long-double")) {
        return "L";
    }

    /* The type token now should be an int or uint, followed by a
     * number.  Confirm that this is so, get the number, divide it
     * by four and return the appropriate length character. */
    if (str->find("int") == 0) {
        str->erase(str->begin(), str->begin() + 3);
    } else if (str->find("uint") == 0) {
        str->erase(str->begin(), str->begin() + 4);
    } else {
        return "";
    }

    unsigned long addnum = strtoul(str->c_str(), NULL, DECIMAL_RADIX);
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

bool
type_2D_to_2D_string(MContext *mc, DNode *t, char *buf)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = WrapNode(g->getUnit()->dnc->toNode(t));

    Type *type = FormTypeParse(g, (*(n->list))[0], false,
                                       false);
    if (!type) {
        return false;
    }

    std::string symbol_string;
    type->toSymbolString(&symbol_string);
    if (symbol_string.size() > 255) {
        fprintf(stderr, "Internal error: encoded type "
                "string is too long (>255 characters): %s.\n",
                symbol_string.c_str());
    }
    std::copy(symbol_string.begin(), symbol_string.end(), buf);
    buf[symbol_string.size()] = '\0';

    return true;
}

bool
type_2D_to_2D_display_2D_string(MContext *mc, DNode *t, char *buf)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = WrapNode(g->getUnit()->dnc->toNode(t));

    Type *type = FormTypeParse(g, (*(n->list))[0], false,
                                       false);
    if (!type) {
        return false;
    }

    std::string type_string;
    type->toString(&type_string);
    std::copy(type_string.begin(), type_string.end(), buf);
    buf[type_string.size()] = '\0';

    return true;
}

void
report_2D_error(MContext *mc, DNode *form, char *str)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *n = g->getUnit()->dnc->toNode(form);

    Error *e = new Error(ErrorInst::Generator::ExternalError, n, str);
    g->ctx->er->addError(e);
}

int
arity(MContext *mc, DNode *name)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *fn_node = g->getUnit()->dnc->toNode(name);
    fn_node = g->parseOptionalMacroCall(fn_node);
    if (!fn_node) {
        return -1;
    }
    const char *fn_name = fn_node->token->str_value.c_str();

    Function *fn = g->ctx->getFunction(fn_name, NULL, NULL, 0);
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

    Node *n = g->getUnit()->dnc->toNode(form);

    assert(n->list && "must receive a list!");

    if (!g->ctx->er->assertArgNums("codomain", n, 0, -1)) {
        return NULL;
    }

    symlist *lst = n->list;

    Node *function_name = (*lst)[0];

    int original_error_count =
        g->ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Type *> parameter_types;

    std::vector<Node *>::iterator iter = lst->begin();
    ++iter;

    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(g, (*iter), false, false);
        if (!parameter_type) {
            g->ctx->er->popErrors(original_error_count);
            return NULL;
        }
        if (parameter_type->base_type == BaseType::Void) {
            break;
        }
        parameter_types.push_back(parameter_type);
        ++iter;
    }

    Function *fn =
        g->ctx->getFunction(function_name->token->str_value.c_str(),
                            &parameter_types, NULL, 0);

    g->ctx->er->popErrors(original_error_count);
    if (fn && !fn->is_macro) {
        return fn->return_type->toNode()->toDNode();
    } else {
        return NULL;
    }
}

DNode *
input_2D_type(MContext *mc, DNode *fn_name_nd, int index)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *fn_node = g->getUnit()->dnc->toNode(fn_name_nd);
    fn_node = g->parseOptionalMacroCall(fn_node);
    if (!fn_node) {
        return NULL;
    }
    const char *fn_name = fn_node->token->str_value.c_str();

    Function *fn = g->ctx->getFunction(fn_name, NULL, NULL, 0);
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

DNode *
struct_2D_member_2D_type(MContext *mc, DNode *name, int index)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *struct_node = g->getUnit()->dnc->toNode(name);
    struct_node = g->parseOptionalMacroCall(struct_node);
    if (!struct_node || !struct_node->token) {
        return NULL;
    }
    const char *struct_name = struct_node->token->str_value.c_str();

    Struct *st = g->ctx->getStruct(struct_name);
    if (!st) {
        return NULL;
    }
    if ((int) st->member_types.size() < index) {
        return NULL;
    }
    return st->member_types.at(index)->toNode()->toDNode();
}

const char *
struct_2D_member_2D_name(MContext *mc, DNode *name, int index)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *struct_node = g->getUnit()->dnc->toNode(name);
    struct_node = g->parseOptionalMacroCall(struct_node);
    if (!struct_node || !struct_node->token) {
        return NULL;
    }
    const char *struct_name = struct_node->token->str_value.c_str();

    Struct *st = g->ctx->getStruct(struct_name);
    if (!st) {
        return NULL;
    }
    if ((int) st->member_types.size() < index) {
        return NULL;
    }
    return st->indexToMember(index);
}

int
struct_2D_member_2D_count(MContext *mc, DNode *name)
{
    dale::Generator *g = (dale::Generator*) mc->generator;

    Node *struct_node = g->getUnit()->dnc->toNode(name);
    struct_node = g->parseOptionalMacroCall(struct_node);
    if (!struct_node || !struct_node->token) {
        return -1;
    }
    const char *struct_name = struct_node->token->str_value.c_str();

    Struct *st = g->ctx->getStruct(struct_name);
    if (!st) {
        return -1;
    }
    return st->member_types.size();
}

static std::map<std::string, void*> fns;

void
init_introspection_functions(void)
{
    fns.clear();
    fns["exists-fn"]                = (void *) exists_2D_fn;
    fns["exists-type"]              = (void *) exists_2D_type;
    fns["type-of"]                  = (void *) type_2D_of;
    fns["printf-length"]            = (void *) printf_2D_length;
    fns["exists-variable"]          = (void *) exists_2D_variable;
    fns["exists-macro"]             = (void *) exists_2D_macro;
    fns["report-error"]             = (void *) report_2D_error;
    fns["type-to-string"]           = (void *) type_2D_to_2D_string;
    fns["type-to-display-string"]   = (void *) type_2D_to_2D_display_2D_string;
    fns["register-type"]            = (void *) register_2D_type;
    fns["struct-member-count"]      = (void *) struct_2D_member_2D_count;
    fns["struct-member-type"]       = (void *) struct_2D_member_2D_type;
    fns["struct-member-name"]       = (void *) struct_2D_member_2D_name;
    fns["input-type"]               = (void *) input_2D_type;
    fns["is-char-type"]             = (void *) is_2D_char_2D_type;
    fns["is-integer-type"]          = (void *) is_2D_integer_2D_type;
    fns["is-signed-integer-type"]   = (void *) is_2D_signed_2D_integer_2D_type;
    fns["is-unsigned-integer-type"] = (void *) is_2D_unsigned_2D_integer_2D_type;
    fns["is-floating-point-type"]   = (void *) is_2D_floating_2D_point_2D_type;
    fns["is-pointer-type"]          = (void *) is_2D_pointer_2D_type;
    fns["is-pointer-to-type"]       = (void *) is_2D_pointer_2D_to_2D_type;
    fns["pointee-type"]             = (void *) pointee_2D_type;
    fns["fn-by-args-count"]         = (void *) fn_2D_by_2D_args_2D_count;
    fns["fn-by-args-name"]          = (void *) fn_2D_by_2D_args_2D_name;
    fns["has-errors"]               = (void *) has_2D_errors;
    fns["is-const"]                 = (void *) is_2D_const;
    fns["must-init"]                = (void *) must_2D_init;
}

#define eq(str) !strcmp(name, str)

void *
find_introspection_function(const char *name)
{
    std::map<std::string, void*>::iterator b = fns.find(name);
    if (b == fns.end()) {
        return NULL;
    }
    return b->second;
}
}
