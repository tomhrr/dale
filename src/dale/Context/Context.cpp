#include "Context.h"

#include "../llvm_LinkAll.h"
#include "../Utils/Utils.h"

namespace dale
{
Context::Context(void)
{
    this->nt = NULL;
    this->er = NULL;
    this->tr = NULL;

    /* For serialisation use only. Native types and an error reporter
     * must be set post-deserialisation. */
}

Context::Context(ErrorReporter *er,
                 NativeTypes *nt,
                 TypeRegister *tr)
{
    this->nt = nt;
    this->er = er;
    this->tr = tr;

    Namespace *root = new Namespace(er, tr, ".", NULL, 0);
    namespaces = new NSNode();
    namespaces->ns = root;

    active_ns_nodes.push_back(namespaces);
    used_ns_nodes.push_back(namespaces);
}

void 
Context::deleteNamespaces(NSNode *node)
{
    for (std::map<std::string, NSNode *>::iterator
            b = node->children.begin(),
            e = node->children.end();
            b != e;
            ++b) {
        deleteNamespaces(b->second);
    }
    delete node->ns;
    delete node;
}

Context::~Context(void)
{
    deleteNamespaces(namespaces);
}

Namespace *
Context::ns(void)
{
    return active_ns_nodes.back()->ns;
}

bool
Context::popUntilNamespace(Namespace *ns)
{
    for (;;) {
        if (active_ns_nodes.size() == 0) {
            return false;
        }
        if (active_ns_nodes.back()->ns == ns) {
            return true;
        }
        active_ns_nodes.pop_back();
    }
    for (;;) {
        if (used_ns_nodes.size() == 0) {
            return false;
        }
        if (used_ns_nodes.back()->ns == ns) {
            return true;
        }
        used_ns_nodes.pop_back();
    }
}

bool
Context::activateNamespace(const char *name)
{
    NSNode *current_nsnode = active_ns_nodes.back();
    std::map<std::string, NSNode *> *children = &current_nsnode->children;

    std::map<std::string, NSNode *>::iterator
        b = children->find(std::string(name));

    if (b != children->end()) {
        active_ns_nodes.push_back(b->second);
        used_ns_nodes.push_back(b->second);
        return true;
    }

    Namespace *new_ns = new Namespace(er, tr, name, 
                                      current_nsnode->ns,
                                      (current_nsnode->ns->lv_index + 1));
    NSNode *new_namespaces = new NSNode();
    new_namespaces->ns = new_ns;
    current_nsnode->children.insert(
        std::pair<std::string, NSNode *>(
            std::string(name),
            new_namespaces
        )
    );

    active_ns_nodes.push_back(new_namespaces);
    used_ns_nodes.push_back(new_namespaces);

    return true;
}

bool
Context::deactivateNamespace(const char *name)
{
    if (strcmp(name, active_ns_nodes.back()->ns->name.c_str())) {
        Error *e = new Error(
            ErrorInst::Generator::CannotDeactivateInactiveNamespace,
            nullNode(),
            name
        );
        er->addError(e);
        return false;
    }

    if (strcmp(name, used_ns_nodes.back()->ns->name.c_str())) {
        used_ns_nodes.back()->ns->print();
        Error *e = new Error(
            ErrorInst::Generator::CannotDeactivateNonLastNamespace,
            nullNode(),
            name
        );
        er->addError(e);
        return false;
    }

    used_ns_nodes.pop_back();
    active_ns_nodes.pop_back();

    return true;
}

static int anon_count = 0;

bool
Context::activateAnonymousNamespace(void)
{
    char buf[10];
    sprintf(buf, "anon%d", ++anon_count);
    return activateNamespace(buf);
}

bool
Context::deactivateAnonymousNamespace(void)
{
    active_ns_nodes.pop_back();
    used_ns_nodes.pop_back();
    
    return true;
}

NSNode *
getNSNodeFromNode(std::vector<std::string> *ns_parts,
                  NSNode *current)
{
    for (std::vector<std::string>::iterator 
            b = ns_parts->begin(),
            e = ns_parts->end();
            b != e;
            ++b) {
        std::map<std::string, NSNode *>::iterator
            next_current = current->children.find((*b));
        if (next_current == current->children.end()) {
            return NULL;
        }
        current = next_current->second;
    }

    return current;
}

NSNode *
Context::getNSNode(const char *name, 
                   bool ignore_last,
                   std::vector<std::string> *ns_parts)
{
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

    NSNode *from_root =
        getNSNodeFromNode(ns_parts, namespaces);
    if (from_root) {
        return from_root;
    }

    return NULL;
}

NSNode *
Context::getNSNode(const char *name, bool ignore_last)
{
    std::vector<std::string> ns_parts;
    return getNSNode(name, ignore_last, &ns_parts);
}

Namespace *
Context::getNamespace(const char *name, bool ignore_last)
{
    NSNode *nsnode = getNSNode(name, ignore_last);
    if (!nsnode) {
        return NULL;
    }
    return nsnode->ns;
}

bool
Context::useNamespace(const char *name)
{
    NSNode *nsnode = getNSNode(name, false);
    if (!nsnode) {
        return false;
    }

    used_ns_nodes.push_back(nsnode);
    return true;
}

bool
Context::unuseNamespace(void)
{
    used_ns_nodes.pop_back();
    return true;
}

void 
eraseLLVMMacros_(NSNode *node)
{
    for (std::map<std::string, NSNode *>::iterator
            b = node->children.begin(),
            e = node->children.end();
            b != e;
            ++b) {
        eraseLLVMMacros_(b->second);
    }
    node->ns->eraseLLVMMacros();
}

void
Context::eraseLLVMMacros(void)
{
    eraseLLVMMacros_(namespaces);
}

void 
eraseLLVMMacrosAndCTOFunctions_(NSNode *node)
{
    for (std::map<std::string, NSNode *>::iterator
            b = node->children.begin(),
            e = node->children.end();
            b != e;
            ++b) {
        eraseLLVMMacrosAndCTOFunctions_(b->second);
    }
    node->ns->eraseLLVMMacrosAndCTOFunctions();
}

void
Context::eraseLLVMMacrosAndCTOFunctions(void)
{
    eraseLLVMMacrosAndCTOFunctions_(namespaces);
}

bool
existsNonExternCFunctionInList(std::vector<Element::Function *> *fn_list)
{
    for (std::vector<Element::Function *>::iterator
            b = fn_list->begin(),
            e = fn_list->end();
            b != e;
            ++b) {
        if ((*b)->linkage != Linkage::Extern_C) {
            return true;
        }
    }
    return false;
}

bool
Context::existsNonExternCFunction(const char *name)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator 
        iter;

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
            rb != re;
            ++rb) {
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

bool
existsExternCFunctionInList(std::vector<Element::Function *> *fn_list)
{
    for (std::vector<Element::Function *>::iterator
            b = fn_list->begin(),
            e = fn_list->end();
            b != e;
            ++b) {
        if ((*b)->linkage == Linkage::Extern_C) {
            return true;
        }
    }
    return false;
}

bool
Context::existsExternCFunction(const char *name)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator 
        iter;

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
            rb != re;
            ++rb) {
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

bool
isOverloadedFunctionInList(Element::Function *fn,
                           std::vector<Element::Function *> *fn_list)
{
    for (std::vector<Element::Function *>::iterator
            b = fn_list->begin(),
            e = fn_list->end();
            b != e;
            ++b) {
        if (!(*b)->isEqualTo(fn)) {
            return true;
        }
    }
    return false;
}

bool
Context::isOverloadedFunction(const char *name)
{
    std::map<std::string, std::vector<Element::Function *> *>::iterator 
        iter;

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

    Element::Function *check_fn = NULL;

    for (std::vector<NSNode *>::reverse_iterator
            rb = used_ns_nodes.rbegin(),
            re = used_ns_nodes.rend();
            rb != re;
            ++rb) {
        iter = (*rb)->ns->functions.find(name);
        if (iter != (*rb)->ns->functions.end()) {
            if (!check_fn) {
                check_fn = iter->second->front();
            }
            if (isOverloadedFunctionInList(check_fn,
                                           iter->second)) {
                return true;
            }
        }
    }
            
    return false;
}

Element::Function *
Context::getFunction(const char *name,
                     std::vector<Element::Type *> *types,
                     Element::Function **closest_fn,
                     bool is_macro)
{
    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return NULL;
        }
        const char *fn_name = strrchr(name, '.') + 1;
        return ns->getFunction(fn_name, types, closest_fn, is_macro);
    }

    for (std::vector<NSNode *>::reverse_iterator
            rb = used_ns_nodes.rbegin(),
            re = used_ns_nodes.rend();
            rb != re;
            ++rb) {
        Element::Function *fn = 
            (*rb)->ns->getFunction(name, types, closest_fn, is_macro);
        if (fn) {
            return fn;
        }
    }

    return NULL;
}

Element::Function *
Context::getFunction(const char *name,
                     std::vector<Element::Type *> *types,
                     bool is_macro)
{
    return getFunction(name, types, NULL, is_macro);
}

Element::Variable *
Context::getVariable(const char *name)
{
    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return NULL;
        }
        const char *var_name = strrchr(name, '.') + 1;
        return ns->getVariable(var_name);
    }

    for (std::vector<NSNode *>::reverse_iterator
            rb = used_ns_nodes.rbegin(),
            re = used_ns_nodes.rend();
            rb != re;
            ++rb) {
        Element::Variable *var = (*rb)->ns->getVariable(name);
        if (var) {
            return var;
        }
    }
            
    return NULL;
}

Element::Struct *
Context::getStruct(const char *name)
{
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
            rb != re;
            ++rb) {
        Element::Struct *st = (*rb)->ns->getStruct(name);
        if (st) {
            return st;
        }
    }
 
    return NULL;
}

Element::Struct *
Context::getStruct(const char *name,
                   std::vector<std::string> *namespaces)
{
    if (name == NULL) {
        return NULL;
    }

    if ((namespaces == NULL) || (namespaces->size() == 0)) {
        return getStruct(name);
    }

    std::string full_name;
    for (std::vector<std::string>::iterator
            b = namespaces->begin(),
            e = namespaces->end();
            b != e;
            ++b) {
        full_name.append((*b))
                 .append(".");
    }
    full_name.append(std::string(name));

    return getStruct(full_name.c_str());
}

Element::Enum *
Context::getEnum(const char *name)
{
    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return NULL;
        }
        const char *en_name = strrchr(name, '.') + 1;
        return ns->getEnum(en_name);
    }

    for (std::vector<NSNode *>::reverse_iterator
            rb = used_ns_nodes.rbegin(),
            re = used_ns_nodes.rend();
            rb != re;
            ++rb) {
        Element::Enum *en = (*rb)->ns->getEnum(name);
        if (en) {
            return en;
        }
    }
            
    return NULL;
}

bool 
Context::setNamespacesForStruct(const char *name,
                                std::vector<std::string> *namespaces)
{
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
            rb != re;
            ++rb) {
        Element::Struct *st = (*rb)->ns->getStruct(name);
        if (st) {
            (*rb)->ns->setNamespaces(namespaces);
            return true;
        }
    }

    return false;
}

bool
Context::setFullyQualifiedStructName(const char *name,
                                     std::string *fqsn)
{
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
            rb != re;
            ++rb) {
        Element::Struct *st = (*rb)->ns->getStruct(name);
        if (st) {
            std::vector<std::string> nss;
            (*rb)->ns->setNamespaces(&nss);
            for (std::vector<std::string>::iterator
                    b = nss.begin(), e = nss.end();
                    b != e;
                    ++b) {
                fqsn->append((*b));
                fqsn->append(".");
            }
            fqsn->append(name);
            return true;
        }
    }

    return false;
}

bool
Context::setNamespacesForEnum(const char *name,
                              std::vector<std::string> *namespaces)
{
    std::string ss_name(name);

    if (strchr(name, '.')) {
        Namespace *ns = getNamespace(name, true);
        if (!ns) {
            return false;
        }
        const char *en_name = strrchr(name, '.') + 1;
        if (!ns->getEnum(en_name)) {
            return false;
        }
        splitString(&ss_name, namespaces, '.');
        namespaces->pop_back();
        return true;
    }

    for (std::vector<NSNode *>::reverse_iterator
            rb = used_ns_nodes.rbegin(),
            re = used_ns_nodes.rend();
            rb != re;
            ++rb) {
        Element::Enum *en = (*rb)->ns->getEnum(name);
        if (en) {
            (*rb)->ns->setNamespaces(namespaces);
            return true;
        }
    }

    return false;
}

void
getFunctionNames_(std::set<std::string> *names,
                  std::string *prefix,
                  NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        getFunctionNames_(names, prefix, b->second);
    }
    nsnode->ns->getFunctionNames(names, prefix);
}

void 
Context::getFunctionNames(std::set<std::string> *names,
                          std::string *prefix)
{
    getFunctionNames_(names, prefix, namespaces);
}

bool
merge_(NSNode *nsnode_dst, NSNode *nsnode_src)
{
    Namespace *ns_dst = nsnode_dst->ns;
    Namespace *ns_src = nsnode_src->ns;

    for (std::map<std::string, NSNode *>::iterator
            src_b = nsnode_src->children.begin(),
            src_e = nsnode_src->children.end();
            src_b != src_e;
            ++src_b) {
        std::map<std::string, NSNode *>::iterator dst_m =
            nsnode_dst->children.find(src_b->first);
        if (dst_m != nsnode_dst->children.end()) {
            merge_(dst_m->second, src_b->second);
        } else {
            src_b->second->ns->parent_namespace = ns_dst;
            nsnode_dst->children.insert(
                std::pair<std::string, NSNode *>(
                    src_b->first, src_b->second
                )
            );
        } 
    }

    ns_dst->merge(ns_src);

    return true;
}

bool
Context::merge(Context *other)
{
    return merge_(namespaces, other->namespaces);
}

bool
regetPointers_(llvm::Module *mod, NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        regetPointers_(mod, b->second);
    }
    nsnode->ns->regetPointers(mod);
    return true;
}

bool
regetFunctionPointers(llvm::Module *mod, NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        regetFunctionPointers(mod, b->second);
    }
    nsnode->ns->regetFunctionPointers(mod);
    return true;
}

bool
Context::rebuildFunctions(llvm::Module *mod, NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        rebuildFunctions(mod, b->second);
    }

    Namespace *ns = nsnode->ns; 

    std::map<std::string, std::vector<Element::Function *>* >::iterator
        b, e;

    for (b = ns->functions.begin(), e = ns->functions.end(); b != e; ++b) {
        for (std::vector<Element::Function *>::iterator
                fb = b->second->begin(),
                fe = b->second->end();
                fb != fe;
                ++fb) {
            Element::Function *fn = (*fb);

            fn->llvm_function = mod->getFunction(fn->internal_name->c_str());
            if (fn->llvm_function) {
                continue;
            }
            
            std::vector<llvm::Type*> types;

            for (std::vector<Element::Variable *>::iterator
                    vb = fn->parameter_types->begin(),
                    ve = fn->parameter_types->end();
                    vb != ve;
                    ++vb) {
                Element::Variable *var = (*vb);
                if (var->type->base_type == Type::VarArgs) {
                    break;
                }
                llvm::Type *llvm_type =
                    toLLVMType(var->type, NULL, false, false);
                if (!llvm_type) {
                    er->flush();
                    fprintf(stderr, "Failed conversion 1 (%s).\n",
                            b->first.c_str());
                    abort();
                }
                types.push_back(llvm_type);
            }

            llvm::Type *llvm_return_type =
                toLLVMType(fn->return_type, NULL, true, false);
            if (!llvm_return_type) {
                er->flush();
                fprintf(stderr, "Failed conversion 2 (RT) (%s).\n",
                        b->first.c_str());
                abort();
            }

            bool varargs = fn->isVarArgs();

            llvm::FunctionType *ft =
                llvm::FunctionType::get(
                    llvm_return_type,
                    llvm::ArrayRef<llvm::Type*>(types),
                    varargs
                );
            
            fn->llvm_function =
                llvm::dyn_cast<llvm::Function>(
                    mod->getOrInsertFunction(fn->internal_name->c_str(), 
                                             ft)
                );
            if (!fn->llvm_function) {
                fprintf(stderr, "Internal error: unable to re-get "
                        "function ('%s').\n",
                        fn->internal_name->c_str());
                abort();
            }
        }
    }
    
    return true;
}

bool
Context::rebuildVariables(llvm::Module *mod, NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        rebuildVariables(mod, b->second);
    }

    Namespace *ns = nsnode->ns;

    for (std::map<std::string, Element::Variable *>::iterator
            b = ns->variables.begin(),
            e = ns->variables.end();
            b != e;
            ++b) {
        /* todo: feels like a horrible hack. How does an empty-string
         * variable get into this map? */
        if (!b->first.compare("")) {
            continue;
        }
        Element::Variable *var = b->second;
        /* internal_name is only set when the variable's value
         * pointer needs to be updated after module linkage. */
        if (var->internal_name.size() > 0) {
            Element::Type *pptype = tr->getPointerType(var->type);
            llvm::Type *tt = toLLVMType(pptype, NULL, true, true);
            if (!tt) {
                fprintf(stderr, "Unable to perform type conversion.\n");
                abort();
            }
            llvm::PointerType *pt =
                llvm::cast<llvm::PointerType>(tt);
            llvm::Type *elt = pt->getElementType();
            if (!elt) {
                fprintf(stderr, "Unable to get element type.\n");
                abort();
            }

            var->value =
                llvm::cast<llvm::Value>(
                    mod->getOrInsertGlobal(
                        var->internal_name.c_str(),
                        elt
                    )
                );
            if (!var->value) {
                fprintf(stderr, "Internal error: unable to re-get "
                        "global variable ('%s').\n",
                        var->internal_name.c_str());
                abort();
            }
        } else {
            var->value =
                llvm::cast<llvm::Value>(
                    mod->getOrInsertGlobal(
                        (*b).first.c_str(),
                        llvm::cast<llvm::PointerType>(toLLVMType(
                                    new
                                    Element::Type(var->type
                                                     ->makeCopy()),
                                    NULL, true, true))->getElementType()

                    )
                );
            if (!var->value) {
                fprintf(stderr, "Internal error: unable to re-get "
                        "global variable ('%s', '%s').\n",
                        (*b).first.c_str(),
                        var->internal_name.c_str());
                abort();
            }
        }
    }

    return true;
}

llvm::GlobalValue::LinkageTypes 
Context::toLLVMLinkage(int linkage)
{
    return
        (linkage == dale::Linkage::Auto)
        ? llvm::GlobalValue::InternalLinkage
        : (linkage == dale::Linkage::Intern)
        ? llvm::GlobalValue::InternalLinkage
        : (linkage == dale::Linkage::Extern)
        ? llvm::GlobalValue::ExternalLinkage
        : (linkage == dale::Linkage::Extern_C)
        ? llvm::GlobalValue::ExternalLinkage
        : (linkage == dale::Linkage::Extern_Weak)
        ? llvm::GlobalValue::LinkOnceODRLinkage
        : llvm::GlobalValue::ExternalLinkage;
}

bool
Context::regetPointers(llvm::Module *mod)
{
    regetPointers_(mod, namespaces);
    rebuildFunctions(mod, namespaces);
    return true;
}

bool
Context::regetPointersForNewModule(llvm::Module *mod)
{
    regetPointers_(mod, namespaces);
    rebuildVariables(mod, namespaces);
    rebuildFunctions(mod, namespaces);
    regetFunctionPointers(mod, namespaces);
    return true;
}

llvm::Type *
Context::toLLVMTypeFunction(Element::Type *type,
                            Node *n,
                            bool refs_to_pointers)
{
    std::vector<llvm::Type*> llvm_fn_params;
    bool is_varargs = 0;

    std::vector<Element::Type*>::iterator iter;
    iter = type->parameter_types->begin();

    while (iter != type->parameter_types->end()) {
        if ((*iter)->base_type == Type::VarArgs) {
            is_varargs = true;
        } else {
            llvm::Type *t = toLLVMType((*iter), n,
                                       true, false,
                                       refs_to_pointers);
            if (!t) {
                return NULL;
            }
            llvm_fn_params.push_back(t);
        }
        ++iter;
    }

    Element::Type *r_type = type->return_type;
    llvm::Type *llvm_r_type = NULL;

    if (r_type->is_retval) {
        llvm_fn_params.push_back(
            toLLVMType(tr->getPointerType(r_type),
                       NULL, true)
        );
        llvm_r_type =
            toLLVMType(tr->getBasicType(dale::Type::Void),
                       NULL, true);
    } else {
        llvm_r_type = toLLVMType(r_type, NULL);
    }

    llvm::FunctionType *fntype = 
        llvm::FunctionType::get(
            llvm_r_type,
            llvm_fn_params,
            is_varargs
        );
    return fntype;
}

llvm::Type *
Context::toLLVMTypeArray(Element::Type *type,
                         Node *n)
{
    llvm::Type *new_type =
        llvm::ArrayType::get(
            toLLVMType(type->array_type, n),
            type->array_size
        );
    return new_type;
}

llvm::Type *
Context::toLLVMTypePointer(Element::Type *type,
                           Node *n,
                           bool refs_to_pointers)
{
    llvm::Type *temp_type = toLLVMType(type, n, true, false, 
                                       refs_to_pointers);

    if (!temp_type) {
        return NULL;
    }

    /* If this is a pointer to void, then return a _vp struct
        * instead. */
    if (temp_type->isVoidTy()) {
        Element::Struct *structp = getStruct("_vp", NULL);
        if (!structp) {
            fprintf(stderr, "Internal error: no _vp struct.\n");
            abort();
        }
        if (!structp->type) {
            fprintf(stderr, "Internal error: found vp struct, "
                    "but it doesn't have a type. (%s)\n",
                    type->struct_name->c_str());
            abort();
        }
        return llvm::PointerType::getUnqual(structp->type);
    }
    return llvm::PointerType::getUnqual(temp_type);
}

llvm::Type *
Context::toLLVMTypeBase(Element::Type *type,
                        Node *n)
{
    llvm::LLVMContext &lc = llvm::getGlobalContext();

    int base_type = type->base_type;
    llvm::Type *lbt = NULL;

    switch (base_type) {
        case Type::Int:        lbt = nt->getNativeIntType();          break;
        case Type::UInt:       lbt = nt->getNativeUIntType();         break;
        case Type::Char:       lbt = nt->getNativeCharType();         break;
        case Type::Void:       lbt = llvm::Type::getVoidTy(lc);       break;
        case Type::Bool:       lbt = llvm::Type::getInt1Ty(lc);       break;
        case Type::Float:      lbt = llvm::Type::getFloatTy(lc);      break;
        case Type::Double:     lbt = llvm::Type::getDoubleTy(lc);     break;
        case Type::LongDouble: lbt = nt->getNativeLongDoubleType();   break;
        case Type::Int8:       lbt = llvm::Type::getInt8Ty(lc);       break;
        case Type::UInt8:      lbt = llvm::Type::getInt8Ty(lc);       break;
        case Type::Int16:      lbt = llvm::Type::getInt16Ty(lc);      break;
        case Type::UInt16:     lbt = llvm::Type::getInt16Ty(lc);      break;
        case Type::Int32:      lbt = llvm::Type::getInt32Ty(lc);      break;
        case Type::UInt32:     lbt = llvm::Type::getInt32Ty(lc);      break;
        case Type::Int64:      lbt = llvm::Type::getInt64Ty(lc);      break;
        case Type::UInt64:     lbt = llvm::Type::getInt64Ty(lc);      break;
        case Type::IntPtr:     lbt = nt->getNativeIntptrType();       break;
        case Type::Size:       lbt = nt->getNativeSizeType();         break;
        case Type::PtrDiff:    lbt = nt->getNativePtrDiffType();      break;
        case Type::Int128:     lbt = llvm::IntegerType::get(lc, 128); break;
        case Type::UInt128:    lbt = llvm::IntegerType::get(lc, 128); break;
    }

    return lbt;
}

llvm::Type *
Context::toLLVMTypeStruct(Element::Type *type,
                          Node *n)
{
    Element::Struct *structp =
        getStruct(type->struct_name->c_str(), type->namespaces);

    if (structp) {
        if (!structp->type) {
            fprintf(stderr, "Internal error: found struct, "
                    "but it doesn't have a type. (%s)\n",
                    type->struct_name->c_str());
            abort();
        }
        return structp->type;
    }

    return NULL;
}

llvm::Type *
Context::toLLVMType_(Element::Type *type,
                     Node *n,
                     bool refs_to_pointers)
{
    llvm::LLVMContext &lc = llvm::getGlobalContext();

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
        llvm::Type *result = toLLVMTypePointer(type, n, refs_to_pointers);
        type->is_reference = 1;
        return result;
    }

    llvm::Type *base_type = toLLVMTypeBase(type, n);
    if (base_type) {
        return base_type;
    }

    if (type->struct_name != NULL) {
        llvm::Type *struct_type = toLLVMTypeStruct(type, n);
        if (struct_type) {
            return struct_type;
        }
    }

    std::string type_str;
    type->toStringProper(&type_str);
    Error *e = new Error(
        ErrorInst::Generator::UnableToConvertTypeToLLVMType,
        (n ? n : nullNode()),
        type_str.c_str()
    );
    er->addError(e);

    return NULL;
}

llvm::Type *
Context::toLLVMType(Element::Type *type,
                    Node *n,
                    bool allow_non_first_class,
                    bool externally_defined,
                    bool refs_to_pointers)
{
    int error_count =
        er->getErrorTypeCount(ErrorType::Error);

    /* If type designates an opaque struct, then disallow
     * instantiation. */
    if (!allow_non_first_class) {
        if (type->struct_name) {
            Element::Struct *structp =
                getStruct(type->struct_name->c_str(),
                          type->namespaces);
            if (structp) {
                if (((structp->linkage == StructLinkage::Opaque)
                        || (structp->element_types.size() == 0))
                        && !externally_defined) {
                    Error *e = new Error(
                        ErrorInst::Generator::CannotInstantiateOpaqueStruct,
                        (n ? n : nullNode())
                    );
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
        if (type->struct_name && externally_defined) {
            /* Even when allow_non_first_class is specified, if
             * this is an externally defined struct, then return
             * the type (see e.g. _IO_2_1_stdin_). */
            return llvm_type;
        }
        if (error_count != er->getErrorTypeCount(ErrorType::Error)) {
            return llvm_type;
        }
        std::string temp;
        type->toStringProper(&temp);
        Error *e = new Error(
            ErrorInst::Generator::TypeIsNotFirstClass,
            (n ? n : nullNode()),
            temp.c_str()
        );
        er->addError(e);
        return NULL;
    }

    return llvm_type;
}

bool
removeUnneeded_(std::set<std::string> *forms,
                std::set<std::string> *found_forms,
                NSNode *nsnode)
{
    nsnode->ns->removeUnneeded(forms, found_forms);

    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        std::set<std::string>::iterator fb = forms->find(b->first);
        if (fb != forms->end()) {
            found_forms->insert((*fb));
            continue;
        }

        std::set<std::string> subforms;
        for (std::set<std::string>::iterator
                fb = forms->begin(),
                fe = forms->end();
                fb != fe;
                ++fb) {
            if (fb->find(b->first) == 0) {
                std::string subform((*fb));
                subform.erase(0, (b->first.size() + 1));
                subforms.insert(subform);
            }
        }

        std::set<std::string> subfound_forms;

        removeUnneeded_(&subforms,
                        &subfound_forms,
                        b->second);

        for (std::set<std::string>::iterator
                sfb = subfound_forms.begin(),
                sfe = subfound_forms.end();
                sfb != sfe;
                ++sfb) {
            std::string fullform;
            fullform.append(b->first)
                    .append(".")
                    .append(*sfb);
            found_forms->insert(fullform);
        }
    }

    return true;
}

bool
Context::removeUnneeded(std::set<std::string> *forms,
                        std::set<std::string> *found_forms)
{
    return removeUnneeded_(forms, found_forms, namespaces);
}

bool
eraseOnceForms_(std::set<std::string> *once_tags,
                llvm::Module *mod,
                NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        eraseOnceForms_(once_tags, mod, b->second);
    }
    nsnode->ns->eraseOnceFunctions(once_tags, mod);
    nsnode->ns->eraseOnceVariables(once_tags, mod);

    return true;
}

bool
Context::eraseOnceForms(std::set<std::string> *once_tags,
                        llvm::Module *mod)
{
    return eraseOnceForms_(once_tags, mod, namespaces);
}

void
relink_(NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        b->second->ns->parent_namespace = nsnode->ns;
        relink_(b->second);
    }
}

void
Context::relink(void)
{
    relink_(namespaces);
}

void
print_(NSNode *nsnode)
{
    nsnode->ns->print();

    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        fprintf(stderr, "Namespace (>): %s\n",
                        b->first.c_str());
        print_(b->second);
    }
}

void
Context::print(void)
{
    print_(namespaces);
}

bool
deleteAnonymousNamespaces_(NSNode *nsnode)
{
    if (nsnode->ns->name.find("anon") == 0) {
        delete nsnode->ns;
        nsnode->ns = NULL;
    }

    std::map<std::string, NSNode *>::iterator
        b = nsnode->children.begin();
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

bool
Context::deleteAnonymousNamespaces(void)
{
    return deleteAnonymousNamespaces_(namespaces);
}

bool
removeDeserialised_(NSNode *nsnode)
{
    for (std::map<std::string, NSNode *>::iterator
            b = nsnode->children.begin(),
            e = nsnode->children.end();
            b != e;
            ++b) {
        removeDeserialised_(b->second);
    }

    nsnode->ns->removeDeserialised();

    return true;
}

bool
Context::removeDeserialised(void)
{
    return removeDeserialised_(namespaces);
}


}
