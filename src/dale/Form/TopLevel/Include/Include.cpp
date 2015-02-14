#include "Include.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../CommonDecl/CommonDecl.h"
#include "../../../Node/Node.h"

namespace dale
{
bool
FormTopLevelIncludeParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("include", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;
    Node *n = (*lst)[1];
    n = units->top()->mp->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!ctx->er->assertArgIsAtom("include", n, "1")) {
        return false;
    }
    if (!ctx->er->assertAtomIsStringLiteral("include", n, "1")) {
        return false;
    }

    Token *t = n->token;

    std::string filename_buf(t->str_value.c_str());

    /* Check if the file exists in the current directory, or in
     * ./include.  If it doesn't, go through each of the -I
     * (include_directory_paths) directories.  If it doesn't exist in
     * any of them, check DALE_INCLUDE_PATH (set at compile time -
     * used to be an environment variable).  Otherwise, print an error
     * and return nothing. */

    FILE *include_file = fopen(filename_buf.c_str(), "r");

    if (!include_file) {
        filename_buf.clear();
        filename_buf.append("./include/");
        filename_buf.append(t->str_value.c_str());
        include_file = fopen(filename_buf.c_str(), "r");
        if (!include_file) {
            std::vector<const char *> *idp =
                units->mr->include_directory_paths;
            for (std::vector<const char*>::iterator b = idp->begin(),
                                                    e = idp->end();
                    b != e;
                    ++b) {
                filename_buf.clear();
                filename_buf.append(*b)
                    .append("/")
                    .append(t->str_value.c_str());
                include_file = fopen(filename_buf.c_str(), "r");
                if (include_file) {
                    break;
                }
            }
        }
        if (!include_file) {
            filename_buf.clear();
            filename_buf.append(DALE_INCLUDE_PATH)
            .append("/")
            .append(t->str_value.c_str());

            include_file = fopen(filename_buf.c_str(), "r");

            if (!include_file) {
                Error *e = new Error(
                    ErrorInst::Generator::FileError,
                    n,
                    filename_buf.c_str(),
                    strerror(errno)
                );
                ctx->er->addError(e);
                return false;
            }
        }
    }
    /* Add the current parser/module/context to their respective
     * stacks, create new parser/module/context for the new file.
     * */

    Unit *unit = new Unit(filename_buf.c_str(), units, ctx->er, ctx->nt,
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
            units->mr->run(ctx, units->top()->module, nullNode(), "drt", &import_forms);
        }
    }

    units->top()->ctx->regetPointers(units->top()->module);

    return true;
}
}
