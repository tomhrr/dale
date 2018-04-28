#ifndef DALE_MACROPROCESSOR
#define DALE_MACROPROCESSOR

#include "../Context/Context.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"

struct PoolNode {
    void *value;
    PoolNode *next_node;
    PoolNode *last_node;
};

struct MContext {
    int arg_count;
    PoolNode *pool_node;
    void *units;
};

namespace dale {
class Units;

extern llvm::Function *pool_free_fn;
extern void (*pool_free_fptr)(MContext *);

/*! MacroProcessor

    Handles macro parsing and execution.
*/
class MacroProcessor {
    private:
    Units *units;
    Context *ctx;
    /*! Parse a macro call.
     *  @param n The node list.
     *  @param macro_to_call The macro to call.
     *
     *  parseMacroCall is a wrapper around this function: the reason
     *  for the split is that parseMacroCall will print the macro and
     *  the macro expansion, if the corresponding compilation option
     *  has been enabled and the macro was able to be expanded.
     */
    Node *parseMacroCall_(Node *n, Function *macro_to_call);

    public:
    llvm::ExecutionEngine *ee;

    /*! Construct a new macro processor.
     *  @param units The units context.
     *  @param ctx The context.
     *  @param ee The execution engine.
     */
    MacroProcessor(Units *units, Context *ctx,
                   llvm::ExecutionEngine *ee);
    ~MacroProcessor();

    /*! Parse a macro call.
     *  @param n The node list.
     *  @param macro_to_call The macro to call.
     *
     *  If macro_to_call is not provided, then the first element of n
     *  must map to an unoverloaded macro name.  The result of this
     *  function is the macro expansion.
     */
    Node *parseMacroCall(Node *n, Function *macro_to_call);
    /*! Parse a potential macro call.
     *  @param n The node list.
     *  @param once Whether to limit the number of expansions to one.
     *
     *  If the node list does not represent a valid macro call, this
     *  returns the argument node list, unchanged.  Otherwise, it
     *  operates as per parseMacroCall.
     */
    Node *parsePotentialMacroCall(Node *n, bool once = false);
    /*! Initialise the pool-free function pointer.
     *
     *  This must be done at least once before parseMacroCall is first
     *  called.
     */
    void setPoolfree();
};
}

#endif
