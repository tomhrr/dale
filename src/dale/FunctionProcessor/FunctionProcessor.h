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
