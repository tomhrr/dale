#include "Once.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../Linkage/Enum/Enum.h"
#include "../../Type/Type.h"
#include "../../ProcBody/ProcBody.h"

namespace dale
{
bool
FormTopLevelOnceParse(Generator *gen,
           Node *top)
{
    Context *ctx = gen->units->top()->ctx;

    if (!ctx->er->assertArgNums("once", top, 1, 1)) {
        return false;
    }
    symlist *lst = top->list;
    Node *n = (*lst)[1];
    n = gen->getUnit()->mp->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("once", n, "1")) {
        return false;
    }
    const char *once_name = n->token->str_value.c_str();
    std::string once_tag(once_name);

    if (gen->included_once_tags->find(once_tag) !=
            gen->included_once_tags->end()) {
        if (gen->units->size() == 1) {
            Error *e = new Error(
                ErrorInst::Generator::CannotOnceTheLastOpenFile,
                n
            );
            ctx->er->addError(e);
            return false;
        }
        gen->units->pop();
        Unit *unit = gen->units->top();
        gen->units->top()->ctx    = unit->ctx;
        gen->units->top()->once_tag.clear();
        gen->units->top()->once_tag = unit->once_tag;
    }
    gen->included_once_tags->insert(once_tag);
    gen->units->top()->once_tag = once_tag;
    gen->units->top()->setOnceTag(once_tag);

    return true;
}
}
