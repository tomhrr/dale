#include "NamespaceSavePoint.h"

#include "../llvm_LinkAll.h"
#include "../NativeTypes/NativeTypes.h"

namespace dale
{
NamespaceSavePoint::NamespaceSavePoint(Namespace *ns)
{
    for (std::map<std::string, std::vector<Function *>* >::iterator
            b = ns->functions.begin(),
            e = ns->functions.end();
            b != e;
            ++b) {
        function_count.insert(
            std::pair<std::string, int>(b->first,
                                        b->second->size())
        );
    }

    variable_count = ns->variables_ordered.size();
    struct_count   = ns->structs_ordered.size();
    enum_count     = ns->enums_ordered.size();

    src_ns = ns;
}

NamespaceSavePoint::~NamespaceSavePoint(void)
{
}

bool NamespaceSavePoint::restore(void)
{
    std::map<std::string, std::vector<Function *>*>::iterator
        fb;

    for (std::map<std::string, int>::iterator
            b = function_count.begin(),
            e = function_count.end();
            b != e;
            ++b) {
        fb = src_ns->functions.find(b->first);
        assert(fb != src_ns->functions.end());
        int count = b->second;
        int to_remove = fb->second->size() - count;
        while (to_remove--) {
            if (fb->second->back()->llvm_function) {
                fb->second->back()->llvm_function->eraseFromParent();
            }
            fb->second->pop_back();
        }
    }

    int variable_diff = 
        src_ns->variables_ordered.size() - variable_count;

    while (variable_diff--) {
        src_ns->variables.erase(src_ns->variables_ordered.back());
        src_ns->variables_ordered.pop_back();
    }

    int struct_diff = 
        src_ns->structs_ordered.size() - struct_count;

    while (struct_diff--) {
        src_ns->structs.erase(src_ns->structs_ordered.back());
        src_ns->structs_ordered.pop_back();
    }

    int enum_diff = 
        src_ns->enums_ordered.size() - enum_count;

    while (enum_diff--) {
        src_ns->enums.erase(src_ns->enums_ordered.back());
        src_ns->enums_ordered.pop_back();
    }

    return true;
}
}
