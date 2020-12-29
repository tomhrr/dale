#include "Context.h"

#include <algorithm>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../Utils/Utils.h"
#include "../llvmUtils/llvmUtils.h"
#include "../llvm_LinkAll.h"

using namespace dale::ErrorInst;

namespace dale {
Context::Context() {
    this->nt = NULL;
    this->er = NULL;
    this->tr = NULL;

    /* For serialisation use only. Native types and an error reporter
     * must be set post-deserialisation. */
}

Context::Context(ErrorReporter *er, NativeTypes *nt, TypeRegister *tr) {
    this->nt = nt;
    this->er = er;
    this->tr = tr;

    Namespace *root = new Namespace(er, tr, ".", NULL, 0);
    namespaces = new NSNode();
    namespaces->ns = root;

    active_ns_nodes.push_back(namespaces);
    used_ns_nodes.push_back(namespaces);

    this->retrieval_logging = 0;
}

void Context::deleteNamespaces(NSNode *node) {
    for (std::map<std::string, NSNode *>::iterator
             b = node->children.begin(),
             e = node->children.end();
         b != e; ++b) {
        deleteNamespaces(b->second);
    }
    delete node->ns;
    delete node;
}

Context::~Context() { deleteNamespaces(namespaces); }

Namespace *Context::ns() { return active_ns_nodes.back()->ns; }

bool Context::activateNamespace(const char *name) {
    assert(active_ns_nodes.size() && "no active namespace nodes");

    NSNode *current_nsnode = active_ns_nodes.back();
    std::map<std::string, NSNode *> *children =
        &current_nsnode->children;

    std::map<std::string, NSNode *>::iterator b =
        children->find(std::string(name));

    if (b != children->end()) {
        active_ns_nodes.push_back(b->second);
        used_ns_nodes.push_back(b->second);
        return true;
    }

    Namespace *new_ns =
        new Namespace(er, tr, name, current_nsnode->ns,
                      (current_nsnode->ns->lv_index + 1));
    NSNode *new_namespaces = new NSNode();
    new_namespaces->ns = new_ns;
    current_nsnode->children.insert(std::pair<std::string, NSNode *>(
        std::string(name), new_namespaces));

    active_ns_nodes.push_back(new_namespaces);
    used_ns_nodes.push_back(new_namespaces);

    return true;
}

bool Context::activateNamespaces(std::vector<std::string> *names) {
    for (std::vector<std::string>::iterator b = names->begin(),
                                            e = names->end();
         b != e; ++b) {
        bool res = activateNamespace(b->c_str());
        if (!res) {
            return false;
        }
    }

    return true;
}

bool Context::deactivateNamespace(const char *name) {
    if (strcmp(name, active_ns_nodes.back()->ns->name.c_str())) {
        Error *e = new Error(CannotDeactivateInactiveNamespace,
                             nullNode(), name);
        er->addError(e);
        return false;
    }

    if (strcmp(name, used_ns_nodes.back()->ns->name.c_str())) {
        Error *e = new Error(CannotDeactivateNonLastNamespace,
                             nullNode(), name);
        er->addError(e);
        return false;
    }

    used_ns_nodes.pop_back();
    active_ns_nodes.pop_back();

    return true;
}

bool Context::deactivateNamespaces(std::vector<std::string> *names) {
    for (std::vector<std::string>::reverse_iterator b = names->rbegin(),
                                                    e = names->rend();
         b != e; ++b) {
        bool res = deactivateNamespace(b->c_str());
        if (!res) {
            return false;
        }
    }

    return true;
}

static int anon_count = 0;

bool Context::activateAnonymousNamespace() {
    char buf[10];
    snprintf(buf, sizeof(buf), "anon%d", ++anon_count);
    return activateNamespace(buf);
}

bool Context::deactivateAnonymousNamespace() {
    active_ns_nodes.pop_back();
    used_ns_nodes.pop_back();

    return true;
}

void Context::activateFunctionScope(Function *fn) {
    active_function_scopes.push_back(fn);
}

void Context::deactivateFunctionScope(Function *fn) {
    assert((active_function_scopes.size() &&
            active_function_scopes.back() == fn)
           && "unmatched function scope");
    active_function_scopes.pop_back();
}

Function *Context::getCurrentFunctionScope() {
    if (!active_function_scopes.size()) return NULL;
    return active_function_scopes.back();
}

NSNode *getNSNodeFromNode(std::vector<std::string> *ns_parts,
                          NSNode *current) {
    for (std::vector<std::string>::iterator b = ns_parts->begin(),
                                            e = ns_parts->end();
         b != e; ++b) {
        std::map<std::string, NSNode *>::iterator next_current =
            current->children.find((*b));
        if (next_current == current->children.end()) {
            return NULL;
        }
        current = next_current->second;
    }

    return current;
}

NSNode *Context::getNSNode(const char *name, bool ignore_last,
                           std::vector<std::string> *ns_parts) {
    if (name[0] == '.') {
        return namespaces;
    }

    std::string ss_name(name);
    splitString(&ss_name, ns_parts, '.');

    if (ignore_last) {
        ns_parts->pop_back();
    }

    NSNode *from_active =
        getNSNodeFromNode(ns_parts, active_ns_nodes.back());
    if (from_active) {
        return from_active;
    }

    NSNode *from_root = getNSNodeFromNode(ns_parts, namespaces);
    if (from_root) {
        return from_root;
    }

    return NULL;
}

NSNode *Context::getNSNode(const char *name, bool ignore_last) {
    std::vector<std::string> ns_parts;
    return getNSNode(name, ignore_last, &ns_parts);
}

Namespace *Context::getNamespace(const char *name, bool ignore_last) {
    NSNode *nsnode = getNSNode(name, ignore_last);
    if (!nsnode) {
        return NULL;
    }
    return nsnode->ns;
}

bool Context::useNamespace(const char *name) {
    NSNode *nsnode = getNSNode(name, false);
    if (!nsnode) {
        return false;
    }

    used_ns_nodes.push_back(nsnode);
    return true;
}

bool Context::unuseNamespace() {
    used_ns_nodes.pop_back();
    return true;
}

void eraseLLVMMacros_(NSNode *node) {
    for (std::map<std::string, NSNode *>::iterator
             b = node->children.begin(),
             e = node->children.end();
         b != e; ++b) {
        eraseLLVMMacros_(b->second);
    }
    node->ns->eraseLLVMMacros();
}

void Context::eraseLLVMMacros() { eraseLLVMMacros_(namespaces); }

void eraseLLVMMacrosAndCTOFunctions_(NSNode *node) {
    for (std::map<std::string, NSNode *>::iterator
             b = node->children.begin(),
             e = node->children.end();
         b != e; ++b) {
        eraseLLVMMacrosAndCTOFunctions_(b->second);
    }
    node->ns->eraseLLVMMacrosAndCTOFunctions();
}

void Context::eraseLLVMMacrosAndCTOFunctions() {
    eraseLLVMMacrosAndCTOFunctions_(namespaces);
}

bool existsNonExternCFunctionInList(std::vector<Function *> *fn_list) {
    for (std::vector<Function *>::iterator b = fn_list->begin(),
                                           e = fn_list->end();
         b != e; ++b) {
        if ((*b)->linkage != Linkage::Extern_C) {
            return true;
        }
    }
    return false;
}

bool Context::existsNonExternCFunction(const char *name) {
    std::map<std::string, std::vector<Function *> *>::iterator iter;

    const char *fn_name;
    Namespace *name_ns;

    if (strchr(name, '.')) {
        name_ns = getNamespace(name, true);
        if (!name_ns) {
            return false;
        }
        fn_name = strrchr(name, '.') + 1;
        iter = name_ns->functions.find(fn_name);
        if (iter != name_ns->functions.end()) {
            return existsNonExternCFunctionInList(iter->second);
        }
    }

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        iter = (*rb)->ns->functions.find(name);
        if (iter != (*rb)->ns->functions.end()) {
            bool res = existsNonExternCFunctionInList(iter->second);
            if (res) {
                return true;
            }
        }
    }

    return false;
}

bool existsExternCFunctionInList(std::vector<Function *> *fn_list) {
    for (std::vector<Function *>::iterator b = fn_list->begin(),
                                           e = fn_list->end();
         b != e; ++b) {
        if ((*b)->linkage == Linkage::Extern_C) {
            return true;
        }
    }
    return false;
}

bool Context::existsExternCFunction(const char *name) {
    std::map<std::string, std::vector<Function *> *>::iterator iter;

    const char *fn_name;
    Namespace *name_ns;

    if (strchr(name, '.')) {
        name_ns = getNamespace(name, true);
        if (!name_ns) {
            return false;
        }
        fn_name = strrchr(name, '.') + 1;
        iter = name_ns->functions.find(fn_name);
        if (iter != name_ns->functions.end()) {
            return existsExternCFunctionInList(iter->second);
        }
    }

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        iter = (*rb)->ns->functions.find(name);
        if (iter != (*rb)->ns->functions.end()) {
            bool res = existsExternCFunctionInList(iter->second);
            if (res) {
                return true;
            }
        }
    }

    return false;
}

bool isOverloadedFunctionInList(Function *fn,
                                std::vector<Function *> *fn_list) {
    for (std::vector<Function *>::iterator b = fn_list->begin(),
                                           e = fn_list->end();
         b != e; ++b) {
        if (!(*b)->isEqualTo(fn)) {
            return true;
        }
    }
    return false;
}

bool Context::isOverloadedFunction(const char *name) {
    std::map<std::string, std::vector<Function *> *>::iterator iter;

    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return false;
        }
        const char *fn_name = strrchr(name, '.') + 1;

        iter = ns->functions.find(fn_name);

        if (iter != ns->functions.end()) {
            return isOverloadedFunctionInList(iter->second->front(),
                                              iter->second);
        }

        return false;
    }

    Function *check_fn = NULL;

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        iter = (*rb)->ns->functions.find(name);
        if (iter != (*rb)->ns->functions.end()) {
            if (!check_fn) {
                check_fn = iter->second->front();
            }
            if (isOverloadedFunctionInList(check_fn, iter->second)) {
                return true;
            }
        }
    }

    return false;
}

Function *getFunction_(Namespace *ns, const char *name,
                       std::vector<Type *> *types,
                       Function **closest_fn, bool is_macro,
                       std::vector<bool> *lvalues,
                       std::vector<Type *> *array_types) {
    Function *fn = ns->getFunction(name, types, closest_fn, is_macro,
                                   false, lvalues, array_types);
    if (fn) {
        return fn;
    }
    return ns->getFunction(name, types, closest_fn, is_macro, true,
                           lvalues, array_types);
}

Function *Context::getFunction(const char *name,
                               std::vector<Type *> *types,
                               Function **closest_fn, bool is_macro,
                               std::vector<bool> *lvalues,
                               std::vector<Type *> *array_types) {
    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return NULL;
        }
        const char *fn_name = strrchr(name, '.') + 1;
        Function *fn = getFunction_(ns, fn_name, types, closest_fn,
                                    is_macro, lvalues, array_types);
        if (fn && retrieval_logging) {
            retrieved_fn.push_back(fn);
        }
        return fn;
    }

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        Function *fn = getFunction_((*rb)->ns, name, types, closest_fn,
                                    is_macro, lvalues, array_types);
        if (fn) {
            if (retrieval_logging) {
                retrieved_fn.push_back(fn);
            }
            return fn;
        }
    }

    return NULL;
}

bool Context::addVariable(const char* name, Variable *var) {
    var->fn = getCurrentFunctionScope();
    return ns()->addVariable(name, var);
}

Variable* Context::getVariableInner(const char *name) {
    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return NULL;
        }
        const char *var_name = strrchr(name, '.') + 1;
        Variable *var = ns->getVariable(var_name);
        if (var && retrieval_logging) {
            retrieved_var.push_back(var);
        }
        return var;
    }

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        Variable *var = (*rb)->ns->getVariable(name);
        if (var) {
            if (retrieval_logging) {
                retrieved_var.push_back(var);
            }
            return var;
        }
    }

    return NULL;
}

Variable *Context::getVariable(const char *name) {
    Variable *ret = getVariableInner(name);
    if (ret && ret->fn && ret->fn != getCurrentFunctionScope()) {
        // the variable can't be referenced as a value
        return NULL;
    }
    return ret;
}

Struct *Context::getStruct(const char *name) {
    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return NULL;
        }
        const char *st_name = strrchr(name, '.') + 1;
        return ns->getStruct(st_name);
    }

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        Struct *st = (*rb)->ns->getStruct(name);
        if (st) {
            return st;
        }
    }

    return NULL;
}

Struct *Context::getStruct(const char *name,
                           std::vector<std::string> *namespaces) {
    if (name == NULL) {
        return NULL;
    }

    if ((namespaces == NULL) || (namespaces->size() == 0)) {
        return getStruct(name);
    }

    std::string full_name;
    for (std::vector<std::string>::iterator b = namespaces->begin(),
                                            e = namespaces->end();
         b != e; ++b) {
        full_name.append((*b)).append(".");
    }
    full_name.append(std::string(name));

    return getStruct(full_name.c_str());
}

Struct *Context::getStruct(Type *type) {
    return getStruct(type->struct_name.c_str(), &(type->namespaces));
}

bool Context::setNamespacesForStruct(
    const char *name, std::vector<std::string> *namespaces) {
    std::string ss_name(name);

    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return false;
        }
        const char *st_name = strrchr(name, '.') + 1;
        if (!ns->getStruct(st_name)) {
            return false;
        }
        splitString(&ss_name, namespaces, '.');
        namespaces->pop_back();
        return true;
    }

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        Struct *st = (*rb)->ns->getStruct(name);
        if (st) {
            (*rb)->ns->setNamespaces(namespaces);
            return true;
        }
    }

    return false;
}

bool Context::setFullyQualifiedStructName(const char *name,
                                          std::string *fqsn) {
    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return false;
        }
        const char *st_name = strrchr(name, '.') + 1;
        if (!ns->getStruct(st_name)) {
            return false;
        }
        fqsn->append(name);
        return true;
    }

    for (std::vector<NSNode *>::reverse_iterator
             rb = used_ns_nodes.rbegin(),
             re = used_ns_nodes.rend();
         rb != re; ++rb) {
        Struct *st = (*rb)->ns->getStruct(name);
        if (st) {
            std::vector<std::string> nss;
            (*rb)->ns->setNamespaces(&nss);
            for (std::vector<std::string>::iterator b = nss.begin(),
                                                    e = nss.end();
                 b != e; ++b) {
                fqsn->append((*b));
                fqsn->append(".");
            }
            fqsn->append(name);
            return true;
        }
    }

    return false;
}

void getFunctionNames_(std::set<std::string> *names,
                       std::string *prefix, NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        getFunctionNames_(names, prefix, b->second);
    }
    nsnode->ns->getFunctionNames(names, prefix);
}

void Context::getFunctionNames(std::set<std::string> *names,
                               std::string *prefix) {
    getFunctionNames_(names, prefix, namespaces);
}

bool merge_(NSNode *nsnode_dst, NSNode *nsnode_src) {
    Namespace *ns_dst = nsnode_dst->ns;
    Namespace *ns_src = nsnode_src->ns;

    for (std::map<std::string, NSNode *>::iterator
             src_b = nsnode_src->children.begin(),
             src_e = nsnode_src->children.end();
         src_b != src_e; ++src_b) {
        std::map<std::string, NSNode *>::iterator dst_m =
            nsnode_dst->children.find(src_b->first);
        if (dst_m != nsnode_dst->children.end()) {
            merge_(dst_m->second, src_b->second);
        } else {
            src_b->second->ns->parent_namespace = ns_dst;
            nsnode_dst->children.insert(
                std::pair<std::string, NSNode *>(src_b->first,
                                                 src_b->second));
        }
    }

    ns_dst->merge(ns_src);

    return true;
}

bool Context::merge(Context *other) {
    return merge_(namespaces, other->namespaces);
}

bool regetPointers_(llvm::Module *mod, NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        regetPointers_(mod, b->second);
    }
    nsnode->ns->regetPointers(mod);
    return true;
}

bool regetFunctionPointers(llvm::Module *mod, NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        regetFunctionPointers(mod, b->second);
    }
    nsnode->ns->regetFunctionPointers(mod);
    return true;
}

bool Context::rebuildFunction(Function *fn, const char *name,
                              llvm::Module *mod) {
    std::vector<llvm::Type *> types;

    int arg_count = 0;
    for (std::vector<Variable *>::iterator vb = fn->parameters.begin(),
                                           ve = fn->parameters.end();
         vb != ve; ++vb) {
        Variable *var = (*vb);
        Type *var_type = var->type;
        if (!fn->is_macro && var_type->is_reference) {
            var_type = tr->getPointerType(var_type);
        }
        if (var_type->base_type == BaseType::VarArgs) {
            if (!(var->name.compare("rest"))) {
                var_type = tr->getPointerType(tr->type_pdnode);
            } else {
                break;
            }
        } else if (fn->is_macro && (arg_count > 0)) {
            var_type = tr->type_pdnode;
        }

        llvm::Type *llvm_type =
            toLLVMType(var_type, NULL, false, false);
        assert(llvm_type && "failed type conversion");
        types.push_back(llvm_type);
        arg_count++;
    }

    llvm::Type *llvm_return_type;
    Type *ret_type = fn->return_type;
    if (ret_type->is_retval) {
        types.push_back(
            toLLVMType(tr->getPointerType(ret_type), NULL, true));
        llvm_return_type =
            toLLVMType(tr->getBasicType(BaseType::Void), NULL, true);
    } else {
        llvm_return_type = toLLVMType(ret_type, NULL, true, false);
    }
    assert(llvm_return_type && "failed type conversion");

    bool varargs = fn->isVarArgs() && !fn->is_macro;

    llvm::FunctionType *ft = llvm::FunctionType::get(
        llvm_return_type, llvm::ArrayRef<llvm::Type *>(types), varargs);

#if D_LLVM_VERSION_ORD <= 80
    fn->llvm_function = llvm::dyn_cast<llvm::Function>(
        mod->getOrInsertFunction(fn->symbol.c_str(), ft));
#else
    llvm::FunctionCallee fc =
        mod->getOrInsertFunction(fn->symbol.c_str(), ft);
    fn->llvm_function = llvm::cast<llvm::Function>(fc.getCallee());
#endif

    assert(fn->llvm_function && "unable to re-get function");

    return true;
}

bool Context::rebuildFunctions(llvm::Module *mod, NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        rebuildFunctions(mod, b->second);
    }

    Namespace *ns = nsnode->ns;

    std::map<std::string, std::vector<Function *> *>::iterator b, e;

    for (b = ns->functions.begin(), e = ns->functions.end(); b != e;
         ++b) {
        for (std::vector<Function *>::iterator fb = b->second->begin(),
                                               fe = b->second->end();
             fb != fe; ++fb) {
            Function *fn = (*fb);
            fn->llvm_function = mod->getFunction(fn->symbol.c_str());
            if (fn->llvm_function) {
                continue;
            }
            rebuildFunction(fn, b->first.c_str(), mod);
        }
    }

    return true;
}

bool Context::rebuildVariable(Variable *var, const char *name,
                              llvm::Module *mod) {
    /* symbol is only set when the variable's value
     * pointer needs to be updated after module linkage. */

    const char *variable_name =
        (var->symbol.size() > 0) ? var->symbol.c_str() : name;

    var->value = llvm::cast<llvm::Value>(mod->getOrInsertGlobal(
        variable_name,
        llvm::cast<llvm::PointerType>(
            toLLVMType(tr->getPointerType(var->type), NULL, true, true))
            ->getElementType()));
    assert(var->value && "unable to re-get global variable");

    return true;
}

bool Context::rebuildVariables(llvm::Module *mod, NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        rebuildVariables(mod, b->second);
    }

    Namespace *ns = nsnode->ns;

    for (std::map<std::string, Variable *>::iterator
             b = ns->variables.begin(),
             e = ns->variables.end();
         b != e; ++b) {
        Variable *var = b->second;
        rebuildVariable(var, b->first.c_str(), mod);
    }

    return true;
}

llvm::GlobalValue::LinkageTypes Context::toLLVMLinkage(int linkage) {
    llvm::GlobalValue::LinkageTypes lt;
    switch (linkage) {
        case Linkage::Auto:
        case Linkage::Intern:
            lt = llvm::GlobalValue::InternalLinkage;
            break;
        case Linkage::Extern_Weak:
            lt = llvm::GlobalValue::WeakODRLinkage;
            break;
        case Linkage::Extern:
        case Linkage::Extern_C:
        default:
            lt = llvm::GlobalValue::ExternalLinkage;
            break;
    }

    return lt;
}

bool Context::regetPointers(llvm::Module *mod) {
    regetPointers_(mod, namespaces);
    rebuildFunctions(mod, namespaces);
    return true;
}

bool Context::regetPointersForNewModule(llvm::Module *mod) {
    regetPointers_(mod, namespaces);
    rebuildVariables(mod, namespaces);
    rebuildFunctions(mod, namespaces);
    regetFunctionPointers(mod, namespaces);
    return true;
}

llvm::Type *Context::toLLVMTypeFunction(Type *type, Node *n,
                                        bool refs_to_pointers) {
    std::vector<llvm::Type *> llvm_fn_params;
    bool is_varargs = 0;

    std::vector<Type *>::iterator iter;
    iter = type->parameter_types.begin();

    while (iter != type->parameter_types.end()) {
        if ((*iter)->base_type == BaseType::VarArgs) {
            is_varargs = true;
        } else {
            /* Parameter references should always be converted to
             * pointers. */
            llvm::Type *t = toLLVMType((*iter), n, true, false, true);
            if (!t) {
                return NULL;
            }
            llvm_fn_params.push_back(t);
        }
        ++iter;
    }

    Type *r_type = type->return_type;
    llvm::Type *llvm_r_type = NULL;

    if (r_type->is_retval) {
        llvm_fn_params.push_back(
            toLLVMType(tr->getPointerType(r_type), NULL, true));
        llvm_r_type =
            toLLVMType(tr->getBasicType(BaseType::Void), NULL, true);
    } else {
        llvm_r_type = toLLVMType(r_type, NULL);
    }

    llvm::FunctionType *fntype = llvm::FunctionType::get(
        llvm_r_type, llvm_fn_params, is_varargs);
    return fntype;
}

llvm::Type *Context::toLLVMTypeArray(Type *type, Node *n) {
    llvm::Type *new_type = llvm::ArrayType::get(
        toLLVMType(type->array_type, n), type->array_size);
    return new_type;
}

llvm::Type *Context::toLLVMTypePointer(Type *type, Node *n,
                                       bool refs_to_pointers) {
    llvm::Type *llvm_type =
        toLLVMType(type, n, true, false, refs_to_pointers);

    if (!llvm_type) {
        return NULL;
    }

    /* Previously, this returned a pointer to the '_vp' struct type,
     * which was defined in the drt module as containing one (p char).
     * However, when modules are merged, the '_vp' types in each take
     * distinct names, which can cause type-checking problems for
     * anything that uses '_vp' after that point.  Using an unnamed
     * struct resolves this issue. */
    if (llvm_type->isVoidTy()) {
        std::vector<llvm::Type *> types;
        types.push_back(toLLVMType(tr->type_pchar, NULL));
        llvm::ArrayRef<llvm::Type *> llvm_types(types);
        llvm::LLVMContext &lc = *getContext();
        llvm::StructType *st = llvm::StructType::get(lc, llvm_types);
        return llvm::PointerType::getUnqual(st);
    }
    return llvm::PointerType::getUnqual(llvm_type);
}

llvm::Type *Context::toLLVMTypeBase(Type *type, Node *n) {
    llvm::LLVMContext &lc = *getContext();

    int base_type = type->base_type;
    llvm::Type *lbt = NULL;

    switch (base_type) {
        case BaseType::Int:
            lbt = nt->getNativeIntType();
            break;
        case BaseType::UInt:
            lbt = nt->getNativeUIntType();
            break;
        case BaseType::Char:
            lbt = nt->getNativeCharType();
            break;
        case BaseType::Void:
            lbt = llvm::Type::getVoidTy(lc);
            break;
        case BaseType::Bool:
            lbt = nt->getNativeCharType();
            break;
        case BaseType::Float:
            lbt = llvm::Type::getFloatTy(lc);
            break;
        case BaseType::Double:
            lbt = llvm::Type::getDoubleTy(lc);
            break;
        case BaseType::LongDouble:
            lbt = nt->getNativeLongDoubleType();
            break;
        case BaseType::Int8:
            lbt = llvm::Type::getInt8Ty(lc);
            break;
        case BaseType::UInt8:
            lbt = llvm::Type::getInt8Ty(lc);
            break;
        case BaseType::Int16:
            lbt = llvm::Type::getInt16Ty(lc);
            break;
        case BaseType::UInt16:
            lbt = llvm::Type::getInt16Ty(lc);
            break;
        case BaseType::Int32:
            lbt = llvm::Type::getInt32Ty(lc);
            break;
        case BaseType::UInt32:
            lbt = llvm::Type::getInt32Ty(lc);
            break;
        case BaseType::Int64:
            lbt = llvm::Type::getInt64Ty(lc);
            break;
        case BaseType::UInt64:
            lbt = llvm::Type::getInt64Ty(lc);
            break;
        case BaseType::IntPtr:
            lbt = nt->getNativeIntptrType();
            break;
        case BaseType::Size:
            lbt = nt->getNativeSizeType();
            break;
        case BaseType::PtrDiff:
            lbt = nt->getNativePtrDiffType();
            break;
        case BaseType::Int128:
            lbt = llvm::IntegerType::get(lc, 128);
            break;
        case BaseType::UInt128:
            lbt = llvm::IntegerType::get(lc, 128);
            break;
    }

    return lbt;
}

llvm::Type *Context::toLLVMTypeStruct(Type *type, Node *n) {
    Struct *st = getStruct(type);
    if (st) {
        assert(st->type && "found struct, but it does not have a type");
        return st->type;
    }

    return NULL;
}

llvm::Type *Context::toLLVMType_(Type *type, Node *n,
                                 bool refs_to_pointers) {
    llvm::LLVMContext &lc = *getContext();

    if (type->bitfield_size) {
        return llvm::IntegerType::get(lc, type->bitfield_size);
    }

    if (type->is_function) {
        return toLLVMTypeFunction(type, n, refs_to_pointers);
    }

    if (type->is_array) {
        return toLLVMTypeArray(type, n);
    }

    if (type->points_to != NULL) {
        return toLLVMTypePointer(type->points_to, n, refs_to_pointers);
    }

    if (refs_to_pointers && type->is_reference) {
        type->is_reference = 0;
        llvm::Type *result =
            toLLVMTypePointer(type, n, refs_to_pointers);
        type->is_reference = 1;
        return result;
    }

    llvm::Type *base_type = toLLVMTypeBase(type, n);
    if (base_type) {
        return base_type;
    }

    if (type->struct_name.size()) {
        llvm::Type *struct_type = toLLVMTypeStruct(type, n);
        if (struct_type) {
            return struct_type;
        }
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(UnableToConvertTypeToLLVMType,
                         (n ? n : nullNode()), type_str.c_str());
    er->addError(e);

    return NULL;
}

llvm::Type *Context::toLLVMType(Type *type, Node *n,
                                bool allow_non_first_class,
                                bool externally_defined,
                                bool refs_to_pointers) {
    int error_count = er->getErrorTypeCount(ErrorType::Error);

    /* If type designates an opaque struct, then disallow
     * instantiation. */
    if (!allow_non_first_class) {
        if (type->struct_name.size()) {
            Struct *st = getStruct(type);
            if (st) {
                if (((st->linkage == StructLinkage::Opaque) ||
                     (st->member_types.size() == 0)) &&
                    !externally_defined) {
                    Error *e = new Error(CannotInstantiateOpaqueStruct,
                                         (n ? n : nullNode()));
                    er->addError(e);
                    return NULL;
                }
            }
        }
    }

    llvm::Type *llvm_type = toLLVMType_(type, n, refs_to_pointers);

    if (!llvm_type) {
        return llvm_type;
    }

    if (!allow_non_first_class && !llvm_type->isFirstClassType()) {
        if (type->struct_name.size() && externally_defined) {
            /* Even when allow_non_first_class is specified, if
             * this is an externally defined struct, then return
             * the type (see e.g. _IO_2_1_stdin_). */
            return llvm_type;
        }
        if (error_count != er->getErrorTypeCount(ErrorType::Error)) {
            return llvm_type;
        }
        std::string type_str;
        type->toString(&type_str);
        Error *e = new Error(TypeIsNotFirstClass, (n ? n : nullNode()),
                             type_str.c_str());
        er->addError(e);
        return NULL;
    }

    return llvm_type;
}

bool removeUnneeded_(std::set<std::string> *forms,
                     std::set<std::string> *found_forms,
                     NSNode *nsnode) {
    nsnode->ns->removeUnneeded(forms, found_forms);

    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        std::set<std::string>::iterator fb = forms->find(b->first);
        if (fb != forms->end()) {
            found_forms->insert((*fb));
            continue;
        }

        std::set<std::string> subforms;
        for (std::set<std::string>::iterator fb = forms->begin(),
                                             fe = forms->end();
             fb != fe; ++fb) {
            if (fb->find(b->first) == 0) {
                std::string subform((*fb));
                subform.erase(0, (b->first.size() + 1));
                subforms.insert(subform);
            }
        }

        std::set<std::string> subfound_forms;

        removeUnneeded_(&subforms, &subfound_forms, b->second);

        for (std::set<std::string>::iterator
                 sfb = subfound_forms.begin(),
                 sfe = subfound_forms.end();
             sfb != sfe; ++sfb) {
            std::string fullform;
            fullform.append(b->first).append(".").append(*sfb);
            found_forms->insert(fullform);
        }
    }

    return true;
}

bool Context::removeUnneeded(std::set<std::string> *forms,
                             std::set<std::string> *found_forms) {
    return removeUnneeded_(forms, found_forms, namespaces);
}

bool eraseOnceForms_(std::set<std::string> *once_tags,
                     llvm::Module *mod, NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        eraseOnceForms_(once_tags, mod, b->second);
    }
    nsnode->ns->eraseOnceFunctions(once_tags, mod);
    nsnode->ns->eraseOnceVariables(once_tags, mod);

    return true;
}

bool Context::eraseOnceForms(std::set<std::string> *once_tags,
                             llvm::Module *mod) {
    return eraseOnceForms_(once_tags, mod, namespaces);
}

void relink_(NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        b->second->ns->parent_namespace = nsnode->ns;
        relink_(b->second);
    }
}

void Context::relink() { relink_(namespaces); }

void print_(NSNode *nsnode) {
    nsnode->ns->print();

    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        fprintf(stderr, "Namespace (>): %s\n", b->first.c_str());
        print_(b->second);
    }
}

void Context::print() { print_(namespaces); }

bool deleteAnonymousNamespaces_(NSNode *nsnode) {
    if (nsnode->ns->name.find("anon") == 0) {
        delete nsnode->ns;
        nsnode->ns = NULL;
    }

    std::map<std::string, NSNode *>::iterator b =
        nsnode->children.begin();
    while (b != nsnode->children.end()) {
        deleteAnonymousNamespaces_(b->second);
        ++b;
    }

    b = nsnode->children.begin();
    while (b != nsnode->children.end()) {
        if (b->first.find("anon") == 0) {
            nsnode->children.erase(b++);
        } else {
            ++b;
        }
    }

    return true;
}

bool Context::deleteAnonymousNamespaces() {
    return deleteAnonymousNamespaces_(namespaces);
}

bool removeDeserialised_(NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        removeDeserialised_(b->second);
    }

    nsnode->ns->removeDeserialised();

    return true;
}

bool Context::removeDeserialised() {
    return removeDeserialised_(namespaces);
}

void Context::enableRetrievalLog() { retrieval_logging++; }

void Context::disableRetrievalLog() {
    retrieval_logging--;
    if (!retrieval_logging) {
        retrieved_var.clear();
        retrieved_fn.clear();
    }
}

void Context::getRetrievedVariables(
    std::vector<Variable *> *variables) {
    std::copy(retrieved_var.begin(), retrieved_var.end(),
              std::back_inserter(*variables));
}

void Context::getRetrievedFunctions(
    std::vector<Function *> *functions) {
    std::copy(retrieved_fn.begin(), retrieved_fn.end(),
              std::back_inserter(*functions));
}
}
