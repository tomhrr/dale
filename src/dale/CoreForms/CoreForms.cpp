#include "CoreForms.h"

#include <set>
#include <string>

namespace dale
{
namespace CoreForms
{
const int core_forms_max = 35;
const char *core_forms_strs[core_forms_max + 1] = {
    "goto", "label", "return", "setf", "@", ":", "#", "$",
    "get-dnodes", "p=", "p+", "p-", "p<", "p>", "def", "if", "null",
    "nullptr", "do", "cast", "va-arg", "sizeof", "offsetof",
    "va-start", "va-end",
    "alignmentof", "funcall", "using-namespace", "new-scope",
    "array-of", "setv", "@$", "@:", ":@", "@:@", NULL
};

const int core_forms_no_override_max = 31;
const char *core_forms_no_override_strs[core_forms_no_override_max + 1] = {
    "goto", "label", "return", ":", "get-dnodes", "p=", "p+", "p-", "p<",
    "p>", "def", "if", "null", "nullptr", "do", "cast", "va-arg",
    "va-start", "va-end",
    "sizeof", "offsetof", "alignmentof", "funcall", "using-namespace",
    "new-scope", "array-of",  "setv", "@$", ":@", "@:", "@:@", NULL
};

static std::set<std::string> core_forms;
static std::set<std::string> core_forms_no_override;
static bool initialised = false;

void 
init(void)
{
    if (initialised) {
        return;
    }
 
    initialised = true;

    for (int i = 0; i < core_forms_max; i++) {
        core_forms.insert(core_forms_strs[i]);
    }
    for (int i = 0; i < core_forms_no_override_max; i++) {
        core_forms_no_override.insert(core_forms_no_override_strs[i]);
    }   
}

bool 
exists(const char *name)
{
    return (core_forms.find(name) != core_forms.end());
}

bool 
existsNoOverride(const char *name)
{
    return (core_forms_no_override.find(name) != core_forms_no_override.end());
}
}
}
