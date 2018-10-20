#ifndef DALE_ELEMENT_VARIABLE
#define DALE_ELEMENT_VARIABLE

#include <cstdlib>
#include <string>

#include "../Type/Type.h"
#include "../llvm_AnalysisVerifier.h"
#include "../llvm_AssemblyPrintModulePass.h"
#include "../llvm_CallingConv.h"
#include "../llvm_Function.h"
#include "../llvm_IRBuilder.h"
#include "../llvm_Module.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/raw_ostream.h"

namespace dale {
class Function;

/*! Variable

    A class for storing the details of a variable.
*/
class Variable {
    public:
    /*! The variable's type. */
    Type *type;
    /*! The variable's name (unqualified, unmangled). */
    std::string name;
    /*! The variable's internal name (qualified, mangled). */
    std::string symbol;
    /*! The variable's value. */
    llvm::Value *value;
    /*! For a variable with one of the extern linkages, this indicates
        whether space has been set aside for it (extern variables work
        in the same way here as they do in C). */
    int has_initialiser;
    /*! The variable's once tag. */
    std::string once_tag;
    /*! The variable's index.  This is set by Namespace on variable
     *  insertion. */
    int index;
    /*! The variable's linkage. */
    int linkage;
    /*! Whether the variable should be serialised. */
    bool serialise;
    /*! The function scope the variable is in. NULL if it's a global, or
     *  function scope is not applicable. */
    Function *fn;

    Variable();
    /*! Construct a new variable with the given name and type.
     *  @param name The variable's name (unqualified, unmangled).
     *  @param type The variable's type.
     *
     *  name is copied.  type is not copied.
     */
    Variable(const char *name, Type *type);

    ~Variable();

    /*! Copy the current variable's details to another.
     *  @param var The other variable.
     */
    void copyTo(Variable *other);
    /*! Check whether a variable is equal to another variable.
     *  @param var The other variable.
     *
     *  Two variables are considered equal if their types compare
     *  equal, as per dale::Type::isEqualTo.
     */
    bool isEqualTo(Variable *other);
};
}

#endif
