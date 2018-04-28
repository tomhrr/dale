#include "Include.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../../Function/Function.h"
#include "../../../Lexer/Lexer.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Destruct/Destruct.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Parser/Parser.h"
#include "../../../Units/Units.h"
#include "../../../llvm_Function.h"
#include "../Inst/Inst.h"

using namespace dale::ErrorInst;

namespace dale {
bool FormProcIncludeParse(Units *units, Function *fn,
                          llvm::BasicBlock *block, Node *node,
                          bool get_address, bool prefixed_with_core,
                          ParseResult *pr) {
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
    if (!ctx->er->assertAtomIsStringLiteral("include", path_node,
                                            "1")) {
        return false;
    }

    std::string path_buf;

    FILE *include_file = NULL;
    for (std::vector<const char *>::iterator
             b = units->mr->include_directory_paths.begin(),
             e = units->mr->include_directory_paths.end();
         b != e; ++b) {
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
    }

    Lexer *lxr = new Lexer(include_file);
    Parser parser(lxr, ctx->er, strdup(path_buf.c_str()));

    int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);
    std::vector<Node *> *do_nodes = new std::vector<Node *>();
    Token *do_token = new Token(TokenType::String);
    do_token->str_value.append("do");
    do_nodes->push_back(new Node(do_token));
    for (;;) {
        Node *new_top = parser.getNextList();
        if (!new_top) {
            break;
        }
        if (!new_top->is_token && !new_top->is_list) {
            break;
        }
        do_nodes->push_back(new_top);
    }
    if (ctx->er->getErrorTypeCount(ErrorType::Error) > error_count) {
        return false;
    }
    Node *wrapper = new Node(do_nodes);

    return FormProcInstParse(units, fn, block, wrapper, get_address,
                             false, NULL, pr);
}
}
