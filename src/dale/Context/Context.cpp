#include "Context.h"

#include "llvm/LinkAllVMCore.h"
#include "../NativeTypes/NativeTypes.h"

#define DEBUG 0

namespace dale
{
Context::Context(ErrorReporter *new_erep)
{
    nt = new NativeTypes();
    functions = new std::map<std::string,
    std::vector<Element::Function *>* >;
    variables = new std::map<std::string, Element::Variable *>;
    structs   = new std::map<std::string, Element::Struct *>;
    enums     = new std::map<std::string, Element::Enum *>;
    labels    = new std::map<std::string, Element::Label *>;

    /* active_namespaces determines the namespace into which new
     * bindings will be put. */
    active_namespaces    =
        new std::vector<std::pair<std::string, Context*> >;
    /* used_namespaces is the list of namespaces for lookup -
     * introduced via a (using-namespace <x>) call. */
    used_namespaces      =
        new std::vector<std::pair<std::string, Context*> >;
    /* A map of all activated namespaces for the current context.
     * */
    namespace_to_context = new std::map<std::string, Context *>;

    erep = new_erep;

    current_namespaces = new std::vector<std::string>;

    current_context = NULL;
    parent_context = NULL;

    index = 0;
    next_sub_index = 1;
    top_context = NULL;
    lv_index = 0;
}

Context::~Context(void)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator iter
    = functions->begin();

    while (iter != functions->end()) {
        delete iter->second;
        ++iter;
    }
    functions->clear();
    delete functions;

    std::map<std::string, Element::Variable *>::iterator viter
    = variables->begin();

    while (viter != variables->end()) {
        delete viter->second;
        ++viter;
    }
    variables->clear();
    delete variables;

    delete labels;

    delete structs;

    std::map<std::string, Element::Enum *>::iterator eiter
    = enums->begin();

    while (eiter != enums->end()) {
        delete eiter->second;
        ++eiter;
    }

    delete enums;

    delete current_namespaces;
    delete active_namespaces;
    delete used_namespaces;
    delete namespace_to_context;
}

int Context::getContextDepth(Context *myctx, int *depth)
{
    for (std::map<std::string, Context *>::iterator
            b = namespace_to_context->begin(),
            e = namespace_to_context->end();
            b != e;
            ++b) {
        Context *check = b->second;
        if (check == myctx) {
            *depth = 0;
            return 1;
        }
        if (check->getContextDepth(myctx, depth)) {
            *depth = *depth + 1;
            return 1;
        }
    }
    return 0;
}

int Context::getVarsAfterIndex(int index,
                               std::vector<Element::Variable *> *vars)
{
    for (std::map<std::string, Element::Variable *>::iterator
            vb = variables->begin(),
            ve = variables->end();
            vb != ve;
            ++vb) {
        Element::Variable *v = vb->second;
        if (!v->index) {
            continue;
        }
        if (v->index >= index) {
            vars->push_back(v);
        }
    }

    for (std::map<std::string, Context*>::iterator
            b = namespace_to_context->begin(),
            e = namespace_to_context->end();
            b != e;
            ++b) {
        Context *myctx = b->second;
        myctx->getVarsAfterIndex(index, vars);
    }

    return 1;
}

int Context::getVarsBeforeIndex(int index,
                                std::vector<Element::Variable *> *vars)
{
    for (std::map<std::string, Element::Variable *>::iterator
            vb = variables->begin(),
            ve = variables->end();
            vb != ve;
            ++vb) {
        Element::Variable *v = vb->second;
        if (!v->index) {
            continue;
        }
        if (v->index < index) {
            vars->push_back(v);
        }
    }

    for (std::map<std::string, Context*>::iterator
            b = namespace_to_context->begin(),
            e = namespace_to_context->end();
            b != e;
            ++b) {
        Context *myctx = b->second;
        myctx->getVarsBeforeIndex(index, vars);
    }

    return 1;
}

void Context::dump(void)
{
    for (std::map<std::string,
            std::vector<Element::Function *>* >::iterator
            b = functions->begin(), e = functions->end();
            b != e;
            ++b) {
        fprintf(stderr, "Function: %s (%d)\n", b->first.c_str(),
                (int) b->second->size());
    }

    for (std::map<std::string, Element::Variable*>::iterator
            b = variables->begin(), e = variables->end();
            b != e;
            ++b) {
        fprintf(stderr, "Variable: %s\n", b->first.c_str());
    }

    for (std::map<std::string, Element::Struct*>::iterator
            b = structs->begin(), e = structs->end();
            b != e;
            ++b) {
        fprintf(stderr, "Struct: %s\n", b->first.c_str());
    }

    for (std::map<std::string, Element::Enum*>::iterator
            b = enums->begin(), e = enums->end();
            b != e;
            ++b) {
        fprintf(stderr, "Enum: %s\n", b->first.c_str());
    }

    for (std::map<std::string, Element::Label *>::iterator
            b = labels->begin(), e = labels->end();
            b != e;
            ++b) {
        fprintf(stderr, "Label: %s\n", b->first.c_str());
    }

    for (std::map<std::string, Context*>::iterator
            b = namespace_to_context->begin(),
            e = namespace_to_context->end();
            b != e;
            ++b) {
        fprintf(stderr, "Sub-context: %s\n", b->first.c_str());
        Context *myctx = b->second;
        myctx->dump();
    }

    return;
}

int Context::getCurrentNamespaceIndex(void)
{
    Context& cc = (current_context == NULL) ? *this : *current_context;

    return cc.index;
}

int Context::popUntilNamespaceIndex(int index,
                                    std::vector<std::string> *nss)
{
    while (getCurrentNamespaceIndex() != index) {
        if (active_namespaces->size() == 0) {
            fprintf(stderr, "popUntilNamespaceIndex failed "
                    "(namespace not found).");
            abort();
        }
        std::string ns = active_namespaces->back().first;
        nss->push_back(ns);
        deactivateNamespace(ns.c_str());
    }

    return 1;
}

void Context::printNamespaces(void)
{
    std::vector<std::pair<std::string, Context*> >::iterator iter;

    printf("Active namespaces (%lu): ",
           (unsigned long) active_namespaces->size());

    for (iter = active_namespaces->begin();
            iter != active_namespaces->end();
            ++iter) {
        printf("%s ", (*iter).first.c_str());
    }
    printf("\n");

    printf("Used namespaces (%lu): ",
           (unsigned long) used_namespaces->size());
    for (iter = used_namespaces->begin();
            iter != used_namespaces->end();
            ++iter) {
        printf("%s ", (*iter).first.c_str());
    }
    printf("\n");
}

static int anon_ns = 0;

int Context::activateAnonymousNamespace(void)
{
    std::string anon("anon");
    char buf[256];
    sprintf(buf, "%d", anon_ns++);
    anon.append(buf);

    return activateNamespace(anon.c_str());
}

int Context::deactivateAnonymousNamespace(void)
{
    // Get the context from the back. Remove it from the parent's
    // context. Once an anonymous namespace is deactivated, you
    // will never need it again. (todo: leakage.)

    std::pair<std::string, Context *> &p =
        active_namespaces->back();
    p.second->parent_context->namespace_to_context->erase(p.first);

    used_namespaces->pop_back();
    active_namespaces->pop_back();

    current_context =
        (active_namespaces->empty())
        ? NULL
        : active_namespaces->back().second;

    return 1;
}

int Context::activateNamespace(const char *name)
{
    // Keep a pointer to the map where it should be added/checked
    // for.

    std::map<std::string, Context*> *ns_to_context =
        this->namespace_to_context;

    // Iterate over the active namespaces to find the current map.

    std::vector<std::pair<std::string, Context*> >::iterator ns_iter =
        active_namespaces->begin();

    std::map<std::string, Context*>::iterator ctx_iter;

    std::vector<std::string> new_current_namespaces;

    while (ns_iter != active_namespaces->end()) {
        std::string ns_name = ns_iter->first;

        new_current_namespaces.push_back(ns_name);

        ctx_iter = ns_to_context->find(ns_name);

        if (ctx_iter == ns_to_context->end()) {
            Error *e = new Error(
                ErrorInst::Generator::NamespaceNotInContext,
                new Node(),
                ns_name.c_str()
            );
            erep->addError(e);
            return 0;
        }

        ns_to_context = ctx_iter->second->namespace_to_context;

        ++ns_iter;
    }

    /* Check for a mapping. If it doesn't exist, then create it. */

    std::string n(name);
    new_current_namespaces.push_back(n);
    ctx_iter = ns_to_context->find(name);
    Context *new_current_ctx = NULL;

    if (ctx_iter == ns_to_context->end()) {
        Context *new_ctx = new Context(erep);
        new_ctx->parent_context =
            (current_context ? current_context : this);
        new_ctx->index   = next_sub_index++;
        new_current_ctx  = new_ctx;
        delete new_ctx->current_namespaces;
        new_ctx->current_namespaces =
            new std::vector<std::string>(new_current_namespaces);

        ns_to_context->insert(
            std::pair<std::string, Context *>(n, new_ctx)
        );
    } else {
        new_current_ctx = ctx_iter->second;
    }

    current_context = new_current_ctx;

    /* Add new namespace to active namespaces. */
    std::pair<std::string, Context*> new_pair(n, current_context);
    active_namespaces->push_back(new_pair);

    /* Add new namespace to used namespaces. */
    used_namespaces->push_back(new_pair);

    return 1;
}

int Context::unuseNamespace(const char *name)
{
    if (DEBUG) {
        fprintf(stderr,
                "Popping namespace, ignoring name "
                "argument (%s).\n", name);
    }
    used_namespaces->pop_back();

    return 1;
}

/* Assumption here is that this only fails on extern_c
 * linkage in namespaces. */
int Context::getNewFunctionName(const char *name,
                                std::string *new_name,
                                int linkage,
                                std::vector<Element::Variable *> *params) {

    if (linkage == dale::Linkage::Extern_C) {
        if (active_namespaces->size() > 0) {
            return 0;
        } else {
            new_name->append(name);
            return 1;
        }
    }

    getNewName(name, new_name);

    std::vector<Element::Variable *>::iterator iter = params->begin();
    while (iter != params->end()) {
        (*iter)->type->toEncStr(new_name);
        ++iter;
    }

    return 1;
}

int Context::getNewName(const char *name, std::string *new_name)
{
    new_name->clear();

    std::string name_ss(name);
    std::string encoded_ss;
    encodeNonAN(&name_ss, &encoded_ss);

    std::map<std::string, Context*>::iterator ctx_iter;

    std::vector<std::pair<std::string, Context*> >::iterator iter;

    bool has_active_namespaces = (active_namespaces->size() > 0);

    new_name->append("_Z");

    if (has_active_namespaces) {
        new_name->append("N");
    }

    for (iter = active_namespaces->begin();
            iter != active_namespaces->end();
            ++iter) {

        int len = iter->first.length();
        char num[255];
        sprintf(num, "%d", len);

        new_name->append(num);
        new_name->append(iter->first);
    }

    int len = strlen(name);
    char num[255];
    sprintf(num, "%d", len);
    new_name->append(num);

    new_name->append(encoded_ss);

    if (has_active_namespaces) {
        new_name->append("E");
    }

    return 1;
}

int Context::encodeNonAN(const std::string *from, std::string *to)
{
    std::string::const_iterator iter = from->begin();

    while (iter != from->end()) {
        char c = *iter;
        char buf[5];

        if (!isalnum(c) && c != '_') {
            sprintf(buf, "$%x", c);
        } else {
            sprintf(buf, "%c", c);
        }

        to->append(buf);

        ++iter;
    }

    return 1;
}

int Context::deactivateNamespace(const char *name)
{
    if (strcmp(name, active_namespaces->back().first.c_str())) {
        Error *e = new Error(
            ErrorInst::Generator::CannotDeactivateInactiveNamespace,
            new Node(),
            name
        );
        erep->addError(e);
        return 0;
    }

    if (strcmp(name, used_namespaces->back().first.c_str())) {
        Error *e = new Error(
            ErrorInst::Generator::CannotDeactivateNonLastNamespace,
            new Node(),
            name
        );
        erep->addError(e);
        return 0;
    }

    used_namespaces->pop_back();
    active_namespaces->pop_back();

    current_context =
        (active_namespaces->empty())
        ? NULL
        : active_namespaces->back().second;

    return 1;
}

void splitString(std::string *str, std::vector<std::string> *lst, char c)
{
    int index = 0;
    int len = str->length();

    while (index < len) {
        int found = str->find(c, index);
        if (found == -1) {
            found = str->length();
        }
        std::string temp(str->substr(index, found - index));
        lst->push_back(temp);
        index = found + 1;
    }
}

int Context::useNamespace(const char *name)
{
    std::string ns_name(name);

    Context *new_ctx;

    if (!getContextFromNamespace(name, &new_ctx, 0)) {
        return 0;
    }

    /* Split the string into its components, separated by periods, and
     * then use each namespace in turn. */

    std::vector<std::string> lst;
    splitString(&ns_name, &lst, '.');

    std::pair<std::string, Context*> new_pair(lst.back(), new_ctx);
    used_namespaces->push_back(new_pair);

    return 1;
}

int Context::addFunction(const char *name,
                         Element::Function *function,
                         Node *n)
{
    if (DEBUG)  printf("Adding function '%s'\n", name);

    std::map<std::string, std::vector<Element::Function *>* >::iterator iter;
    std::vector<Element::Function *>::iterator fn_iter;

    std::string temp_name(name);
    Context& cc = (current_context == NULL) ? *this : *current_context;

    iter = cc.functions->find(temp_name);

    /* There used to be a bunch of code here for checking for
     * duplicate declarations, etc. etc. This is now (mostly) handled
     * in the Generator. One thing that is done here is the checking
     * for functions that have the same types as an extant macro, and
     * vice-versa. */

    if (iter != cc.functions->end()) {
        if (!function->is_macro) {
            fn_iter = iter->second->begin();
            while (fn_iter != iter->second->end()) {
                if ((!(*fn_iter)->is_macro)) {
                    if ((*fn_iter) == function) {
                        /* Same function, no need to add. */
                        return 1;
                    } else if (function->isEqualTo((*fn_iter))
                               && (!(*fn_iter)->llvm_function ||
                                   !(*fn_iter)->llvm_function->size())) {
                        /* Declaration, remove the current
                         * function and replace with the new. */
                        iter->second->erase(fn_iter);
                    } else if (function->isEqualTo((*fn_iter))
                               && ((*fn_iter)->llvm_function &&
                                   (*fn_iter)->llvm_function->size())) {
                        /* Have got a populated function already,
                         * don't add this new function. */
                        return 1;
                    } else {
                        ++fn_iter;
                    }
                } else {
                    /* Iterator contains a macro. If the parameter
                     * type lists are the same (save for the first
                     * two macro arguments), then throw an error.
                     * */
                    std::vector<Element::Variable *>::iterator b2 =
                        (*fn_iter)->parameter_types->begin();
                    ++b2;
                    ++b2;
                    if (dale::stl::isEqualToIter(
                                function->parameter_types->begin(),
                                function->parameter_types->end(),
                                b2,
                                (*fn_iter)->parameter_types->end()
                            )) {
                        Node *mine;
                        if (n) {
                            mine = n;
                        }
                        else {
                            mine = new Node();
                        }
                        Error *e = new Error(
                            ErrorInst::Generator::FunctionHasSameParamsAsMacro,
                            mine, name
                        );
                        erep->addError(e);
                        return 0;
                    }

                    ++fn_iter;
                }
            }
        } else {
            /* Macro is being added. If a macro with a body
             * already exists, then don't add this new function.
             * */
            fn_iter = iter->second->begin();
            while (fn_iter != iter->second->end()) {
                if (((*fn_iter)->is_macro)) {
                    if ((*fn_iter) == function) {
                        /* Same function, no need to add. */
                        return 1;
                    } else if (function->isEqualTo((*fn_iter))
                               && (!(*fn_iter)->llvm_function ||
                                   !(*fn_iter)->llvm_function->size())) {
                        /* Declaration, remove the current
                         * function and replace with the new. */
                        iter->second->erase(fn_iter);
                    } else if (function->isEqualTo((*fn_iter))
                               && ((*fn_iter)->llvm_function &&
                                   (*fn_iter)->llvm_function->size())) {
                        /* Have got a populated macro already,
                         * don't add this new macro. */
                        return 1;
                    } else {
                        ++fn_iter;
                    }
                } else {
                    /* Iterator contains a function. If the parameter
                     * type lists are the same (save for the first
                     * two macro arguments), then throw an error.
                     * */
                    std::vector<Element::Variable *>::iterator b2 =
                        function->parameter_types->begin();
                    ++b2;
                    ++b2;
                    if (dale::stl::isEqualToIter(
                                b2,
                                function->parameter_types->end(),
                                (*fn_iter)->parameter_types->begin(),
                                (*fn_iter)->parameter_types->end()
                            )) {
                        Node *mine;
                        if (n) {
                            mine = n;
                        }
                        else {
                            mine = new Node();
                        }
                        Error *e = new Error(
                            ErrorInst::Generator::MacroHasSameParamsAsFunction,
                            mine, name
                        );
                        erep->addError(e);
                        return 0;
                    }
                    ++fn_iter;
                }
            }
        }
        iter->second->push_back(function);
    } else {
        std::vector<Element::Function *> *fns =
            new std::vector<Element::Function *>;
        fns->push_back(function);

        cc.functions->insert(
            std::pair<std::string, std::vector<Element::Function *> *>(
                temp_name, fns
            )
        );
    }

    return 1;
}

int Context::addVariable(const char *name, Element::Variable *variable)
{
    std::map<std::string, Element::Variable *>::iterator iter;
    std::string temp_name(name);
    Context& cc = (current_context == NULL) ? *this : *current_context;

    iter = cc.variables->find(temp_name);

    if (iter == cc.variables->end()) {
        cc.variables->insert(
            std::pair<std::string, Element::Variable *>(temp_name, variable)
        );
        variable->index = ++lv_index;
        return 1;
    } else {
        return 0;
    }
}

int Context::addLabel(const char *name, Element::Label *label)
{
    std::map<std::string, Element::Label *>::iterator iter;
    std::string temp_name(name);
    // Do not add to current_context, because labels are
    // function-scoped.

    iter = labels->find(temp_name);

    if (iter == labels->end()) {
        labels->insert(
            std::pair<std::string, Element::Label *>(temp_name, label)
        );
        label->index = ++lv_index;
        return 1;
    } else {
        return 0;
    }
}

int Context::addStruct(const char *name, Element::Struct *element_struct)
{
    std::map<std::string, Element::Struct *>::iterator iter;
    std::string temp_name(name);
    Context& cc = (current_context == NULL) ? *this : *current_context;

    iter = cc.structs->find(temp_name);

    if (iter == cc.structs->end()) {
        cc.structs->insert(
            std::pair<std::string, Element::Struct *>(
                temp_name, element_struct
            )
        );

        return 1;
    } else {
        return 0;
    }
}

int Context::addEnum(const char *name, Element::Enum *element_enum)
{
    std::map<std::string, Element::Enum *>::iterator iter;
    std::string temp_name(name);
    Context& cc = (current_context == NULL) ? *this : *current_context;

    iter = cc.enums->find(temp_name);

    if (iter == cc.enums->end()) {
        cc.enums->insert(
            std::pair<std::string, Element::Enum *>(
                temp_name, element_enum
            )
        );
        return 1;
    } else {
        return 0;
    }
}

int Context::removeVariable(const char *name)
{
    std::map<std::string, Element::Variable *>::iterator iter;
    std::string temp_name(name);
    Context& cc = (current_context == NULL) ? *this : *current_context;

    iter = cc.variables->find(temp_name);

    if (iter == cc.variables->end()) {
        return 0;
    } else {
        cc.variables->erase(iter);
        return 1;
    }
}

int Context::removeLabel(const char *name)
{
    std::map<std::string, Element::Label *>::iterator iter;
    std::string temp_name(name);

    iter = labels->find(temp_name);

    if (iter == labels->end()) {
        return 0;
    } else {
        labels->erase(iter);
        return 1;
    }
}

int Context::removeStruct(const char *name)
{
    std::map<std::string, Element::Struct *>::iterator iter;
    std::string temp_name(name);
    Context& cc = (current_context == NULL) ? *this : *current_context;

    iter = cc.structs->find(temp_name);

    if (iter == cc.structs->end()) {
        return 0;
    } else {
        delete iter->second;
        cc.structs->erase(iter);
        return 1;
    }
}

int Context::removeEnum(const char *name)
{
    std::map<std::string, Element::Enum *>::iterator iter;
    std::string temp_name(name);
    Context& cc = (current_context == NULL) ? *this : *current_context;

    iter = cc.enums->find(temp_name);

    if (iter == cc.enums->end()) {
        return 0;
    } else {
        delete iter->second;
        cc.enums->erase(iter);
        return 1;
    }
}

int Context::getContextFromNamespace(const char *name,
                                     Context **ctx_ptr_ptr,
                                     int ignore_last)
{
    if (name[0] == '.') {
        Context *cc = (top_context == NULL) ? this : top_context;
        *ctx_ptr_ptr = cc;
        return 1;
    }

    std::vector<std::string> lst;
    std::string str(name);
    splitString(&str, &lst, '.');

    std::vector<std::string>::iterator comp_iter = lst.begin();

    std::map<std::string, Context*> *ns_to_context = namespace_to_context;
    std::map<std::string, Context*>::iterator ctx_iter;

    while (comp_iter != lst.end()) {
        ctx_iter = ns_to_context->find((*comp_iter));

        if (ctx_iter == ns_to_context->end()) {
            return 0;
        }

        ns_to_context = ctx_iter->second->namespace_to_context;

        ++comp_iter;

        if (ignore_last && ((comp_iter + 1) == lst.end())) {
            break;
        }
    }

    *ctx_ptr_ptr = ctx_iter->second;

    return 1;
}

Element::Function *Context::getFunctionBare(
    std::vector<Element::Function *> *function_list,
    std::vector<Element::Type *> *types,
    Element::Function **pclosest_fn,
    int is_macro
) {
    if (!types) {
        Element::Function *last_no_size = NULL;

        std::vector<Element::Function *>::reverse_iterator iter;
        iter = function_list->rbegin();
        while (iter != function_list->rend()) {
            if ((*iter)->llvm_function
                    && (*iter)->llvm_function->size()
                    && (is_macro == (*iter)->is_macro)) {
                return (*iter);
            }
            last_no_size =
                (is_macro == (*iter)->is_macro)
                ? (*iter)
                : last_no_size;
            ++iter;
        }
        int fn_list_size = function_list->size();
        if (fn_list_size == 0) {
            return NULL;
        }
        return last_no_size;
    } else {
        /* If types are provided, is_macro is only taken into
         * account if it is true. This is because you may want to
         * get only the macros at a given point (the top-level),
         * but there is no point at which you want prospective
         * functions sans macros. */

        // Get an iterator over the function list.
        std::vector<Element::Function *>::iterator fn_iter;
        std::vector<Element::Type *>::iterator arg_type_iter;
        std::vector<Element::Variable *>::iterator fn_arg_type_iter;

        // Get an iterator over the types.
        fn_iter = function_list->begin();

        Element::Function *best_va_fn = NULL;
        int best_va_count = -1;

        Element::Function *decl_fn    = NULL;
        Element::Function *closest_fn = NULL;
        int best_closest_count = -1;

        // For each function:
        while (fn_iter != function_list->end()) {
            /* If the function is not a macro, but is_macro is
             * set, then skip it. */
            if (is_macro && !((*fn_iter)->is_macro)) {
                ++fn_iter;
                continue;
            }

            // Iterate over the function's arg types and the
            // provided types.
            fn_arg_type_iter = (*fn_iter)->parameter_types->begin();
            arg_type_iter    = types->begin();
            int matched_arg_count = 0;
            int broke_on_va       = 0;
            int broke_on_failure  = 0;

            /* If this is a macro, then increment the function's
             * parameter types iterator twice, to account for the
             * implicit arguments. */
            if ((*fn_iter)->is_macro) {
                ++fn_arg_type_iter;
                ++fn_arg_type_iter;
            }

            while (fn_arg_type_iter
                    != (*fn_iter)->parameter_types->end()) {

                // If the function's current element is
                // varargs, then record the number of real
                // arguments matched and keep a pointer to
                // this function (if the number of matched
                // arguments is better than that which is
                // currently recorded) - then go to the next
                // function.

                if ((*fn_arg_type_iter)->type->base_type
                        == Type::VarArgs) {
                    if (matched_arg_count > best_va_count) {
                        best_va_count = matched_arg_count;
                        best_va_fn = (*fn_iter);
                        broke_on_va = 1;
                        break;
                    } else {
                        broke_on_failure = 1;
                        break;
                    }
                }

                if (arg_type_iter == types->end()) {
                    broke_on_failure = 1;
                    break;
                }

                // If an element matches, keep going.
                // If it doesn't, go to the next function.
                if ((*arg_type_iter)->isEqualTo(
                            (*fn_arg_type_iter)->type,
                            IGNORE_ARG_CONSTNESS)) {
                    ++arg_type_iter;
                    ++fn_arg_type_iter;
                    ++matched_arg_count;
                    continue;
                } else {
                    broke_on_failure = 1;
                    break;
                }
            }

            if ((!broke_on_failure)
                    && (!broke_on_va)
                    && (arg_type_iter == types->end())) {
                // If the function is a declaration, store it
                // in decl_fn, to use in the event that the
                // real function cannot be found.

                if (!(*fn_iter)->llvm_function->size()) {
                    decl_fn = (*fn_iter);
                } else {
                    return (*fn_iter);
                }
            }

            if (broke_on_failure) {
                if (matched_arg_count > best_closest_count) {
                    best_closest_count = matched_arg_count;
                    closest_fn = (*fn_iter);
                }
            }

            ++fn_iter;
        }

        // If no exact match - is there a varargs match? - if
        // so, use that, otherwise, is there a declaration
        // match? if so, use that.

        if (best_va_fn) {
            return best_va_fn;
        } else if (decl_fn) {
            return decl_fn;
        }

        /* Nothing - set closest_fn (if it isn't null). */
        if (pclosest_fn) {
            *pclosest_fn = closest_fn;
        }

        /* If the argument type list does not comprise (p DNode)s,
         * then change the last argument to a (p DNode) and
         * re-call this function. (E.g. you may have a macro
         * called identity that takes a (p DNode) - if you call it
         * with a value parseable as an int, it won't be found
         * until you check for (p DNode) arguments. Undoubtedly
         * there is a much more efficient way of doing this. */
        std::vector<Element::Type *>::reverse_iterator rarg_type_iter;
        rarg_type_iter = types->rbegin();

        Element::Type *temp_type = new Element::Type();
        temp_type->struct_name = new std::string("DNode");
        temp_type->namespaces  = new std::vector<std::string>;
        Element::Type *r_type = new Element::Type(temp_type);

        while (rarg_type_iter != types->rend()) {
            Element::Type *temp = *rarg_type_iter;
            if (!( temp->points_to
                    &&
                    temp->points_to->struct_name
                    &&
                    !(temp->points_to
                      ->struct_name
                      ->compare("DNode")))) {
                break;
            }
            ++rarg_type_iter;
        }

        if (rarg_type_iter == types->rend()) {
            return NULL;
        }

        Element::Type *old_type = (*rarg_type_iter);
        (*rarg_type_iter) = r_type;
        Element::Function *temp = getFunctionBare(
                                      function_list,
                                      types,
                                      NULL,
                                      1
                                  );
        (*rarg_type_iter) = old_type;
        if (temp) {
            return temp;
        }

        return NULL;
    }
}

int Context::isOverloadedFunctionBare(
    Element::Function *check_fn,
    std::vector<Element::Function *> *function_list
)
{
    std::vector<Element::Function *>::iterator iter
    = function_list->begin();

    while (iter != function_list->end()) {
        if (!(*iter)->isEqualTo(check_fn)) {
            return 1;
        }
        ++iter;
    }

    return 0;
}

bool
Context::existsNonExternCFunctionBare(
    std::vector<Element::Function *> *fnvector)
{
    std::vector<Element::Function *>::iterator iter =
        fnvector->begin();

    while (iter != fnvector->end()) {
        if ((*iter)->return_type->linkage != Linkage::Extern_C) {
            return true;
        }
        ++iter;
    }

    return false;
}

bool Context::existsNonExternCFunction(const char *name)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator iter;
    std::string temp_name(name);

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return false;
        }
        std::vector<std::string> lst;
        splitString(&temp_name, &lst, '.');

        iter = new_ctx->functions->find(lst.back());

        if (iter != new_ctx->functions->end()) {
            return existsNonExternCFunctionBare(iter->second);
        }
        return false;
    }

    iter = functions->find(temp_name);

    if (iter != functions->end()) {
        return existsNonExternCFunctionBare(iter->second);
    }

    return false;
}

bool
Context::existsExternCFunctionBare(std::vector<Element::Function *> *fnvector)
{
    std::vector<Element::Function *>::iterator iter =
        fnvector->begin();

    while (iter != fnvector->end()) {
        if ((*iter)->return_type->linkage == Linkage::Extern_C) {
            return true;
        }
        ++iter;
    }

    return false;
}

bool Context::existsExternCFunction(const char *name)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator iter;
    std::string temp_name(name);

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return 0;
        }
        std::vector<std::string> lst;
        splitString(&temp_name, &lst, '.');

        iter = new_ctx->functions->find(lst.back());

        if (iter != new_ctx->functions->end()) {
            return existsExternCFunctionBare(iter->second);
        }
        return 0;
    }

    iter = functions->find(temp_name);

    if (iter != functions->end()) {
        return existsExternCFunctionBare(iter->second);
    }

    return false;
}

/* isOverloaded does not mean isExtern - this only returns true if
 * there are multiple extern functions with the same name. */
int Context::isOverloadedFunction(const char *name)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator iter;
    std::string temp_name(name);

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return 0;
        }
        std::vector<std::string> lst;
        splitString(&temp_name, &lst, '.');

        iter = new_ctx->functions->find(lst.back());

        if (iter != new_ctx->functions->end()) {
            return isOverloadedFunctionBare(iter->second->front(),
                                            iter->second);
        }
        return 0;
    }

    std::vector<std::pair<std::string, Context*> >::reverse_iterator used_iter;
    used_iter = used_namespaces->rbegin();
    Element::Function *check_fn = NULL;

    while (used_iter != used_namespaces->rend()) {
        iter = used_iter->second->functions->find(temp_name);

        if (iter != used_iter->second->functions->end()) {
            if (!check_fn) {
                check_fn = iter->second->front();
            }
            int res = isOverloadedFunctionBare(check_fn, iter->second);
            if (res) {
                return 1;
            }
        }
        ++used_iter;
    }

    iter = functions->find(temp_name);

    if (iter != functions->end()) {
        return isOverloadedFunctionBare(iter->second->front(),
                                        iter->second);
    }

    return 0;
}

Element::Function *Context::getFunction(
    const char *name,
    std::vector<Element::Type *> *types,
    int is_macro
)
{
    /* If 'types' is null, this will return the last entry from
       the relevant list. */

    return getFunction(name, types, NULL, is_macro);
}

Element::Function *Context::getFunction(
    const char *name,
    std::vector<Element::Type *> *types,
    Element::Function **closest_fn,
    int is_macro
)
{
    /* If the string begins with 'llvm.', then it is an intrinsic.
     * These are always stored in the top-level context. If
     * nothing is found, then continue with checking as per
     * normal. */

    std::map<std::string, std::vector<Element::Function *> *>::iterator iter;
    std::string temp_name(name);

    if (strstr(name, "llvm.") == name) {
        iter = functions->find(temp_name);
        if (iter != functions->end()) {
            return getFunctionBare(iter->second, types,
                                   closest_fn, is_macro);
        }
    }

    /* If the string matches '.', then it is qualified - only
     * accepting fully qualified functions for the moment. */

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return NULL;
        }
        std::vector<std::string> lst;
        splitString(&temp_name, &lst, '.');

        iter = new_ctx->functions->find(lst.back());

        if (iter != new_ctx->functions->end()) {
            return getFunctionBare(iter->second, types,
                                   closest_fn, is_macro);
        }

        return NULL;
    }

    std::vector<std::pair<std::string, Context*> >::reverse_iterator used_iter;
    used_iter = used_namespaces->rbegin();

    while (used_iter != used_namespaces->rend()) {
        iter = used_iter->second->functions->find(temp_name);

        if (iter != used_iter->second->functions->end()) {
            Element::Function *temp =
                getFunctionBare(iter->second, types,
                                closest_fn, is_macro);
            if (temp) {
                return temp;
            }
        }
        ++used_iter;
    }

    iter = functions->find(temp_name);

    if (iter != functions->end()) {
        return getFunctionBare(iter->second, types,
                               closest_fn, is_macro);
    }

    return NULL;
}

Element::Variable *Context::getVariable(const char *name)
{
    std::map<std::string, Element::Variable *>::iterator iter;
    std::string temp_name(name);

    /* If the string matches '.', then it is qualified - only
     * accepting fully qualified variables for the moment. */

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return NULL;
        }
        std::vector<std::string> lst;
        splitString(&temp_name, &lst, '.');

        iter = new_ctx->variables->find(lst.back());

        if (iter != new_ctx->variables->end()) {
            return iter->second;
        }

        return NULL;
    }

    std::vector<std::pair<std::string, Context*> >::reverse_iterator used_iter;
    used_iter = used_namespaces->rbegin();

    while (used_iter != used_namespaces->rend()) {
        iter = used_iter->second->variables->find(temp_name);

        if (iter != used_iter->second->variables->end()) {
            return iter->second;
        }
        ++used_iter;
    }

    iter = variables->find(temp_name);

    if (iter != variables->end()) {
        return iter->second;
    }

    return NULL;
}

Element::Label *Context::getLabel(const char *name)
{
    std::map<std::string, Element::Label *>::iterator iter;
    std::string temp_name(name);

    iter = labels->find(temp_name);

    if (iter == labels->end()) {
        return NULL;
    } else {
        return iter->second;
    }
}

Element::Struct *Context::getStruct(const char *name)
{
    std::string temp_name(name);

    /* If the string matches '.', then it is qualified - only
     * accepting fully qualified structs for the moment. */

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return NULL;
        }
        const char *lastpart = strrchr(name, '.');
        ++lastpart;

        Element::Struct *str = new_ctx->getStruct(lastpart);

        if (str) {
            return str;
        }

        return NULL;
    }

    std::vector<std::pair<std::string, Context*> >::reverse_iterator used_iter;
    used_iter = used_namespaces->rbegin();

    while (used_iter != used_namespaces->rend()) {
        Element::Struct *str =
            used_iter->second->getStruct(name);

        if (str) {
            return str;
        }
        ++used_iter;
    }

    std::map<std::string, Element::Struct*>::iterator str_iter;
    str_iter = structs->find(temp_name);

    if (str_iter != structs->end()) {
        return str_iter->second;
    }

    return NULL;
}

Element::Enum *Context::getEnum(const char *name)
{
    std::string temp_name(name);

    /* If the string matches '.', then it is qualified - only
     * accepting fully qualified enums for the moment. */

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return NULL;
        }
        std::vector<std::string> lst;
        splitString(&temp_name, &lst, '.');

        Element::Enum *str = new_ctx->getEnum(lst.back().c_str());

        if (str) {
            return str;
        }

        return NULL;
    }

    std::vector<std::pair<std::string, Context*> >::reverse_iterator used_iter;
    used_iter = used_namespaces->rbegin();

    while (used_iter != used_namespaces->rend()) {
        Element::Enum *str =
            used_iter->second->getEnum(name);

        if (str) {
            return str;
        }
        ++used_iter;
    }

    std::map<std::string, Element::Enum*>::iterator str_iter;
    str_iter = enums->find(temp_name);

    if (str_iter != enums->end()) {
        return str_iter->second;
    }

    return NULL;
}

void Context::setNamespacesForStruct(const char *name,
                                     std::vector<std::string> *namespaces)
{
    namespaces->clear();
    std::string temp_name(name);

    /* If the string matches '.', then it is qualified - only
     * accepting full qualifications for the moment. */

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return;
        }
        splitString(&temp_name, namespaces, '.');
        namespaces->pop_back();
        return;
    }

    std::vector<std::pair<std::string, Context*> >::reverse_iterator used_iter;
    used_iter = used_namespaces->rbegin();

    while (used_iter != used_namespaces->rend()) {
        Element::Struct *str =
            used_iter->second->getStruct(name);

        if (str) {
            std::vector<std::string>::iterator my_iter
            = used_iter->second->current_namespaces->begin();
            while (my_iter != used_iter->second->current_namespaces->end()) {
                namespaces->push_back((*my_iter));
                ++my_iter;
            }

            return;
        }
        ++used_iter;
    }

    return;
}

void Context::setNamespacesForEnum(const char *name,
                                   std::vector<std::string> *namespaces)
{
    namespaces->clear();
    std::string temp_name(name);

    /* If the string matches '.', then it is qualified - only
     * accepting full qualifications for the moment. */

    if (strchr(name, '.')) {
        Context *new_ctx;
        if (!getContextFromNamespace(name, &new_ctx, 1)) {
            return;
        }
        splitString(&temp_name, namespaces, '.');
        namespaces->pop_back();
        return;
    }

    std::vector<std::pair<std::string, Context*> >::reverse_iterator used_iter;
    used_iter = used_namespaces->rbegin();

    while (used_iter != used_namespaces->rend()) {
        Element::Enum *str =
            used_iter->second->getEnum(name);

        if (str) {
            std::vector<std::string>::iterator my_iter
            = used_iter->second->current_namespaces->begin();
            while (my_iter != used_iter->second->current_namespaces->end()) {
                namespaces->push_back((*my_iter));
                ++my_iter;
            }

            return;
        }
        ++used_iter;
    }

    return;
}

void Context::setNamespaces(std::vector<std::string> *namespaces)
{
    namespaces->clear();

    std::vector<std::pair<std::string, Context*> >::iterator iter =
        active_namespaces->begin();

    while (iter != active_namespaces->end()) {
        namespaces->push_back(iter->first);
        ++iter;
    }

    return;
}

Element::Struct *Context::getStructWithNamespaces(
    const char *name,
    std::vector<std::string> *namespaces
)
{
    // Joining together and using getStruct, could be made quicker.

    if (name == NULL) {
        return NULL;
    }

    if (namespaces == NULL) {
        return getStruct(name);
    }

    std::string temp_name;
    std::vector<std::string>::iterator iter = namespaces->begin();
    while (iter != namespaces->end()) {
        temp_name.append(*iter);
        temp_name.append(".");
        ++iter;
    }
    temp_name.append(name);

    return getStruct(temp_name.c_str());
}

int Context::getFunctionNames(
    std::set<std::string> *names
)
{
    Context &cc = *this;
    for (std::map<std::string, std::vector<Element::Function *> *>::iterator
            b = cc.functions->begin(),
            e = cc.functions->end();
            b != e;
            ++b) {
        names->insert(b->first);
    }
    for (std::map<std::string, Context *>::iterator
            b = namespace_to_context->begin(),
            e = namespace_to_context->end();
            b != e;
            ++b) {
        b->second->getFunctionNames(names);
    }

    return 1;
}

int Context::getFunctionNamesInCurrentScope(
    std::set<std::string> *names
)
{
    Context& cc = (current_context == NULL) ? *this : *current_context;
    for (std::map<std::string, std::vector<Element::Function *> *>::iterator
            b = cc.functions->begin(),
            e = cc.functions->end();
            b != e;
            ++b) {
        names->insert(b->first);
    }
    return 1;
}

int Context::getVariablesInCurrentScope(
    std::vector<Element::Variable *> *vars
)
{
    Context& cc = (current_context == NULL) ? *this : *current_context;
    for (std::map<std::string, Element::Variable *>::reverse_iterator
            b = cc.variables->rbegin(),
            e = cc.variables->rend();
            b != e;
            ++b) {
        vars->push_back(b->second);
    }
    return 1;
}

void Context::clearLabels(void)
{
    labels->clear();
}

void Context::toString(std::string *str)
{
    std::map<std::string, Element::Variable *>::iterator iter;

    for (iter = variables->begin(); iter != variables->end(); ++iter) {
        str->append((iter)->second->name->c_str());
        str->append(" ");
        (iter)->second->type->toStringProper(str);
        str->append("\n");
    }

    return;
}

int isExternLinkage(int linkage)
{
    return ((linkage == Linkage::Extern) || (linkage == Linkage::Extern_C));
}

int isExternStructLinkage(int linkage)
{
    return ((linkage == StructLinkage::Extern) ||
            (linkage == StructLinkage::Opaque));
}

int isExternEnumLinkage(int linkage)
{
    return (linkage == EnumLinkage::Extern);
}

/* Merges the argument context into this context. All externally
 * visible 'things' from the second context will be added to the
 * first context. */
int Context::merge(Context *other)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator
    fn_iter = other->functions->begin();

    std::vector<Element::Function *>::iterator ind_fn_end;
    std::vector<Element::Function *>::iterator ind_fn_iter;

    while (fn_iter != other->functions->end()) {
        int ii;
        int len = fn_iter->second->size();

        for (ii = 0; ii < len; ++ii) {
            Element::Function *temp = fn_iter->second->at(ii);
            if (isExternLinkage(temp->return_type->linkage)) {
                if (!addFunction(fn_iter->first.c_str(),
                                 temp,
                                 NULL)) {
                    fprintf(stderr,
                            "Unable to merge function (%s).\n",
                            fn_iter->first.c_str());
                    abort();
                }
            }
        }
        ++fn_iter;
    }

    std::map<std::string, Element::Enum *>::iterator enum_iter =
        other->enums->begin();

    while (enum_iter != other->enums->end()) {
        if (isExternEnumLinkage(enum_iter->second->linkage)) {
            addEnum(enum_iter->first.c_str(),
                    enum_iter->second);
        }
        ++enum_iter;
    }

    std::map<std::string, Element::Variable *>::iterator var_iter
    = other->variables->begin();

    while (var_iter != other->variables->end()) {
        if (isExternLinkage(var_iter->second->type->linkage)) {
            if (!getVariable(var_iter->first.c_str())) {
                if (!addVariable(var_iter->first.c_str(),
                                 var_iter->second)) {
                    fprintf(stderr,
                            "Unable to merge variable (%s).\n",
                            var_iter->first.c_str());
                    abort();
                }
            }
        }
        ++var_iter;
    }

    std::map<std::string, Element::Struct *>::iterator str_iter
    = other->structs->begin();

    while (str_iter != other->structs->end()) {
        if (!getStruct(str_iter->first.c_str())) {
            if (isExternStructLinkage(str_iter->second->linkage)) {
                if (!addStruct(str_iter->first.c_str(), str_iter->second)) {
                    fprintf(stderr,
                            "Unable to merge struct (%s).\n",
                            str_iter->first.c_str());
                    abort();
                }
            }
        }
        ++str_iter;
    }

    std::map<std::string, Context*>::iterator this_ns_iter, other_ns_iter;
    other_ns_iter = other->namespace_to_context->begin();

    while (other_ns_iter != other->namespace_to_context->end()) {
        if (strstr(other_ns_iter->first.c_str(), "anon")
                != other_ns_iter->first.c_str()) {
            this_ns_iter = namespace_to_context->find(other_ns_iter->first);
            if (this_ns_iter != namespace_to_context->end()) {
                this_ns_iter->second->merge(other_ns_iter->second);
            } else {
                namespace_to_context->insert(
                    std::pair<std::string, Context*>(
                        other_ns_iter->first,
                        other_ns_iter->second
                    )
                );
            }
        }
        ++other_ns_iter;
    }

    return 1;
}

static int var_count = 0;

int Context::getUnusedVarname(std::string *str)
{
    char buf[256];
    sprintf(buf, "%d", var_count++);
    str->append("_dv");
    str->append(buf);
    return 1;
}

static int label_count = 0;

int Context::getUnusedLabelname(std::string *str)
{
    char buf[256];
    sprintf(buf, "%d", label_count++);
    str->append("_dl");
    str->append(buf);
    return 1;
}

int Context::removeAllMacros(void)
{
    std::map<std::string,
        std::vector<Element::Function *>* >::iterator iter
        = functions->begin();

    std::vector<Element::Function *>::iterator fiter;

    while (iter != functions->end()) {
        fiter = iter->second->begin();

        while (fiter != iter->second->end()) {
            if ((*fiter)->is_macro && (*fiter)->llvm_function) {
                llvm::Function *llvm_fnptr = (*fiter)->llvm_function;
                llvm_fnptr->eraseFromParent();
                (*fiter)->llvm_function = NULL;
                ++fiter;
                while (fiter != iter->second->end()) {
                    if ((*fiter)->llvm_function
                            == llvm_fnptr) {
                        (*fiter)->llvm_function = NULL;
                    }
                    ++fiter;
                }
                fiter = iter->second->begin();
                continue;
            }
            ++fiter;
        }
        ++iter;
    }

    return 0;
}

int Context::regetPointers(llvm::Module *mod)
{
    Context& cc = (top_context == NULL)
                  ? *this : *top_context;

    for (std::map<std::string, Element::Struct *>::iterator
            b = structs->begin(),
            e = structs->end();
            b != e;
            ++b) {
        Element::Struct *mine = b->second;
        if (mine->internal_name) {
            std::string temp;
            temp.append("struct_").append(*(mine->internal_name));

            llvm::StructType *myst =
                mod->getTypeByName(temp);
            if (!myst) {
                temp.clear();
                temp.append(*(mine->internal_name));
                myst = mod->getTypeByName(temp);
            }
            if (!myst) {
                fprintf(stderr, "Could not get type for struct "
                                "%s (tried that name as well as "
                                "struct_<that name>.\n",
                        mine->internal_name->c_str());
                abort();
            }
            mine->type =
                mod->getTypeByName(temp);
            if (!mine->type) {
                fprintf(stderr, "Could not get type for struct "
                        "%s\n",
                        mine->internal_name->c_str());
                abort();
            }
        }
    }

    std::map<std::string,
        Element::Variable *>::iterator viter =
            variables->begin();

    while (viter != variables->end()) {
        if ((*viter).second->value) {
            /* internal_name is only set when the variable's value
             * pointer needs to be updated after module linkage.
             * */
            if ((*viter).second->internal_name
                    && (*viter).second->internal_name->size() > 0) {
                (*viter).second->value =
                    llvm::cast<llvm::Value>(
                        mod->getOrInsertGlobal(
                            (*viter).second->internal_name->c_str(),
                            llvm::cast<llvm::PointerType>(
                                (*viter).second->value->getType()
                            )->getElementType()
                        )
                    );
                if (!(*viter).second->value) {
                    fprintf(stderr, "Internal error: unable to re-get "
                            "global variable ('%s', '%s').\n",
                            (*viter).first.c_str(),
                            (*viter).second->internal_name
                            ->c_str());
                    abort();
                }
            }
        }
        ++viter;
    }

    std::map<std::string,
        std::vector<Element::Function *>* >::iterator fn_iter =
            functions->begin();

    while (fn_iter != functions->end()) {
        std::vector<Element::Function *>::iterator single_fn_iter =
            fn_iter->second->begin();

        while (single_fn_iter != fn_iter->second->end()) {
            if (strcmp(fn_iter->first.c_str(), "va-start")
                    && strcmp(fn_iter->first.c_str(), "va-end")) {

                if ((*single_fn_iter)->llvm_function) {
                    /* Try to get the function first. */
                    (*single_fn_iter)->llvm_function =
                        mod->getFunction(
                            (*single_fn_iter)->internal_name->c_str()
                        );
                    if (!(*single_fn_iter)->llvm_function) {
                        /* Convert to llvm args. */
                        std::vector<llvm::Type*> fn_args;

                        std::vector<Element::Variable *>::iterator iter;
                        std::vector<Element::Variable *>
                        *fn_args_internal = (*single_fn_iter)->parameter_types;
                        llvm::Type *temp;

                        iter = fn_args_internal->begin();
                        while (iter != fn_args_internal->end()) {
                            if ((*iter)->type->base_type == Type::VarArgs) {
                                break;
                            }
                            temp =
                                cc.daleToLLVMType((*iter)->type, NULL, false,
                                                  false);
                            if (!temp) {
                                erep->flush();
                                fprintf(stderr, "failed conversion 1\n");
                                abort();
                            }
                            fn_args.push_back(temp);
                            ++iter;
                        }

                        llvm::Type *llvm_r_type =
                            cc.daleToLLVMType((*single_fn_iter)->return_type,
                                              NULL, true, false);
                        if (!llvm_r_type) {
                            erep->flush();
                            fprintf(stderr, "failed conversion 2\n");
                            abort();
                        }
                        bool varargs = (*single_fn_iter)->isVarArgs();

                        llvm::FunctionType *ft =
                            llvm::FunctionType::get(
                                llvm_r_type,
                                llvm::ArrayRef<llvm::Type*>(fn_args),
                                varargs
                            );

                        (*single_fn_iter)->llvm_function =
                            llvm::dyn_cast<llvm::Function>(
                                mod->getOrInsertFunction(
                                    (*single_fn_iter)->internal_name->c_str(),
                                    ft
                                )
                            );
                        if (!(*single_fn_iter)->llvm_function) {
                            fprintf(stderr, "Internal error: unable to re-get "
                                    "function ('%s').\n",
                                    fn_iter->first.c_str());
                            abort();
                        }
                    }
                }
            }
            ++single_fn_iter;
        }
        ++fn_iter;
    }

    std::map<std::string, Context *>::iterator niter =
        namespace_to_context->begin();

    while (niter != namespace_to_context->end()) {
        niter->second->erep = erep;
        niter->second->top_context =
            top_context ? top_context : this;
        niter->second->regetPointers(mod);
        ++niter;
    }

    return 1;
}

llvm::Type *Context::daleToLLVMType(Element::Type *type,
                                    Node *n,
                                    bool
                                    allow_non_first_class,
                                    bool
                                    externally_defined)
{
    int error_count =
        erep->getErrorTypeCount(ErrorType::Error);

    /* If type designates an opaque struct, then disallow
     * instantiation. */
    if (!allow_non_first_class) {
        if (type->struct_name) {
            Element::Struct *structp =
                getStructWithNamespaces(type->struct_name->c_str(),
                                        type->namespaces);
            if (structp) {
                if (((structp->linkage == StructLinkage::Opaque)
                        || (structp->element_types->size() == 0))
                        && !externally_defined) {
                    Node *mine;
                    if (n) {
                        mine = n;
                    }
                    else {
                        mine = new Node();
                    }
                    Error *e = new Error(
                        ErrorInst::Generator::CannotInstantiateOpaqueStruct,
                        mine
                    );
                    erep->addError(e);
                    return NULL;
                }
            }
        }
    }

    llvm::Type *llvm_type = daleToLLVMTypeInternal(type, n);

    if (!llvm_type) {
        return llvm_type;
    }

    if (!allow_non_first_class && !llvm_type->isFirstClassType()) {
        if (type->struct_name && externally_defined) {
            /* Even when allow_non_first_class is specified, if
             * this is an externally defined struct, then return
             * the type (see e.g. _IO_2_1_stdin_). */
            return llvm_type;
        }
        if (error_count != erep->getErrorTypeCount(ErrorType::Error)) {
            return llvm_type;
        }
        std::string temp;
        type->toStringProper(&temp);
        Node *mine;
        if (n) {
            mine = n;
        }
        else {
            mine = new Node();
        }
        Error *e = new Error(
            ErrorInst::Generator::TypeIsNotFirstClass,
            mine,
            temp.c_str()
        );
        erep->addError(e);
        return NULL;
    }

    return llvm_type;
}

llvm::Type *Context::daleToLLVMTypeInternal(Element::Type *type,
        Node *n)
{
    llvm::LLVMContext &lc = llvm::getGlobalContext();

    if (type->bitfield_size) {
        return llvm::IntegerType::get(lc, type->bitfield_size);
    }

    if (type->is_function) {
        std::vector<llvm::Type*> llvm_fn_params;
        int is_varargs = 0;

        std::vector<Element::Type*>::iterator iter;
        iter = type->parameter_types->begin();

        while (iter != type->parameter_types->end()) {
            if ((*iter)->base_type == Type::VarArgs) {
                is_varargs = 1;
            } else {
                llvm::Type *t = daleToLLVMTypeInternal((*iter), n);
                if (!t) {
                    return NULL;
                }
                llvm_fn_params.push_back(t);
            }
            ++iter;
        }

        llvm::FunctionType *fntype = 
            llvm::FunctionType::get(
                daleToLLVMTypeInternal(type->return_type, n),
                llvm_fn_params,
                is_varargs
            );
        return fntype;
    }

    if (type->is_array) {
        llvm::Type *new_type =
            llvm::ArrayType::get(
                daleToLLVMTypeInternal(type->array_type, n),
                type->array_size
            );
        return new_type;
    }

    if (type->points_to != NULL) {
        llvm::Type *temp_type =
            daleToLLVMTypeInternal(type->points_to, n);

        if (!temp_type) {
            return NULL;
        }
        /* If this is a pointer to void, then return a _vp struct
         * instead. */
        if (temp_type->isVoidTy()) {
            Element::Struct *structp =
                getStructWithNamespaces("_vp", NULL);

            if (!structp) {
                fprintf(stderr, "Internal error: no _vp struct.\n");
                abort();
            }
            if (!structp->type) {
                fprintf(stderr, "Internal error: found struct, "
                        "but it doesn't have a type. (%s)\n",
                        type->struct_name->c_str());
                abort();
            }
            return llvm::PointerType::getUnqual(structp->type);
        }
        return llvm::PointerType::getUnqual(temp_type);
    }

    int base_type = type->base_type;
    llvm::Type *lbt = NULL;

    switch (base_type) {
    case Type::Int:
        lbt = nt->getNativeIntType();
        break;
    case Type::UInt:
        lbt = nt->getNativeUIntType();
        break;
    case Type::Char:
        lbt = nt->getNativeCharType();
        break;
    case Type::Void:
        lbt = llvm::Type::getVoidTy(lc);
        break;
    case Type::Bool:
        lbt = llvm::Type::getInt1Ty(lc);
        break;
    case Type::Float:
        lbt = llvm::Type::getFloatTy(lc);
        break;
    case Type::Double:
        lbt = llvm::Type::getDoubleTy(lc);
        break;
    case Type::LongDouble:
        lbt = nt->getNativeLongDoubleType();
        break;
    case Type::Int8:
        lbt = llvm::Type::getInt8Ty(lc);
        break;
    case Type::UInt8:
        lbt = llvm::Type::getInt8Ty(lc);
        break;
    case Type::Int16:
        lbt = llvm::Type::getInt16Ty(lc);
        break;
    case Type::UInt16:
        lbt = llvm::Type::getInt16Ty(lc);
        break;
    case Type::Int32:
        lbt = llvm::Type::getInt32Ty(lc);
        break;
    case Type::UInt32:
        lbt = llvm::Type::getInt32Ty(lc);
        break;
    case Type::Int64:
        lbt = llvm::Type::getInt64Ty(lc);
        break;
    case Type::UInt64:
        lbt = llvm::Type::getInt64Ty(lc);
        break;
    case Type::IntPtr:
        lbt = nt->getNativeIntptrType();
        break;
    case Type::Size:
        lbt = nt->getNativeSizeType();
        break;
    case Type::PtrDiff:
        lbt = nt->getNativePtrDiffType();
        break;
    case Type::Int128:
        lbt = llvm::IntegerType::get(lc, 128);
        break;
    case Type::UInt128:
        lbt = llvm::IntegerType::get(lc, 128);
        break;
    }

    if (lbt) {
        return lbt;
    }

    /* Check for a struct. */
    if (type->struct_name != NULL) {
        Element::Struct *structp =
            getStructWithNamespaces(type->struct_name->c_str(),
                                    type->namespaces);

        if (structp) {
            if (!structp->type) {
                fprintf(stderr, "Internal error: found struct, "
                        "but it doesn't have a type. (%s)\n",
                        type->struct_name->c_str());
                abort();
            }
            return structp->type;
        }
    }

    std::string temp;
    type->toStringProper(&temp);
    Node *mine;
    if (n) {
        mine = n;
    }
    else {
        mine = new Node();
    }
    Error *e = new Error(
        ErrorInst::Generator::UnableToConvertTypeToLLVMType,
        mine,
        temp.c_str()
    );
    erep->addError(e);

    return NULL;
}
}
