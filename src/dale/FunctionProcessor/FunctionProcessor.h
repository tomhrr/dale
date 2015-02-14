#ifndef DALE_ELEMENT_FUNCTIONPROCESSOR
#define DALE_ELEMENT_FUNCTIONPROCESSOR

#include "../Function/Function.h"
#include "../ParseResult/ParseResult.h"

namespace dale
{
class Units;

/*! FunctionProcessor

    Handles function execution.
*/
class FunctionProcessor
{
public:
    Units *units;

    FunctionProcessor(Units *units);
    ~FunctionProcessor();

    bool parseFuncallInternal(
    Function *dfn,
    Node *n,
    bool getAddress,
    ParseResult *fn_ptr,
    int skip,
    std::vector<llvm::Value*> *extra_call_args,
    ParseResult *pr);

    void processRetval(Type *return_type,
                              llvm::BasicBlock *block,
                              ParseResult *pr,
                              std::vector<llvm::Value*> *call_args);

    bool parseFunctionCall(Function *dfn,
            llvm::BasicBlock *block,
            Node *n,
            const char *name,
            bool getAddress,
            bool prefixed_with_core,
            Function **macro_to_call,
            ParseResult *pr);
};
}

#endif
