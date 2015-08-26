#include "Include.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../CommonDecl/CommonDecl.h"
#include "../../../Node/Node.h"

#include <cstdio>

using namespace dale::ErrorInst;

namespace dale
{
bool
FormTopLevelIncludeParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("include", node, 1, 1)) {
        return false;
    }

    std::vector<Node *> *lst = node->list;
    Node *path_node = (*lst)[1];
    path_node = units->top()->mp->parsePotentialMacroCall(path_node);
    if (!path_node) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("include", path_node, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsStringLiteral("include", path_node, "1")) {
        return false;
    }

    std::string path_buf;

    FILE *include_file = NULL;
    for (std::vector<const char *>::iterator
                b = units->mr->include_directory_paths.begin(),
                e = units->mr->include_directory_paths.end();
            b != e;
            ++b) {
        path_buf.clear();
        path_buf.append((*b));
        if (*(path_buf.rbegin()) != '/') {
            path_buf.push_back('/');
        }
        path_buf.append(path_node->token->str_value.c_str());
        include_file = fopen(path_buf.c_str(), "r");
        if (include_file) {
            break;
        }
    }

    if (!include_file) {
        Error *e = new Error(FileError, path_node, path_buf.c_str(),
                             strerror(errno));
        ctx->er->addError(e);
        return false;
    } else {
        int res = fclose(include_file);
        if (res != 0) {
            error("unable to close include file %s", path_buf.c_str(), true);
        }
    }

    Unit *unit = new Unit(path_buf.c_str(), units, ctx->er, ctx->nt,
                          ctx->tr, units->top()->ee,
                          units->top()->is_x86_64);
    units->push(unit);
    units->top()->once_tag.clear();

    units->top()->ee->addModule(units->top()->module);
    CommonDecl::addVarargsFunctions(unit);

    if (!units->no_common) {
        if (units->no_dale_stdlib) {
            units->top()->addCommonDeclarations();
        } else {
            std::vector<const char*> import_forms;
            units->mr->run(ctx, units->top()->linker,
                           units->top()->module, nullNode(), "drt",
                           &import_forms);
        }
    }

    units->top()->ctx->regetPointers(units->top()->module);

    return true;
}
}
