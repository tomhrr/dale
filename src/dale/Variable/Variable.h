#ifndef DALE_ELEMENT_VARIABLE
#define DALE_ELEMENT_VARIABLE

#include "../Type/Type.h"

#include <string>
#include <cstdlib>

#include "../llvm_Module.h"
#include "../llvm_Function.h"
#include "llvm/PassManager.h"
#include "../llvm_CallingConv.h"
#include "../llvm_AnalysisVerifier.h"
#include "../llvm_AssemblyPrintModulePass.h"
#include "../llvm_IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

namespace dale
{
class Variable
{
public:
    Type *type;
    std::string   name;
    std::string   internal_name;
    llvm::Value   *value;
    /* For a variable with one of the extern linkages,
     * this indicates whether space has been set aside for
     * it (extern variables work in the same way here as
     * they do in C). */
    int           has_initialiser;
    std::string once_tag;
    int index;
    int linkage;
    bool serialise;

    Variable();
    Variable(char *new_name, Type *new_type);

    ~Variable();

    bool isEqualTo(Variable *other_var);
};
}

#endif
