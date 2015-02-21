#include "Module.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"

namespace dale
{
static int 
isValidModuleName(std::string *str)
{
    int i;
    for (i = 0; i < (int) str->length(); ++i) {
        char c = (*str)[i];
        if (!(isalnum(c) || (c == '-') || (c == '_') || (c == '.'))) {
            return 0;
        }
    }
    return 1;
}

bool
FormTopLevelModuleParse(Units *units, Node *node)
{
    Node *top = node;
    Context *ctx = units->top()->ctx;

    if (units->module_name.size() > 0) {
        fprintf(stderr, "Internal error: module name already set.\n");
        abort();
    }

    if (!ctx->er->assertArgNums("module", top, 1, 2)) {
        return false;
    }

    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = units->top()->mp->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("module", n, "1")) {
        return false;
    }

    if (!isValidModuleName(&(n->token->str_value))) {
        Error *e = new Error(
            ErrorInst::Generator::InvalidModuleName,
            n,
            n->token->str_value.c_str()
        );
        ctx->er->addError(e);
        return false;
    }

    const char *my_module_name = n->token->str_value.c_str();

    if (lst->size() == 3) {
        n = (*lst)[2];
        n = units->top()->mp->parseOptionalMacroCall(n);
        if (!n) {
            return false;
        }
        if (!ctx->er->assertArgIsList("module", n, "2")) {
            return false;
        }
        if (!(n->list->at(0)->is_token)
                ||
                (n->list->at(0)->token->str_value.compare("attr"))) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                n,
                "attr",
                0,
                0
            );
            ctx->er->addError(e);
            return false;
        }

        symlist *attr_list = n->list;
        std::vector<Node*>::iterator b = attr_list->begin(),
                                     e = attr_list->end();
        ++b;
        for (; b != e; ++b) {
            if ((*b)->is_list) {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                ctx->er->addError(e);
                return false;
            }
            if (!((*b)->token->str_value.compare("cto"))) {
                units->cto = true;
            } else {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                ctx->er->addError(e);
                return false;
            }
        }
    }

    units->module_name = std::string("lib");
    units->module_name.append(my_module_name);

    return true;
}
}
