#include "CoreForms.h"

#include <map>
#include <set>
#include <string>
#include <utility>

#include "../Form/Macro/ArrayDeref/ArrayDeref.h"
#include "../Form/Macro/DerefStruct/DerefStruct.h"
#include "../Form/Macro/DerefStructDeref/DerefStructDeref.h"
#include "../Form/Macro/Setv/Setv.h"
#include "../Form/Macro/StructDeref/StructDeref.h"
#include "../Form/Proc/AddressOf/AddressOf.h"
#include "../Form/Proc/Alignmentof/Alignmentof.h"
#include "../Form/Proc/Aref/Aref.h"
#include "../Form/Proc/ArrayOf/ArrayOf.h"
#include "../Form/Proc/Cast/Cast.h"
#include "../Form/Proc/Def/Def.h"
#include "../Form/Proc/Dereference/Dereference.h"
#include "../Form/Proc/Do/Do.h"
#include "../Form/Proc/Funcall/Funcall.h"
#include "../Form/Proc/Goto/Goto.h"
#include "../Form/Proc/If/If.h"
#include "../Form/Proc/Include/Include.h"
#include "../Form/Proc/Label/Label.h"
#include "../Form/Proc/Move/Move.h"
#include "../Form/Proc/NewScope/NewScope.h"
#include "../Form/Proc/Null/Null.h"
#include "../Form/Proc/NullPtr/NullPtr.h"
#include "../Form/Proc/Offsetof/Offsetof.h"
#include "../Form/Proc/PtrAdd/PtrAdd.h"
#include "../Form/Proc/PtrEquals/PtrEquals.h"
#include "../Form/Proc/PtrGreaterThan/PtrGreaterThan.h"
#include "../Form/Proc/PtrLessThan/PtrLessThan.h"
#include "../Form/Proc/PtrSubtract/PtrSubtract.h"
#include "../Form/Proc/Quote/Quote.h"
#include "../Form/Proc/Return/Return.h"
#include "../Form/Proc/Setf/Setf.h"
#include "../Form/Proc/Sizeof/Sizeof.h"
#include "../Form/Proc/Sref/Sref.h"
#include "../Form/Proc/UsingNamespace/UsingNamespace.h"
#include "../Form/Proc/VaArg/VaArg.h"
#include "../Form/Proc/VaEnd/VaEnd.h"
#include "../Form/Proc/VaStart/VaStart.h"
#include "../Form/TopLevel/Def/Def.h"
#include "../Form/TopLevel/Do/Do.h"
#include "../Form/TopLevel/Import/Import.h"
#include "../Form/TopLevel/Include/Include.h"
#include "../Form/TopLevel/Module/Module.h"
#include "../Form/TopLevel/Namespace/Namespace.h"
#include "../Form/TopLevel/UsingNamespace/UsingNamespace.h"

#define ADD_SC(a, b)            \
    standard_core_forms.insert( \
        std::pair<std::string, standard_core_form_t>(a, b));
#define ADD_MC(a, b)         \
    macro_core_forms.insert( \
        std::pair<std::string, macro_core_form_t>(a, b));
#define ADD_TC(a, b)            \
    toplevel_core_forms.insert( \
        std::pair<std::string, toplevel_core_form_t>(a, b));

namespace dale {
namespace CoreForms {
std::map<std::string, standard_core_form_t> standard_core_forms;
std::map<std::string, macro_core_form_t> macro_core_forms;
std::map<std::string, toplevel_core_form_t> toplevel_core_forms;
std::set<std::string> core_forms_no_override;

const int core_forms_no_override_max = 32;
const char
    *core_forms_no_override_strs[core_forms_no_override_max + 1] = {
        "goto",      "label",       "return",  ":",
        "q",         "p=",          "p+",      "p-",
        "p<",        "p>",          "def",     "if",
        "null",      "nullptr",     "do",      "cast",
        "va-arg",    "va-start",    "va-end",  "sizeof",
        "offsetof",  "alignmentof", "funcall", "using-namespace",
        "new-scope", "array-of",    "setv",    "@$",
        ":@",        "@:",          "@:@",     "move",
        NULL};

static bool initialised = false;

void init() {
    if (initialised) {
        return;
    }

    initialised = true;

    ADD_SC("goto", &FormProcGotoParse);
    ADD_SC("if", &FormProcIfParse);
    ADD_SC("label", &FormProcLabelParse);
    ADD_SC("return", &FormProcReturnParse);
    ADD_SC("setf", &FormProcSetfParse);
    ADD_SC("@", &FormProcDereferenceParse);
    ADD_SC(":", &FormProcSrefParse);
    ADD_SC("#", &FormProcAddressOfParse);
    ADD_SC("$", &FormProcArefParse);
    ADD_SC("p=", &FormProcPtrEqualsParse);
    ADD_SC("p+", &FormProcPtrAddParse);
    ADD_SC("p-", &FormProcPtrSubtractParse);
    ADD_SC("p<", &FormProcPtrLessThanParse);
    ADD_SC("p>", &FormProcPtrGreaterThanParse);
    ADD_SC("va-arg", &FormProcVaArgParse);
    ADD_SC("va-start", &FormProcVaStartParse);
    ADD_SC("va-end", &FormProcVaEndParse);
    ADD_SC("null", &FormProcNullParse);
    ADD_SC("q", &FormProcQuoteParse);
    ADD_SC("def", &FormProcDefParse);
    ADD_SC("nullptr", &FormProcNullPtrParse);
    ADD_SC("do", &FormProcDoParse);
    ADD_SC("cast", &FormProcCastParse);
    ADD_SC("sizeof", &FormProcSizeofParse);
    ADD_SC("offsetof", &FormProcOffsetOfParse);
    ADD_SC("alignmentof", &FormProcAlignmentOfParse);
    ADD_SC("funcall", &FormProcFuncallParse);
    ADD_SC("using-namespace", &FormProcUsingNamespaceParse);
    ADD_SC("new-scope", &FormProcNewScopeParse);
    ADD_SC("array-of", &FormProcArrayOfParse);
    ADD_SC("move", &FormProcMoveParse);
    ADD_SC("include", &FormProcIncludeParse);

    ADD_MC("setv", &FormMacroSetvParse);
    ADD_MC("@$", &FormMacroArrayDerefParse);
    ADD_MC(":@", &FormMacroDerefStructParse);
    ADD_MC("@:", &FormMacroStructDerefParse);
    ADD_MC("@:@", &FormMacroDerefStructDerefParse);

    ADD_TC("do", &FormTopLevelDoParse);
    ADD_TC("def", &FormTopLevelDefParse);
    ADD_TC("namespace", &FormTopLevelNamespaceParse);
    ADD_TC("using-namespace", &FormTopLevelUsingNamespaceParse);
    ADD_TC("include", &FormTopLevelIncludeParse);
    ADD_TC("module", &FormTopLevelModuleParse);
    ADD_TC("import", &FormTopLevelImportParse);

    for (int i = 0; i < core_forms_no_override_max; i++) {
        core_forms_no_override.insert(core_forms_no_override_strs[i]);
    }
}

bool exists(const char *name) {
    return (
        (standard_core_forms.find(name) != standard_core_forms.end()) ||
        (macro_core_forms.find(name) != macro_core_forms.end()));
}

bool existsNoOverride(const char *name) {
    return (core_forms_no_override.find(name) !=
            core_forms_no_override.end());
}

standard_core_form_t getStandard(const char *name) {
    std::map<std::string, standard_core_form_t>::iterator b =
        standard_core_forms.find(name);
    if (b == standard_core_forms.end()) {
        return NULL;
    }
    return b->second;
}

macro_core_form_t getMacro(const char *name) {
    std::map<std::string, macro_core_form_t>::iterator b =
        macro_core_forms.find(name);
    if (b == macro_core_forms.end()) {
        return NULL;
    }
    return b->second;
}

toplevel_core_form_t getTopLevel(const char *name) {
    std::map<std::string, toplevel_core_form_t>::iterator b =
        toplevel_core_forms.find(name);
    if (b == toplevel_core_forms.end()) {
        return NULL;
    }
    return b->second;
}
}
}
