#include "Introspection.h"
#include "../Type/Type.h"
#include "../Form/Type/Type.h"
#include "../Form/Proc/Inst/Inst.h"
#include "../Utils/Utils.h"

using namespace dale;

std::map<std::string, std::vector<std::string>*> fn_by_args;

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
    Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(t1);
    n = units->top()->mp->parsePotentialMacroCall(n);
    if (!n) {
        return false;
    }

    Node *n2 = units->top()->dnc->toNode(t2);
    n2 = units->top()->mp->parsePotentialMacroCall(n2);
    if (!n2) {
        return false;
    }

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *type1 = FormTypeParse(units, n,  false, false);
    Type *type2 = FormTypeParse(units, n2, false, false);

    units->top()->ctx->er->popErrors(error_count_begin);
    if (!type1 || !type2) {
        return 0;
    }
    return (type1->isEqualTo(type2));
}

bool
is_2D_char_2D_type(MContext *mc, DNode *t)
{
    Units *units = (dale::Units*) mc->units;
    Node *n = units->top()->dnc->toNode(t);
    n = units->top()->mp->parsePotentialMacroCall(n);
    return (n && n->token && !n->token->str_value.compare("char"));
}

static bool
get_type(MContext *mc, DNode *dnode, Type **type)
{
    Units *units = (dale::Units*) mc->units;
    Node *n = units->top()->dnc->toNode(dnode);
    n = units->top()->mp->parsePotentialMacroCall(n);
    if (!n) {
        return false;
    }

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    *type = FormTypeParse(units, n, false, false);

    units->top()->ctx->er->popErrors(error_count_begin);
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
    Units *units = (dale::Units*) mc->units;
    Node *n = units->top()->dnc->toNode(t);
    n = units->top()->mp->parsePotentialMacroCall(n);
    if (!n) {
        return false;
    }

    Node *n2 = units->top()->dnc->toNode(pointee);
    n2 = units->top()->mp->parsePotentialMacroCall(n2);
    if (!n2) {
        return false;
    }

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *type  = FormTypeParse(units, n,  false, false);
    Type *type2 = FormTypeParse(units, n2, false, false);

    units->top()->ctx->er->popErrors(error_count_begin);
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
    dale::Units *units = (dale::Units*) mc->units;
    Node *n = units->top()->dnc->toNode(t);

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *ptype = FormTypeParse(units, n, false, false);
    if (!ptype) {
        units->top()->ctx->er->popErrors(error_count_begin);
        return false;
    }

    if (ptype->is_const) {
        return true;
    }
    if (ptype->struct_name.size()) {
        Struct *st =
            units->top()->ctx->getStruct(
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(t);

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *ptype = FormTypeParse(units, n, false, false);
    if (!ptype) {
        units->top()->ctx->er->popErrors(error_count_begin);
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    bool made_temp = false;
    std::vector<DeferredGoto*> dgs;
    std::map<std::string, Label*> mls;
    if (!units->top()->getGlobalFunction()) {
        units->top()->makeTemporaryGlobalFunction();
        made_temp = true;
    }

    ParseResult pr;

    /* POMC may succeed, but the underlying macro may return a null
     * DNode pointer.  This is not necessarily an error. */
    n = units->top()->mp->parsePotentialMacroCall(n);
    if (n) {
        FormProcInstParse(units, units->top()->getGlobalFunction(),
                          units->top()->getGlobalBlock(),
                          n, false, false, NULL, &pr);
    }

    if (made_temp) {
        units->top()->removeTemporaryGlobalFunction();
    }

    int error_count_end =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    bool has_errors =
        ((error_count_end - error_count_begin) != 0);

    units->top()->ctx->er->popErrors(error_count_begin);
    return has_errors;
}

bool
exists_2D_fn(MContext *mc, DNode *form)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    if (!units->top()->ctx->er->assertArgNums("exists-fn", n, 1, -1)) {
        return false;
    }

    std::vector<Node *> *lst = n->list;

    Node *node_return_type   = (*lst)[0];
    Node *node_function_name = (*lst)[1];

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    Type *return_type = FormTypeParse(units, node_return_type, false, false);
    if (!return_type) {
        units->top()->ctx->er->popErrors(error_count_begin);
        return false;
    }

    std::vector<Type *> parameter_types;

    std::vector<Node *>::iterator iter = lst->begin();
    std::advance(iter, 2);

    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(units, (*iter), false, false);
        if (!parameter_type) {
            units->top()->ctx->er->popErrors(error_count_begin);
            return false;
        }
        if (parameter_type->base_type == BaseType::Void) {
            break;
        }
        parameter_types.push_back(parameter_type);
        ++iter;
    }

    Function *fn =
        units->top()->ctx->getFunction(node_function_name->token->str_value.c_str(),
                            &parameter_types, NULL, 0);

    units->top()->ctx->er->popErrors(error_count_begin);
    return (fn && !fn->is_macro);
}

bool
exists_2D_type(MContext *mc, DNode *t)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = WrapNode(units->top()->dnc->toNode(t));

    if (!units->top()->ctx->er->assertArgNums("exists-type", n, 0, 0)) {
        return false;
    }

    std::vector<Node *> *lst = n->list;
    int error_count = units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);
    Type *type = FormTypeParse(units, (*lst)[0], false, false);
    units->top()->ctx->er->popErrors(error_count);

    return !!type;
}

bool
exists_2D_macro(MContext *mc, DNode *form)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    if (!units->top()->ctx->er->assertArgNums("exists-macro", n, 0, -1)) {
        return false;
    }

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node *> *lst = n->list;
    Node *function_name = (*lst)[0];

    std::vector<Type *> parameter_types;

    std::vector<Node *>::iterator iter = lst->begin();
    ++iter;

    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(units, (*iter), false, false);
        if (!parameter_type) {
            units->top()->ctx->er->popErrors(error_count_begin);
            return false;
        }
        if (parameter_type->base_type == BaseType::Void) {
            break;
        }
        parameter_types.push_back(parameter_type);
        ++iter;
    }

    Function *fn =
        units->top()->ctx->getFunction(function_name->token->str_value.c_str(),
                            &parameter_types, NULL, 1);

    units->top()->ctx->er->popErrors(error_count_begin);
    return (fn && fn->is_macro);
}

bool
exists_2D_variable(MContext *mc, DNode *form)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);
    n = units->top()->mp->parsePotentialMacroCall(n);
    if (!n) {
        return false;
    }
    if (!n->token) {
        return false;
    }

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    Variable *var =
        units->top()->ctx->getVariable(n->token->str_value.c_str());

    units->top()->ctx->er->popErrors(error_count_begin);
    return (var ? true : false);
}

int
fn_2D_by_2D_args_2D_count(MContext *mc, DNode *form, const char *prefix)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    if (!units->top()->ctx->er->assertArgNums("fn-by-args-count", n, 0, -1)) {
        return 0;
    }

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node *> *lst = n->list;

    std::vector<Node *>::iterator iter = lst->begin();

    std::string map_key;
    std::vector<Type *> parameter_types;
    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(units, (*iter), false, false);
        if (!parameter_type) {
            units->top()->ctx->er->popErrors(error_count_begin);
            return 0;
        }
        parameter_type->toString(&map_key);
        parameter_types.push_back(parameter_type);
        ++iter;
    }

    std::map<std::string, std::vector<std::string>*>::iterator
        b = fn_by_args.find(map_key),
        e = fn_by_args.end();
    if (b != e) {
        std::vector<std::string>* fn_by_args_list = b->second;
        delete fn_by_args_list;
        fn_by_args.erase(b);
    }

    /* For each function that exists, see if it has an instance
     * for the provided parameter types. */

    std::set<std::string> function_names;
    bool has_prefix = prefix;
    if (!prefix) {
        prefix = "";
    }
    std::string ss_prefix(prefix);
    units->top()->ctx->getFunctionNames(&function_names,
                             (has_prefix ? &ss_prefix : NULL));
    std::vector<std::string> *fn_by_args_list =
        new std::vector<std::string>;

    for (std::set<std::string>::iterator
            b = function_names.begin(),
            e = function_names.end();
            b != e;
            ++b) {
        Function *fn = units->top()->ctx->getFunction(b->c_str(), &parameter_types,
                                           NULL, 0);
        if (fn && !fn->is_macro) {
            fn_by_args_list->push_back(*b);
        }
    }

    fn_by_args.insert(
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    if (!units->top()->ctx->er->assertArgNums("fn-by-args-name", n, 0, -1)) {
        return NULL;
    }

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node *> *lst = n->list;
    std::vector<Node *>::iterator iter = lst->begin();

    std::string map_key;
    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(units, (*iter), false, false);
        if (!parameter_type) {
            units->top()->ctx->er->popErrors(error_count_begin);
            return NULL;
        }
        parameter_type->toString(&map_key);
        ++iter;
    }

    std::map<std::string, std::vector<std::string>*>::iterator
        b = fn_by_args.find(map_key),
        e = fn_by_args.end();
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    bool made_temp = false;
    std::vector<DeferredGoto*> dgs;
    std::map<std::string, Label*> mls;
    if (!units->top()->getGlobalFunction()) {
        units->top()->makeTemporaryGlobalFunction();
        made_temp = true;
    }

    ParseResult pr;
    bool res =
        FormProcInstParse(units, units->top()->getGlobalFunction(),
                          units->top()->getGlobalBlock(),
                          n, false, false, NULL, &pr);

    if (made_temp) {
        units->top()->removeTemporaryGlobalFunction();
    }

    if (!res) {
        return NULL;
    }

    return pr.type->toNode()->toDNode();
}

const char *
printf_2D_length(MContext *mc, DNode *t)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(t);
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = WrapNode(units->top()->dnc->toNode(t));

    Type *type = FormTypeParse(units, (*(n->list))[0], false,
                                       false);
    if (!type) {
        return false;
    }

    std::string symbol_string;
    type->toSymbolString(&symbol_string);
    assert((symbol_string.size() <= 255)
           && "encoded type string is too long (>255 characters)");
    std::copy(symbol_string.begin(), symbol_string.end(), buf);
    buf[symbol_string.size()] = '\0';

    return true;
}

bool
type_2D_to_2D_display_2D_string(MContext *mc, DNode *t, char *buf)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = WrapNode(units->top()->dnc->toNode(t));

    Type *type = FormTypeParse(units, (*(n->list))[0], false,
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    Error *e = new Error(ErrorInst::ExternalError, n, str);
    units->top()->ctx->er->addError(e);
}

int
arity(MContext *mc, DNode *name)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *fn_node = units->top()->dnc->toNode(name);
    fn_node = units->top()->mp->parsePotentialMacroCall(fn_node);
    if (!fn_node) {
        return -1;
    }
    const char *fn_name = fn_node->token->str_value.c_str();

    Function *fn = units->top()->ctx->getFunction(fn_name, NULL, NULL, 0);
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *n = units->top()->dnc->toNode(form);

    if (!units->top()->ctx->er->assertArgNums("codomain", n, 0, -1)) {
        return NULL;
    }

    std::vector<Node *> *lst = n->list;

    Node *function_name = (*lst)[0];

    int error_count_begin =
        units->top()->ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Type *> parameter_types;

    std::vector<Node *>::iterator iter = lst->begin();
    ++iter;

    while (iter != lst->end()) {
        Type *parameter_type = FormTypeParse(units, (*iter), false, false);
        if (!parameter_type) {
            units->top()->ctx->er->popErrors(error_count_begin);
            return NULL;
        }
        if (parameter_type->base_type == BaseType::Void) {
            break;
        }
        parameter_types.push_back(parameter_type);
        ++iter;
    }

    Function *fn =
        units->top()->ctx->getFunction(function_name->token->str_value.c_str(),
                            &parameter_types, NULL, 0);

    units->top()->ctx->er->popErrors(error_count_begin);
    if (fn && !fn->is_macro) {
        return fn->return_type->toNode()->toDNode();
    } else {
        return NULL;
    }
}

DNode *
input_2D_type(MContext *mc, DNode *fn_name_nd, int index)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *fn_node = units->top()->dnc->toNode(fn_name_nd);
    fn_node = units->top()->mp->parsePotentialMacroCall(fn_node);
    if (!fn_node) {
        return NULL;
    }
    const char *fn_name = fn_node->token->str_value.c_str();

    Function *fn = units->top()->ctx->getFunction(fn_name, NULL, NULL, 0);
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *struct_node = units->top()->dnc->toNode(name);
    struct_node = units->top()->mp->parsePotentialMacroCall(struct_node);
    if (!struct_node || !struct_node->token) {
        return NULL;
    }
    const char *struct_name = struct_node->token->str_value.c_str();

    Struct *st = units->top()->ctx->getStruct(struct_name);
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
    dale::Units *units = (dale::Units*) mc->units;

    Node *struct_node = units->top()->dnc->toNode(name);
    struct_node = units->top()->mp->parsePotentialMacroCall(struct_node);
    if (!struct_node || !struct_node->token) {
        return NULL;
    }
    const char *struct_name = struct_node->token->str_value.c_str();

    Struct *st = units->top()->ctx->getStruct(struct_name);
    if (!st) {
        return NULL;
    }
    if ((int) st->member_types.size() < index) {
        return NULL;
    }
    return st->indexToName(index);
}

int
struct_2D_member_2D_count(MContext *mc, DNode *name)
{
    dale::Units *units = (dale::Units*) mc->units;

    Node *struct_node = units->top()->dnc->toNode(name);
    struct_node = units->top()->mp->parsePotentialMacroCall(struct_node);
    if (!struct_node || !struct_node->token) {
        return -1;
    }
    const char *struct_name = struct_node->token->str_value.c_str();

    Struct *st = units->top()->ctx->getStruct(struct_name);
    if (!st) {
        return -1;
    }
    return st->member_types.size();
}

static std::map<std::string, void*> fns;

void
init_introspection_functions()
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
