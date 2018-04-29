#include "Import.h"

#include <vector>

#include "../../../Node/Node.h"
#include "../../../Units/Units.h"
#include "Config.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormTopLevelImportParse(Units *units, Node *node) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("import", node, 1, 2)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *name_node = (*lst)[1];
    name_node = units->top()->mp->parsePotentialMacroCall(name_node);
    if (!name_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("import", name_node, "1")) {
        return false;
    }
    const char *name = name_node->token->str_value.c_str();

    std::vector<const char *> import_forms;
    if (lst->size() == 3) {
        Node *forms_node = (*lst)[2];
        if (!ctx->er->assertArgIsList("import", forms_node, "2")) {
            return false;
        }
        std::vector<Node *> *forms_lst = forms_node->list;
        for (std::vector<Node *>::iterator b = forms_lst->begin(),
                                           e = forms_lst->end();
             b != e; ++b) {
            if (!ctx->er->assertArgIsAtom("import", (*b), "2")) {
                return false;
            }
            import_forms.push_back((*b)->token->str_value.c_str());
        }
    }

    bool res =
        units->mr->run(ctx, units->top()->linker, units->top()->module,
                       node, name, &import_forms);
    if (!res) {
        Error *e = new Error(UnableToLoadModule, node, name);
        ctx->er->addError(e);
    }
    return res;
}
}
