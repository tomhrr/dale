#ifndef DALE_MACROPROCESSOR
#define DALE_MACROPROCESSOR

#include "../Context/Context.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"

struct PoolNode
{
    void     *value;
    PoolNode *next_node;
    PoolNode *last_node;
};

struct MContext
{
    int      arg_count;
    PoolNode *pool_node;
    void     *units;
};

namespace dale
{
class Units;

extern llvm::Function *pool_free_fn;
extern void (*pool_free_fptr)(MContext *);

/*! MacroProcessor

    Handles macro parsing and execution.
*/
class MacroProcessor
{
private:
    Units *units;
    Context *ctx;

public:
    llvm::ExecutionEngine *ee; 

    /*! Construct a new macro processor.
     *  @param units The units context.
     *  @param ctx The context.
     *  @param ee The execution engine.
     */
    MacroProcessor(Units *units, Context *ctx, llvm::ExecutionEngine* ee);
    ~MacroProcessor();

    /*! Parse a macro call.
     *  @param n The node list.
     *  @param macro_to_call The macro to call.
     *
     *  If macro_to_call is not provided, then the first element of n
     *  must map to an unoverloaded macro name.  The result of this
     *  function is the macro expansion.
     */
    Node* parseMacroCall(Node *n, Function *macro_to_call);
    /*! Parse a potential macro call.
     *  @param n The node list.
     *
     *  If the node list does not represent a valid macro call, this
     *  returns the argument node list, unchanged.  Otherwise, it
     *  operates as per parseMacroCall.
     */
    Node* parsePotentialMacroCall(Node *n);
    /*! Initialise the pool-free function pointer.
     *
     *  This must be done at least once before parseMacroCall is first
     *  called.
     */
    void setPoolfree();
};
}

#endif
