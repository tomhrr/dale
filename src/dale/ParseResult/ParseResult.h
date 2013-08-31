#ifndef DALE_PARSERESULT
#define DALE_PARSERESULT

#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "../Element/Type/Type.h"

namespace dale
{
class ParseResult
{
public:
    Element::Type *type;
    llvm::BasicBlock *block;
    llvm::Value *value;
    int treat_as_terminator;
    int do_not_destruct;
    int do_not_copy_with_setf;
    int freshly_copied;

    ParseResult();
    ParseResult(llvm::BasicBlock *new_block,
                Element::Type *new_type,
                llvm::Value *new_value);
    ~ParseResult();

    int copyTo(ParseResult *x);
    void set(llvm::BasicBlock *new_block,
            Element::Type *new_type,
            llvm::Value *new_value);
};
}

#endif
