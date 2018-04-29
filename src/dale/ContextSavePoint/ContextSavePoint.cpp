#include "ContextSavePoint.h"

#include <map>
#include <string>
#include <utility>

#include "../NativeTypes/NativeTypes.h"
#include "../llvm_LinkAll.h"

namespace dale {
void makeNamespaceSavePoints(NSNode *nsnode, SPNode *spnode) {
    spnode->nsp = new NamespaceSavePoint(nsnode->ns);

    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e; ++b) {
        SPNode *sub_nsp = new SPNode();
        makeNamespaceSavePoints(b->second, sub_nsp);
        spnode->children.insert(
            std::pair<std::string, SPNode *>(b->first, sub_nsp));
    }

    return;
}

ContextSavePoint::ContextSavePoint(Context *ctx) {
    active = ctx->active_ns_nodes.back();
    nsps = new SPNode();
    makeNamespaceSavePoints(active, nsps);
    src_ctx = ctx;
    active_count = src_ctx->active_ns_nodes.size();
    used_count = src_ctx->used_ns_nodes.size();
}

ContextSavePoint::~ContextSavePoint() {}

bool restore_(SPNode *spnode) {
    for (std::map<std::string, SPNode *>::iterator
             b = spnode->children.begin(),
             e = spnode->children.end();
         b != e; ++b) {
        restore_(b->second);
    }
    spnode->nsp->restore();

    return true;
}

bool eraseUnneeded(Context *ctx, SPNode *spnode, NSNode *nsnode) {
    for (std::map<std::string, NSNode *>::iterator
             b = nsnode->children.begin(),
             e = nsnode->children.end();
         b != e;) {
        std::map<std::string, SPNode *>::iterator spb =
            spnode->children.find(b->first);
        if (spb == spnode->children.end()) {
            ctx->deleteNamespaces(b->second);
            std::map<std::string, NSNode *>::iterator b_copy = b;
            ++b_copy;
            nsnode->children.erase(b);
            b = b_copy;
        } else {
            eraseUnneeded(ctx, spb->second, b->second);
            ++b;
        }
    }

    return true;
}

bool ContextSavePoint::restore() {
    int current_active_count = src_ctx->active_ns_nodes.size();
    int current_used_count = src_ctx->used_ns_nodes.size();
    while (current_active_count > active_count) {
        src_ctx->active_ns_nodes.pop_back();
        current_active_count--;
    }
    while (current_used_count > used_count) {
        src_ctx->used_ns_nodes.pop_back();
        current_used_count--;
    }

    restore_(nsps);
    eraseUnneeded(src_ctx, nsps, active);
    return true;
}
}
