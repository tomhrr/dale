#include "Label.h"

namespace dale {
Label::Label(llvm::BasicBlock *block, Namespace *ns, int index) {
    this->index = index;
    this->block = block;
    this->ns = ns;
}

Label::~Label() {}
}
