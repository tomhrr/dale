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
private:
    /*! The units context. */
    Units *units;

    /*! Prepare to call a retval function.
     *  @param return_type The return type of the function being called.
     *  @param block The current block.
     *  @param pr The parse result to use for the function result.
     *  @param call_args The arguments for the function.
     *
     *  This should be called before the corresponding CreateCall.  If
     *  the return type is not a retval type, this function is a
     *  no-op.
     */
    void processRetval(Type *return_type, llvm::BasicBlock *block,
                       ParseResult *pr, std::vector<llvm::Value*> *call_args);

public:
    /*! Construct a new function processor.
     *  @param units The units context.
     */
    FunctionProcessor(Units *units);
    ~FunctionProcessor();

    bool parseFuncallInternal(Function *dfn, Node *n, bool get_address,
                              ParseResult *fn_ptr, int skip,
                              std::vector<llvm::Value*> *extra_call_args,
                              ParseResult *pr);
    bool parseFunctionCall(Function *dfn, llvm::BasicBlock *block,
                           Node *n, const char *name, bool get_address,
                           bool prefixed_with_core, Function **macro_to_call,
                           ParseResult *pr);
};
}

#endif
