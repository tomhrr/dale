#include "SavePoint.h"

#include <vector>

#include "../NativeTypes/NativeTypes.h"
#include "../llvm_LinkAll.h"

namespace dale {
SavePoint::SavePoint(Context *ctx, Function *fn,
                     llvm::BasicBlock *block) {
    block_count = fn->llvm_function->size();
    instruction_index = block->size();
    dg_count = fn->deferred_gotos.size();
    labels = fn->labels;
    this->block = block;
    this->fn = fn;
    csp = new ContextSavePoint(ctx);
}

SavePoint::~SavePoint() {
    if (csp != NULL) {
        delete csp;
    }
}

bool SavePoint::restore() {
    int block_pop_back = fn->llvm_function->size() - block_count;
    std::vector<llvm::BasicBlock *> blocks;
    while (block_pop_back--) {
        llvm::Function::iterator bi = fn->llvm_function->begin(),
                                 be = fn->llvm_function->end(), bl;
        while (bi != be) {
            bl = bi;
            ++bi;
        }
        bl->removeFromParent();
        bl->dropAllReferences();
        blocks.push_back(&*bl);
    }

    int to_pop_back = block->size() - instruction_index;
    while (to_pop_back--) {
        llvm::BasicBlock::iterator bi = block->begin(),
                                   be = block->end(), bl;
        while (bi != be) {
            bl = bi;
            ++bi;
        }
        bl->eraseFromParent();
    }

    int dg_to_pop_back = fn->deferred_gotos.size() - dg_count;
    while (dg_to_pop_back--) {
        fn->deferred_gotos.pop_back();
    }
    fn->labels = labels;

    csp->restore();
    delete csp;
    csp = NULL;

    for (std::vector<llvm::BasicBlock *>::iterator b = blocks.begin(),
                                                   e = blocks.end();
         b != e; ++b) {
        delete (*b);
    }

    return true;
}
}
