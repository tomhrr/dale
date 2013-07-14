#ifndef DALE_ELEMENT_VARIABLE
#define DALE_ELEMENT_VARIABLE

#include "../Type/Type.h"

#include <string>
#include <cstdlib>

#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

namespace dale
{
namespace Element
{
class Variable
{
public:
    Element::Type *type;
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
    Variable(char *new_name, Element::Type *new_type);

    ~Variable();

    bool isEqualTo(Element::Variable *other_var);
};
}
}

#endif
