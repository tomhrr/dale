#ifndef DALE_ELEMENT_FUNCTION
#define DALE_ELEMENT_FUNCTION

#include "../Type/Type.h"
#include "../Variable/Variable.h"
#include "../Label/Label.h"
#include "../DeferredGoto/DeferredGoto.h"
#include <vector>

namespace dale
{
/*! Function

    A class for storing the details of a function.
*/
class Function
{
public:
    /*! The function's return type. */
    Type *return_type;
    /*! The function's parameter types.  These are stored as variables
     *  so that they may be named. */
    std::vector<Variable *> parameter_types;
    /*! The function's deferred gotos.  During parsing, if a goto
     *  instruction is found for a label that has not yet been defined,
     *  a DeferredGoto is added here. */
    std::vector<DeferredGoto *> deferred_gotos;
    /*! The function's labels.  Used for goto instruction evaluation. */
    std::map<std::string, Label *> labels;
    /*! The function's LLVM function. */
    llvm::Function *llvm_function;
    /*! The function's internal name (mangled, qualified). */
    std::string internal_name;
    /*! The function's once tag. */
    std::string once_tag;
    /*! Whether the function should always be inlined. */
    bool always_inline;
    /*! Whether the function is actually a macro. */
    bool is_macro;
    /*! Whether the function is for use only during compile time. */
    bool cto;
    /*! Whether the function is a destructor. */
    bool is_destructor;
    /*! Whether the function is a setf-overriding function. */
    bool is_setf_fn;
    /*! Whether the function should be serialised. */
    bool serialise;
    /*! The function's linkage. */
    int linkage;
    /*! The function's index.  This is set by Namespace on function
     *  insertion. */
    int index;

    Function();
    /*! Construct a new function using the given parameters.
     *  @param return_type The return type.
     *  @param parameter_types The parameter types.
     *  @param llvm_function The LLVM function.
     *  @param is_macro Whether the function is actually a macro.
     *  @param internal_name The internal name.
     *  @param always_inline Whether the function should always be inlined.
     *
     *  return_type and llvm_function are not copied.  parameter_types
     *  is copied, but the individual variables are not.
     *  internal_name is copied.
     */
    Function(Type *return_type,
             std::vector<Variable *> *parameter_types,
             llvm::Function *llvm_function,
             bool is_macro,
             std::string *internal_name,
             bool always_inline = false);
    ~Function();

    /*! Return a boolean indicating whether the function is a varargs
     *  function.
     */
    bool isVarArgs();
    /*! Return the number of arguments required by the function.
     *  (This is not the same as the size of parameter_types, due to
     *  the possibility of a varargs parameter being present in that
     *  vector.)
     */
    int numberOfRequiredArgs();
    /*! Check whether a function is equal to another function.
     *  @param fn The other function.
     *
     *  Two functions are considered equal if their return types and
     *  parameters compare equal.
     */
    bool isEqualTo(Function *fn);
    /*! Check whether a function's attributes are the same as another's.
     *  @param fn The other function.
     */
    bool attrsAreEqual(Function *fn);
    /*! Return the label with the given name.
     *  @param name The label's name.
     */
    Label *getLabel(const char *name);
    /*! Add a label with the given name.
     *  @param name The label's name.
     *  @param label The label object.
     */
    bool addLabel(const char *name, Label *label);
    /*! Check whether a function is a declaration, rather than a definition.
     */
    bool isDeclaration();
    /*! Check whether a function is a retval function.
     */
    bool hasRetval();
};
}

#endif
