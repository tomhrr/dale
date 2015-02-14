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
public:
    Units *units;
    Context *ctx;
    llvm::ExecutionEngine *ee; 
    
    MacroProcessor(Units *units, Context *ctx, llvm::ExecutionEngine* ee);
    ~MacroProcessor();

    Node* parseMacroCall(Node *n,
                                const char *name,
                                Function *macro_to_call);
    Node * parseOptionalMacroCall(Node *n);
    void setPoolfree(void);
};
}

#endif
