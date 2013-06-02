#include "ContextSavePoint.h"

#include "llvm/LinkAllVMCore.h"
#include "../NativeTypes/NativeTypes.h"

#define DEBUG 0

/* todo: probably a fair bit of leakage going on here. Have to remove
 * structs, enums, variables from their parent modules as
 * well. */

namespace dale
{
ContextSavePoint::ContextSavePoint(Context *ctx)
{
    function_count = new std::map<std::string, int>;
    variables      = new std::set<std::string>;
    structs        = new std::set<std::string>;
    enums          = new std::set<std::string>;
    namespaces     = new std::map<std::string, ContextSavePoint *>;

    for (std::map<std::string, std::vector<Element::Function *>* >::iterator
            b = ctx->functions->begin(),
            e = ctx->functions->end();
            b != e;
            ++b) {
        function_count->insert(
            std::pair<std::string, int>(b->first,
                                        b->second->size())
        );
    }

    for (std::map<std::string, Element::Variable*>::iterator
            b = ctx->variables->begin(),
            e = ctx->variables->end();
            b != e;
            ++b) {
        variables->insert(b->first);
    }

    for (std::map<std::string, Element::Struct*>::iterator
            b = ctx->structs->begin(),
            e = ctx->structs->end();
            b != e;
            ++b) {
        structs->insert(b->first);
    }

    for (std::map<std::string, Element::Enum*>::iterator
            b = ctx->enums->begin(),
            e = ctx->enums->end();
            b != e;
            ++b) {
        enums->insert(b->first);
    }

    for (std::map<std::string, Context *>::iterator
            b = ctx->namespace_to_context->begin(),
            e = ctx->namespace_to_context->end();
            b != e;
            ++b) {
        namespaces->insert(
            std::pair<std::string, ContextSavePoint *>(
                b->first,
                new ContextSavePoint(b->second)
            )
        );
    }

    active_namespace_count  = ctx->active_namespaces->size();
    used_namespace_count    = ctx->used_namespaces->size();
    current_namespace_count = ctx->current_namespaces->size();

    my_context = ctx;
}

ContextSavePoint::~ContextSavePoint(void)
{
    delete function_count;
    delete variables;
    delete structs;
    delete enums;
    delete namespaces;
}

bool ContextSavePoint::restore(void)
{
    Context *ctx = my_context;

    for (std::map<std::string, int>::iterator
            b = function_count->begin(),
            e = function_count->end();
            b != e;
            ++b) {
        std::map<std::string, std::vector<Element::Function *>*>::iterator
        fns = ctx->functions->find(b->first);
        if (fns == ctx->functions->end()) {
            fprintf(stderr, "Looking for function list for '%s', "
                    "but it no longer exists in the "
                    "context.", b->first.c_str());
            abort();
        }
        int count = b->second;
        int to_remove = fns->second->size() - count;
        while (to_remove--) {
            if (fns->second->back()->llvm_function) {
                fns->second->back()->llvm_function->eraseFromParent();
            }
            fns->second->pop_back();
        }
    }

    for (std::map<std::string, Element::Variable *>::iterator
            b = ctx->variables->begin(),
            e = ctx->variables->end();
            b != e;
            ++b) {
        if (variables->find(b->first) == variables->end()) {
            ctx->variables->erase(b);
        }
    }

    for (std::map<std::string, Element::Struct *>::iterator
            b = ctx->structs->begin(),
            e = ctx->structs->end();
            b != e;
            ++b) {
        if (structs->find(b->first) == structs->end()) {
            ctx->structs->erase(b);
        }
    }

    for (std::map<std::string, Element::Enum *>::iterator
            b = ctx->enums->begin(),
            e = ctx->enums->end();
            b != e;
            ++b) {
        if (enums->find(b->first) == enums->end()) {
            ctx->enums->erase(b);
        }
    }

    for (std::map<std::string, Context *>::iterator
            b = ctx->namespace_to_context->begin(),
            e = ctx->namespace_to_context->end();
            b != e;
            ++b) {
        if (namespaces->find(b->first) == namespaces->end()) {
            ctx->namespace_to_context->erase(b);
        }
    }

    for (std::map<std::string, ContextSavePoint *>::iterator
            b = namespaces->begin(),
            e = namespaces->end();
            b != e;
            ++b) {
        b->second->restore();
    }

    return true;
}
}
