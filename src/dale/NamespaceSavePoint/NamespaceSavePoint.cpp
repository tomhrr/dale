#include "NamespaceSavePoint.h"

#include "llvm/LinkAllVMCore.h"
#include "../NativeTypes/NativeTypes.h"

namespace dale
{
NamespaceSavePoint::NamespaceSavePoint(Namespace *ns)
{
    for (std::map<std::string, std::vector<Element::Function *>* >::iterator
            b = ns->functions.begin(),
            e = ns->functions.end();
            b != e;
            ++b) {
        function_count.insert(
            std::pair<std::string, int>(b->first,
                                        b->second->size())
        );
    }

    for (std::map<std::string, Element::Variable*>::iterator
            b = ns->variables.begin(),
            e = ns->variables.end();
            b != e;
            ++b) {
        variables.insert(b->first);
    }

    for (std::map<std::string, Element::Struct*>::iterator
            b = ns->structs.begin(),
            e = ns->structs.end();
            b != e;
            ++b) {
        structs.insert(b->first);
    }

    for (std::map<std::string, Element::Enum*>::iterator
            b = ns->enums.begin(),
            e = ns->enums.end();
            b != e;
            ++b) {
        enums.insert(b->first);
    }

    src_ns = ns;
}

NamespaceSavePoint::~NamespaceSavePoint(void)
{
}

bool NamespaceSavePoint::restore(void)
{
    std::map<std::string, std::vector<Element::Function *>*>::iterator
        fb;

    for (std::map<std::string, int>::iterator
            b = function_count.begin(),
            e = function_count.end();
            b != e;
            ++b) {
        fb = src_ns->functions.find(b->first);
        if (fb == src_ns->functions.end()) {
            fprintf(stderr, "Looking for function list for '%s', "
                            "but it no longer exists in the "
                            "context.", b->first.c_str());
            abort();
        }
        int count = b->second;
        int to_remove = fb->second->size() - count;
        while (to_remove--) {
            if (fb->second->back()->llvm_function) {
                fb->second->back()->llvm_function->eraseFromParent();
            }
            fb->second->pop_back();
        }
    }

    for (std::map<std::string, Element::Variable *>::iterator
            b = src_ns->variables.begin(),
            e = src_ns->variables.end();
            b != e;
            ++b) {
        if (variables.find(b->first) == variables.end()) {
            src_ns->variables.erase(b);
        }
    }

    for (std::map<std::string, Element::Struct *>::iterator
            b = src_ns->structs.begin(),
            e = src_ns->structs.end();
            b != e;
            ++b) {
        if (structs.find(b->first) == structs.end()) {
            src_ns->structs.erase(b);
        }
    }

    for (std::map<std::string, Element::Enum *>::iterator
            b = src_ns->enums.begin(),
            e = src_ns->enums.end();
            b != e;
            ++b) {
        if (enums.find(b->first) == enums.end()) {
            src_ns->enums.erase(b);
        }
    }

    return true;
}
}
