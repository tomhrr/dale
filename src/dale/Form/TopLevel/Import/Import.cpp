#include "Import.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"

namespace dale
{
bool
FormTopLevelImportParse(Generator *gen, Node *node)
{
    Context *ctx = gen->ctx;
    Node *top = node;

    assert(top->list && "parseImport must receive a list!");

    if (!ctx->er->assertArgNums("import", top, 1, 2)) {
        return false;
    }

    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = gen->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("import", n, "1")) {
        return false;
    }

    const char *my_module_name = n->token->str_value.c_str();

    std::vector<const char *> import_forms;
    if (lst->size() == 3) {
        n = (*lst)[2];
        if (!ctx->er->assertArgIsList("import", n, "2")) {
            return false;
        }
        symlist *formlst = n->list;
        for (symlist::iterator b = formlst->begin(),
                e = formlst->end();
                b != e;
                ++b) {
            if (!ctx->er->assertArgIsAtom("import", (*b), "2")) {
                return false;
            }
            import_forms.push_back((*b)->token->str_value.c_str());
        }
    }

    int res = gen->addDaleModule(top, my_module_name, &import_forms);
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::UnableToLoadModule,
            top,
            my_module_name
        );
        ctx->er->addError(e);
        return false;
    }

    return true;
}
}
