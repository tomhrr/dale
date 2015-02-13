#include "Include.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../CommonDecl/CommonDecl.h"
#include "../../../Node/Node.h"

namespace dale
{
bool
FormTopLevelIncludeParse(Generator *gen, Node *node)
{
    Context *ctx = gen->units->top()->ctx;

    if (!ctx->er->assertArgNums("include", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;
    Node *n = (*lst)[1];
    n = gen->getUnit()->mp->parseOptionalMacroCall(n);
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
                gen->units->mr->include_directory_paths;
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

    Unit *unit = new Unit(filename_buf.c_str(), gen, ctx->er, ctx->nt,
                          ctx->tr, gen->getUnit()->ee,
                          gen->units->top()->is_x86_64);
    gen->units->push(unit);
    gen->units->top()->once_tag.clear();

    gen->units->top()->ee->addModule(gen->units->top()->module);
    CommonDecl::addVarargsFunctions(unit);

    if (!gen->no_add_common_declarations) {
        if (gen->no_drt) {
            gen->getUnit()->addCommonDeclarations();
        } else {
            std::vector<const char*> import_forms;
            gen->units->mr->run(ctx, gen->units->top()->module, nullNode(), "drt", &import_forms);
        }
    }

    gen->units->top()->ctx->regetPointers(gen->units->top()->module);

    return true;
}
}
