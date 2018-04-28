#ifndef DALE_ELEMENT_FUNCTIONPROCESSOR
#define DALE_ELEMENT_FUNCTIONPROCESSOR

#include <vector>

#include "../Function/Function.h"
#include "../ParseResult/ParseResult.h"

namespace dale {
class Units;

/*! FunctionProcessor

    Handles function execution.
*/
class FunctionProcessor {
    private:
    /*! The units context. */
    Units *units;

    public:
    /*! Construct a new function processor.
     *  @param units The units context.
     */
    explicit FunctionProcessor(Units *units);
    ~FunctionProcessor();

    /*! Parse a function pointer call.
     *  @param fn The function in which the FP call appears.
     *  @param n The argument node list.
     *  @param fn_ptr The parse result containing the FP's details.
     *  @param skip The number of nodes to be skipped in the argument
     * list.
     *  @param extra_call_args Additional arguments to append to the
     * call.
     *  @param pr The parse result for the returned value.
     */
    bool parseFunctionPointerCall(
        Function *fn, Node *n, ParseResult *fn_ptr, int skip,
        std::vector<llvm::Value *> *extra_call_args, ParseResult *pr);
    /*! Parse a function call.
     *  @param fn The function in which the function call appears.
     *  @param block The current block.
     *  @param n The node list (including the function name).
     *  @param name The name of the function.
     *  @param get_address Whether the address of the result is
     * required.
     *  @param macro_to_call Storage for a potential macro result.
     *  @param pr The parse result for the returned value.
     *
     *  Although this only handles function execution, it is
     *  also responsible for determining macro candidates: if a macro
     *  is to be called, then it will be stored in macro_to_call and
     *  the function will return false.
     */
    bool parseFunctionCall(Function *fn, llvm::BasicBlock *block,
                           Node *n, const char *name, bool get_address,
                           Function **macro_to_call, ParseResult *pr);
};
}

#endif
