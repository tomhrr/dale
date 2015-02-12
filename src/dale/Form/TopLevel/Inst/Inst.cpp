#include "Inst.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"

#include "../Namespace/Namespace.h"
#include "../UsingNamespace/UsingNamespace.h"
#include "../Include/Include.h"
#include "../Import/Import.h"
#include "../Do/Do.h"
#include "../GlobalVariable/GlobalVariable.h"
#include "../Function/Function.h"
#include "../Struct/Struct.h"
#include "../Macro/Macro.h"
#include "../Enum/Enum.h"
#include "../Def/Def.h"
#include "../Once/Once.h"
#include "../Module/Module.h"

#define eq(str) !strcmp(t->str_value.c_str(), str)

namespace dale
{
bool
FormTopLevelInstParse(Generator *gen, Node *node)
{
    Context *ctx = gen->units->top()->ctx;

    ctx->deleteAnonymousNamespaces();
    Node *top = node;

    if (!top) {
        return false;
    }

    if (!top->is_token && !top->is_list) {
        gen->units->pop();
        if (!gen->units->empty()) {
            Unit *unit = gen->units->top();
            gen->units->top()->ctx    = unit->ctx;
            gen->current_once_tag.clear();
            gen->current_once_tag = unit->once_tag;
            return 1;
        }
        return false;
    }

    if (!top->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::OnlyListsAtTopLevel, top
        );
        ctx->er->addError(e);
        return false;
    }

    symlist *lst = top->list;

    if (lst->size() == 0) {
        Error *e = new Error(
            ErrorInst::Generator::NoEmptyLists, top
        );
        ctx->er->addError(e);
        return false;
    }

    Node *n = (*lst)[0];

    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        ctx->er->addError(e);
        return false;
    }

    Token *t = n->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol, n
        );
        ctx->er->addError(e);
        return false;
    }

    bool (*toplevel_form)(Generator *gen, Node *n);

    toplevel_form =
        (eq("do"))              ? &FormTopLevelDoParse
      : (eq("def"))             ? &FormTopLevelDefParse
      : (eq("namespace"))       ? &FormTopLevelNamespaceParse
      : (eq("using-namespace")) ? &FormTopLevelUsingNamespaceParse
      : (eq("include"))         ? &FormTopLevelIncludeParse
      : (eq("module"))          ? &FormTopLevelModuleParse
      : (eq("import"))          ? &FormTopLevelImportParse
      : (eq("once"))            ? &FormTopLevelOnceParse
                                : NULL;

    if (toplevel_form) {
        toplevel_form(gen, top);
        return true;
    }

    Node *newtop = gen->getUnit()->mp->parseOptionalMacroCall(top);
    if (!newtop) {
        return false;
    }
    if (newtop != top) {
        return FormTopLevelInstParse(gen, newtop);
    }
    Error *e = new Error(
        ErrorInst::Generator::NotInScope,
        n,
        t->str_value.c_str()
    );
    ctx->er->addError(e);
    return false;
}
}
