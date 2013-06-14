#include "Namespace.h"

#include "llvm/LinkAllVMCore.h"
#include "../NativeTypes/NativeTypes.h"

namespace dale
{
Namespace::Namespace(void)
{
    /* For serialisation use only. A parent namespace and error
     * reporter must be set post-deserialisation. */
    this->parent_namespace = NULL;
    this->er = NULL;
}

Namespace::Namespace(ErrorReporter *er,
                     std::string name,
                     Namespace *pns,
                     int lv_index)
{
    this->er = er;
    this->name = name;
    this->parent_namespace = pns;
    this->lv_index = lv_index;

    if (pns) {
        std::vector<std::string> names;
        if (name.compare(".")) {
            names.push_back(name);
        }
        while (pns && pns->parent_namespace) {
            names.push_back(pns->name);
            pns = pns->parent_namespace;
        }
        char num[5];
        for (std::vector<std::string>::reverse_iterator 
                b = names.rbegin(),
                e = names.rend();
                b != e;
                ++b) {
            sprintf(num, "%lu", b->length());
            symbol_prefix.append(num)
                         .append(*b);
        }
    }

    has_symbol_prefix = (symbol_prefix.size() ? true : false);
}

Namespace::~Namespace(void)
{
    /* Delete each Element::Function vector in the functions map. */ 
}

bool
Namespace::setNamespaces(std::vector<std::string> *namespaces)
{
    std::vector<std::string> names;

    Namespace *pns = parent_namespace;

    if (pns) {
        names.push_back(name);
    }

    while (pns && pns->parent_namespace) {
        names.push_back(pns->name);
        pns = pns->parent_namespace;
    }

    for (std::vector<std::string>::reverse_iterator
            b = names.rbegin(),
            e = names.rend();
            b != e;
            ++b) {
        namespaces->push_back((*b));
    }

    return true;
}

bool
Namespace::addFunction(const char *name,
                       Element::Function *function,
                       Node *n)
{
    std::map<std::string, std::vector<Element::Function *>* >::iterator iter;
    std::vector<Element::Function *>::iterator fn_iter;

    std::string ss_name(name);
    iter = functions.find(ss_name);

    if (iter == functions.end()) {
        std::vector<Element::Function *> *fns =
            new std::vector<Element::Function *>;
        fns->push_back(function);

        functions.insert(
            std::pair<std::string, std::vector<Element::Function *> *>(
                ss_name, fns
            )
        );

        return true;
    }

    fn_iter = iter->second->begin();
    while (fn_iter != iter->second->end()) {
        Element::Function *fn = (*fn_iter);
        if (fn->is_macro == function->is_macro) {
            int is_equal = function->isEqualTo(fn); 
            if (fn == function) {
                return true;
            } else if (is_equal && (fn->isDeclaration())) {
                iter->second->erase(fn_iter);
            } else if (is_equal && (!fn->isDeclaration())) {
                return true;
            } else {
                ++fn_iter;
            }
        } else {
            std::vector<Element::Variable *>::iterator 
                fn_pt_begin       = fn->parameter_types->begin(),
                fn_pt_end         = fn->parameter_types->end(),
                function_pt_begin = function->parameter_types->begin(),
                function_pt_end   = function->parameter_types->end();
            if (fn->is_macro) {
                std::advance(fn_pt_begin, 2);
            }
            else {
                std::advance(function_pt_begin, 2);
            }
            if (dale::stl::isEqualToIter(function_pt_begin,
                                         function_pt_end,
                                         fn_pt_begin,
                                         fn_pt_end)) {
                Error *e = new Error(
                    ((fn->is_macro)
                        ? ErrorInst::Generator::FunctionHasSameParamsAsMacro
                        : ErrorInst::Generator::MacroHasSameParamsAsFunction),
                    (n ? n : nullNode()), 
                    name
                );
                er->addError(e);
                return false;
            }
            ++fn_iter;
        }
    }

    iter->second->push_back(function);

    return true;
}

bool 
Namespace::addVariable(const char *name,
                       Element::Variable *variable)
{
    std::map<std::string, Element::Variable *>::iterator iter;
    std::string ss_name(name);

    iter = variables.find(ss_name);

    if (iter == variables.end()) {
        variables.insert(
            std::pair<std::string, Element::Variable *>(ss_name, variable)
        );
        variable->index = ++lv_index;
        return true;
    } else {
        return false;
    }
}

bool
Namespace::addStruct(const char *name,
                     Element::Struct *element_struct)
{
    std::map<std::string, Element::Struct *>::iterator iter;
    std::string ss_name(name);

    iter = structs.find(ss_name);

    if (iter == structs.end()) {
        structs.insert(
            std::pair<std::string, Element::Struct *>(
                ss_name, element_struct
            )
        );
        return true;
    } else {
        return false;
    } 
}

bool
Namespace::addEnum(const char *name, 
                   Element::Enum *element_enum)
{
    std::map<std::string, Element::Enum *>::iterator iter;
    std::string ss_name(name);

    iter = enums.find(ss_name);

    if (iter == enums.end()) {
        enums.insert(
            std::pair<std::string, Element::Enum *>(
                ss_name, element_enum
            )
        );
        return true;
    } else {
        return false;
    }
}

Element::Function *
Namespace::getFunction(const char *name,
                       std::vector<Element::Type *> *types,
                       Element::Function **pclosest_fn,
                       bool is_macro)
{
    std::string ss_name(name);

    std::map<std::string, std::vector<Element::Function *> *>::iterator
        iter = functions.find(ss_name);
    if (iter == functions.end()) {
        return NULL;
    }

    std::vector<Element::Function *> *function_list = iter->second;
    if (function_list->size() == 0) {
        return NULL;
    }

    if (!types) {
        Element::Function *last_non_declaration = NULL;

        for (std::vector<Element::Function *>::reverse_iterator
                rb = function_list->rbegin(),
                re = function_list->rend();
                rb != re;
                ++rb) {
            Element::Function *fn = (*rb);
            if (is_macro == fn->is_macro) {
                if (!fn->isDeclaration()) {
                    return fn;
                }
                else if (!last_non_declaration) {
                    last_non_declaration = fn;
                }
            }
        }
        return last_non_declaration;
    }

    /* If types are provided, is_macro is only taken into account if
     * it is true. This is because you may want to get only the macros
     * at a given point (the top-level), but there is no point at
     * which you want prospective functions sans macros. */

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

    /* If the argument type list does not comprise (p DNode)s, then
     * change the last argument to a (p DNode) and re-call this
     * function. (E.g. you may have a macro called identity that takes
     * a (p DNode) - if you call it with a value parseable as an int,
     * it won't be found until you check for (p DNode) arguments.
     * Undoubtedly there is a much more efficient way of doing this.
     * */

    std::vector<Element::Type *>::reverse_iterator rarg_type_iter;
    rarg_type_iter = types->rbegin();

    Element::Type dnode_struct;
    std::string dn("DNode");
    dnode_struct.struct_name = &dn;
    std::vector<std::string> namespaces;
    dnode_struct.namespaces = &namespaces;
    Element::Type r_type(&dnode_struct);

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
    (*rarg_type_iter) = &r_type;
    Element::Function *temp = getFunction(
                                    name,
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

Element::Variable *
Namespace::getVariable(const char *name)
{
    std::string ss_name(name);
    std::map<std::string, Element::Variable *>::iterator
        iter = variables.find(ss_name);
    if (iter != variables.end()) {
        return iter->second;
    }
    return NULL;
}

Element::Struct *
Namespace::getStruct(const char *name)
{
    std::string ss_name(name);
    std::map<std::string, Element::Struct *>::iterator
        iter = structs.find(ss_name);
    if (iter != structs.end()) {
        return iter->second;
    }
    return NULL;
}

Element::Enum *
Namespace::getEnum(const char *name)
{
    std::string ss_name(name);
    std::map<std::string, Element::Enum *>::iterator
        iter = enums.find(ss_name);
    if (iter != enums.end()) {
        return iter->second;
    }
    return NULL;
}

void
Namespace::getVarsAfterIndex(int index,
                             std::vector<Element::Variable *> *vars)
{
    for (std::map<std::string, Element::Variable *>::iterator
            b = variables.begin(),
            e = variables.end();
            b != e;
            ++b) {
        Element::Variable *v = b->second;
        if (!v->index) {
            continue;
        }
        if (v->index >= index) {
            vars->push_back(v);
        }
    }

    if (parent_namespace) {
        parent_namespace->getVarsAfterIndex(index, vars);
    }

    return;
}

void
Namespace::getVarsBeforeIndex(int index,
                              std::vector<Element::Variable *> *vars)
{
    for (std::map<std::string, Element::Variable *>::iterator
            b = variables.begin(),
            e = variables.end();
            b != e;
            ++b) {
        Element::Variable *v = b->second;
        if (!v->index) {
            continue;
        }
        if (v->index <= index) {
            vars->push_back(v);
        }
    }

    if (parent_namespace) {
        parent_namespace->getVarsBeforeIndex(index, vars);
    }

    return;
}

void
encodeStandard(const std::string *from,
               std::string *to)
{
    char buf[5];

    for (std::string::const_iterator b = from->begin(),
                                     e = from->end();
            b != e;
            ++b) {
        char c = *b;
        sprintf(buf, ((isalnum(c) || c == '_') ? "%c" : "$%x"), c);
        to->append(buf);
    }

    return; 
}

void
Namespace::nameToSymbol(const char *name, 
                        std::string *new_name)
{
    new_name->append("_Z");

    if (has_symbol_prefix) {
        new_name->append("N");
        new_name->append(symbol_prefix);
    }

    char num[4];
    sprintf(num, "%zu", strlen(name));
    new_name->append(num);

    std::string ss_name(name);
    encodeStandard(&ss_name, new_name);

    if (has_symbol_prefix) {
        new_name->append("E");
    }

    return;
}

void
Namespace::functionNameToSymbol(const char *name,
                                std::string *new_name,
                                int linkage,
                                std::vector<Element::Variable *> *types)
{
    if (linkage == dale::Linkage::Extern_C) {
        new_name->append(name);
        return;
    }

    nameToSymbol(name, new_name);

    for (std::vector<Element::Variable *>::iterator
            b = types->begin(),
            e = types->end();
            b != e;
            ++b) {
        (*b)->type->toEncStr(new_name);
    }

    return;
}

void
Namespace::eraseLLVMMacros(void)
{
    std::map<std::string, std::vector<Element::Function *>* >::iterator 
        b, e;
    std::vector<Element::Function *>::iterator fn_b, fn_e;

    std::set<llvm::Function *> erased;

    for (b = functions.begin(), e = functions.end(); b != e; ++b) {
        for (fn_b = b->second->begin(), fn_e = b->second->end();
                fn_b != fn_e;
                ++fn_b) {
            Element::Function *fn = (*fn_b);
            if (!fn->is_macro) {
                continue;
            }
            if (!fn->llvm_function) {
                continue;
            }
            llvm::Function *lfn = fn->llvm_function;
            fn->llvm_function = NULL;
            if (erased.find(lfn) == erased.end()) {
                erased.insert(lfn);
                lfn->eraseFromParent();
            }
        }
    }

    return;
}

void
Namespace::eraseLLVMMacrosAndCTOFunctions(void)
{
    std::map<std::string, std::vector<Element::Function *>* >::iterator 
        b, e;
    std::vector<Element::Function *>::iterator fn_b, fn_e;

    for (b = functions.begin(), e = functions.end(); b != e; ++b) {
        for (fn_b = b->second->begin(), fn_e = b->second->end();
                fn_b != fn_e;
                ++fn_b) {
            Element::Function *fn = (*fn_b);
            if (!fn->is_macro && !fn->cto) {
                continue;
            }
            if (!fn->llvm_function) {
                continue;
            }
            llvm::Module *m = fn->llvm_function->getParent();
            if (m->getFunction(llvm::StringRef(fn->internal_name->c_str()))) {
                fn->llvm_function->eraseFromParent();
            }
        }
    }

    return;
}

void
Namespace::getFunctionNames(std::set<std::string> *names)
{
    std::map<std::string, std::vector<Element::Function*> *>::iterator
        b, e;

    for (b = functions.begin(), e = functions.end(); b != e; ++b) {
        names->insert(b->first);
    }
}

void
Namespace::getVariables(std::vector<Element::Variable *> *vars)
{
    for (std::map<std::string, Element::Variable *>::reverse_iterator
            b = variables.rbegin(),
            e = variables.rend();
            b != e;
            ++b) {
        vars->push_back(b->second);
    }
}

bool
Namespace::merge(Namespace *other)
{
    if (name.compare(other->name)) {
        fprintf(stderr,
                "Merging namespaces with different names: %s, %s\n",
                name.c_str(), other->name.c_str());
        abort();
    }

    if (other->lv_index > lv_index) {
        lv_index = lv_index + 1;
    }

    std::map<std::string, std::vector<Element::Function *> *>::iterator
        b, e;

    for (b = other->functions.begin(), e = other->functions.end();
            b != e;
            ++b) {
        for (std::vector<Element::Function*>::iterator
                fb = b->second->begin(),
                fe = b->second->end();
                fb != fe;
                ++fb) {
            Element::Function *fn = (*fb);
            if (!Linkage::isExtern(fn->return_type->linkage)) {
                continue;
            }
            bool added = addFunction(b->first.c_str(), fn, NULL);
            if (!added) {
                fprintf(stderr,
                        "Unable to merge function: %s.\n",
                        b->first.c_str());
                abort();
            }
        }
    }

    for (std::map<std::string, Element::Enum*>::iterator
            b = other->enums.begin(),
            e = other->enums.end();
            b != e;
            ++b) {
        if (!EnumLinkage::isExtern(b->second->linkage)) {
            continue;
        }
        if (getEnum(b->first.c_str())) {
            continue;
        }
        bool added = addEnum(b->first.c_str(), b->second);
        if (!added) {
            fprintf(stderr,
                    "Unable to merge enum: %s.\n",
                    b->first.c_str());
            abort();
        }
    }

    for (std::map<std::string, Element::Variable*>::iterator
            b = other->variables.begin(),
            e = other->variables.end();
            b != e;
            ++b) {
        if (!Linkage::isExtern(b->second->type->linkage)) {
            continue;
        }
        if (getVariable(b->first.c_str())) {
            continue;
        }
        bool added = addVariable(b->first.c_str(), b->second);
        if (!added) {
            fprintf(stderr,
                    "Unable to merge variable: %s.\n",
                    b->first.c_str());
            abort();
        }
    }

    for (std::map<std::string, Element::Struct*>::iterator
            b = other->structs.begin(),
            e = other->structs.end();
            b != e;
            ++b) {
        if (!StructLinkage::isExtern(b->second->linkage)) {
            continue;
        }
        if (getStruct(b->first.c_str())) {
            continue;
        }
        bool added = addStruct(b->first.c_str(), b->second);
        if (!added) {
            fprintf(stderr,
                    "Unable to merge struct: %s.\n",
                    b->first.c_str());
            abort();
        }
    }

    return true;
}

bool
Namespace::regetStructPointers(llvm::Module *mod)
{
    for (std::map<std::string, Element::Struct *>::iterator
            b = structs.begin(),
            e = structs.end();
            b != e;
            ++b) {
        Element::Struct *st = b->second;
        if (!st->internal_name) {
            continue;
        }
        std::string type_name;
        type_name.append("struct_")
                 .append(*(st->internal_name));

        llvm::StructType *llvm_st = mod->getTypeByName(type_name);
        if (!llvm_st) {
            type_name.clear();
            type_name.append(*(st->internal_name));
            llvm_st = mod->getTypeByName(type_name);
        }
        if (!llvm_st) {
            fprintf(stderr, "Could not get type for struct %s.\n",
                    st->internal_name->c_str());
            abort();
        }
        st->type = llvm_st;
    }

    return true;
}

bool
Namespace::regetVariablePointers(llvm::Module *mod)
{
    for (std::map<std::string, Element::Variable *>::iterator
            b = variables.begin(),
            e = variables.end();
            b != e;
            ++b) {
        Element::Variable *var = b->second;
        /* internal_name is only set when the variable's value pointer
         * needs to be updated on merge.  */
        std::string *in = var->internal_name;
        if (!(in && in->size())) {
            continue;
        }
        if (!var->value) {
            /* Can't get type if there is no llvm::Value for this
             * value. In that case, the variable's status becomes the
             * responsibility of the caller. */
            continue;
        }
        var->value =
            llvm::cast<llvm::Value>(
                mod->getOrInsertGlobal(
                    in->c_str(),
                    llvm::cast<llvm::PointerType>(
                        var->value->getType()
                    )->getElementType()
                )
            );
        if (!var->value) {
            fprintf(stderr, "Unable to re-get global variable: %s\n",
                    in->c_str());
            abort();
        }
    }

    return true;
}

bool
Namespace::regetFunctionPointers(llvm::Module *mod)
{
    std::map<std::string, std::vector<Element::Function *>* >::iterator 
        b, e;
    
    for (b = functions.begin(), e = functions.end(); b != e; ++b) {
        if (b->first.compare("va-start")) {
            continue;
        }
        if (b->first.compare("va-end")) {
            continue;
        }
        for (std::vector<Element::Function *>::iterator
                fb = b->second->begin(),
                fe = b->second->end();
                fb != fe;
                ++fb) {
            Element::Function *fn = (*fb);
            /* todo: trying this for a moment. */
            /*
            if (!fn->llvm_function) {
                continue;
            }
            */
            fn->llvm_function =
                mod->getFunction(fn->internal_name->c_str());
            /* If fn->llvm_function is NULL at this point, is becomes
             * the responsibility of Context::rebuildFunctions to fix
             * it. */
        }
    }

    return true;
}

bool
Namespace::regetPointers(llvm::Module *mod)
{
    regetStructPointers(mod);
    regetVariablePointers(mod);
    regetFunctionPointers(mod);
    return true;
}

bool
Namespace::eraseOnceFunctions(std::set<std::string> *once_tags,
                              llvm::Module *mod)
{
    std::map<std::string, std::vector<Element::Function*> *>::iterator
        b, e;

    for (b = functions.begin(), e = functions.end(); b != e; ++b) {
        for (std::vector<Element::Function*>::iterator
                fb = b->second->begin(),
                fe = b->second->end();
                fb != fe;
                ++fb) {
            Element::Function *fn = (*fb);
            if (!fn->internal_name) {
                continue;
            }
            if (once_tags->find(fn->once_tag) != once_tags->end()) {
                llvm::Function *fn_to_remove =
                    mod->getFunction(fn->internal_name->c_str());
                if (fn_to_remove) {
                    fn_to_remove->deleteBody();
                }
            }
        }
    }

    return true;
}

bool
Namespace::eraseOnceVariables(std::set<std::string> *once_tags,
                              llvm::Module *mod)
{
    for (std::map<std::string, Element::Variable*>::iterator
            b = variables.begin(),
            e = variables.end();
            b != e;
            ++b) {
        Element::Variable *var = b->second;
        if (!var->internal_name) {
            continue;
        }
        if (once_tags->find(var->once_tag) != once_tags->end()) {
            llvm::GlobalVariable *var_to_remove =
                mod->getGlobalVariable(var->internal_name->c_str());
            if (var_to_remove) {
                var_to_remove->setInitializer(NULL);
                var->value = llvm::cast<llvm::Value>(var_to_remove);
            }
        }
    }

    return true;
}

bool
Namespace::removeUnneededStructs(std::set<std::string> *forms,
                                 std::set<std::string> *found_forms)
{
    std::map<std::string, Element::Struct *>::iterator
        b = structs.begin(),
        e = structs.end();
    while (b != e) {
        std::set<std::string>::iterator fb = forms->find(b->first);
        if (fb == forms->end()) {
            structs.erase(b++);
        } else {
            if (StructLinkage::isExtern(b->second->linkage)) {
                found_forms->insert((*fb));
            }
            ++b;
        }
    }

    return true;
}

bool
Namespace::removeUnneededEnums(std::set<std::string> *forms,
                               std::set<std::string> *found_forms)
{
    std::map<std::string, Element::Enum *>::iterator
        b = enums.begin(),
        e = enums.end();
    
    while (b != e) {
        std::set<std::string>::iterator fb = forms->find(b->first);
        if (fb == forms->end()) {
            enums.erase(b++);
        } else {
            if (EnumLinkage::isExtern(b->second->linkage)) {
                found_forms->insert((*fb));
            }
            ++b;
        }
    }

    return true;
}

bool
Namespace::removeUnneededVariables(std::set<std::string> *forms,
                                   std::set<std::string> *found_forms)
{
    std::map<std::string, Element::Variable *>::iterator
        b = variables.begin(),
        e = variables.end();

    while (b != e) {
        std::set<std::string>::iterator fb = forms->find(b->first);
        if (fb == forms->end()) {
            variables.erase(b++);
        } else {
            if (Linkage::isExtern(b->second->type->linkage)) {
                found_forms->insert((*fb));
            }
            ++b;
        }
    }

    return true;
}

bool
Namespace::removeUnneededFunctions(std::set<std::string> *forms,
                                   std::set<std::string> *found_forms)
{
    std::map<std::string, std::vector<Element::Function*> *>::iterator
        b = functions.begin(),
        e = functions.end();

    while (b != e) {
        std::set<std::string>::iterator fb = forms->find(b->first);
        if (fb == forms->end()) {
            functions.erase(b++);
        } else {
            /* If every function is intern, then skip this (but
             * don't erase - it will not be merged in any event). */
            bool has_extern;
            for (std::vector<Element::Function*>::iterator
                    fnb = b->second->begin(),
                    fne = b->second->end();
                    fnb != fne;
                    ++fnb) {
                if (Linkage::isExternAll((*fnb)->return_type->linkage)) {
                    has_extern = true;
                    break;
                }
            }
            if (has_extern) {
                found_forms->insert((*fb));
            }
            ++b;
        }
    }

    return true;
}

bool
Namespace::removeUnneeded(std::set<std::string> *forms,
                          std::set<std::string> *found_forms)
{
    removeUnneededFunctions(forms, found_forms);
    removeUnneededVariables(forms, found_forms);
    removeUnneededStructs(forms, found_forms);
    removeUnneededEnums(forms, found_forms);
    return true;
}

void
Namespace::print(void)
{
    fprintf(stderr, "Namespace: %s\n", name.c_str());
    fprintf(stderr, "Parent namespace: (%s)\n",
                    ((parent_namespace != NULL) && 
                     (parent_namespace->name.c_str()))
                        ? parent_namespace->name.c_str()
                        : "(nil)");

    for (std::map<std::string, std::vector<Element::Function*>*>::iterator
            b = functions.begin(),
            e = functions.end();
            b != e;
            ++b) {
        fprintf(stderr, "Function: %s (%lu)\n", 
                        b->first.c_str(),
                        b->second->size());
    }
    for (std::map<std::string, Element::Struct *>::iterator
            b = structs.begin(),
            e = structs.end();
            b != e;
            ++b) {
        fprintf(stderr, "Struct: %s\n", b->first.c_str());
    }
    for (std::map<std::string, Element::Enum *>::iterator
            b = enums.begin(),
            e = enums.end();
            b != e;
            ++b) {
        fprintf(stderr, "Enum: %s\n", b->first.c_str());
    }
    for (std::map<std::string, Element::Variable *>::iterator
            b = variables.begin(),
            e = variables.end();
            b != e;
            ++b) {
        fprintf(stderr, "Variable: %s\n", b->first.c_str());
    }
}

}
