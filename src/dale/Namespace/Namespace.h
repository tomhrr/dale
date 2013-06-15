#ifndef DALE_NAMESPACE
#define DALE_NAMESPACE

#include "../Element/Function/Function.h"
#include "../Element/Variable/Variable.h"
#include "../Element/Struct/Struct.h"
#include "../Element/Enum/Enum.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Linkage/Linkage.h"
#include "../Node/Node.h"
#include "../NativeTypes/NativeTypes.h"
#include "../STLUtils/STLUtils.h"

#include <vector>
#include <string>
#include <map>

namespace dale
{

/*! Namespace

    A class for containing the details of a single namespace. Stores
    functions, variables, structs and enums for the namespace, as well
    as its own name and its parent namespace.

    Each of the bindings maps stores the 'bare' name for the binding.
    Mangled names are stored within the relevant Element, where
    necessary.

    A single namespace does not have any knowledge of its
    sub-namespaces; the tree of namespaces as a whole is managed by
    the Context class.
*/

class Namespace
{
public:
    /*! A map from function name to function list. The list is
     *  necessary because functions may be overloaded. Note that both
     *  macros and functions are stored in this map.*/
    std::map<std::string, std::vector<Element::Function *>* > functions;
    /*! A map from variable name to variable. */
    std::map<std::string, Element::Variable *> variables;
    /*! A map from struct name to struct. */
    std::map<std::string, Element::Struct *> structs;
    /*! A map from enum name to enum. */
    std::map<std::string, Element::Enum *> enums;
    /*! The error reporter for this namespace. */
    ErrorReporter *er;
    /*! The current label-variable index. This value is assigned to new
     *  labels and values when they are added to the namespace. */
    int lv_index;
    /*! The namespace name. For the root namespace, this will be a
     *  single period. Does not include any parts from the parent
     *  namespaces. For example, if the fully-qualified name is
     *  'a.b.c', this value will be 'c'. */
    std::string name;
    /*! The parent namespace. This will be NULL for the root
     *  namespace. */
    Namespace *parent_namespace;
    /*! The symbol name prefix. This is derived from the namespace's
     *  fully-qualified name. */
    std::string symbol_prefix;
    /*! A boolean indicating whether this namespace has a symbol
     *  prefix. This will be false for the root namespace, and true
     *  for all others. */
    bool has_symbol_prefix;

    /*! The void constructor, intended solely for use by the
     *  deserialisation procedures. */
    Namespace(void);
    /*! The normal constructor.
     *  @param er The error reporter.
     *  @param name The namespace name.
     *  @param parent_namespace The parent namespace object. May be NULL.
     *  @param lv_index The initial label-variable index for this namespace. */
    Namespace(ErrorReporter *er,
              std::string name,
              Namespace *parent_namespace,
              int lv_index);
    ~Namespace(void);

    /*! Add a function to the namespace.
     *  @param name The bare name of the function.
     *  @param function The function object.
     *  @param n The function's node, for error-reporting purposes.
     *
     *  This will report an error and return false when the function
     *  has the same parameter types as an existing function within
     *  this namespace.
     */
    bool addFunction(const char *name,
                     Element::Function *function,
                     Node *n);
    /*! Add a variable to the namespace.
     *  @param name The bare name of the variable.
     *  @param variable The variable object. */
    bool addVariable(const char *name, 
                     Element::Variable *variable);
    /*! Add a struct to the namespace.
     *  @param name The bare name of the struct.
     *  @param element_struct The struct object. */
    bool addStruct(const char *name, 
                   Element::Struct *element_struct);
    /*! Add an enum to the namespace.
     *  @param name The bare name of the enum.
     *  @param element_enum The enum object. */
    bool addEnum(const char *name, 
                 Element::Enum *element_enum);

    /*! Get a function from this namespace.
     *  @param name The bare name of the function.
     *  @param types The parameter types for the function.
     *  @param pclosest_fn A pointer for setting the closest candidate.
     *  @param is_macro Whether to limit searching to macros only.
     *
     *  pclosest_fn may be NULL. If it is provided, and a function
     *  matching the types cannot be found, then it will be set to a
     *  pointer to the closest possible candidate, for error-reporting
     *  purposes.
     *
     *  If is_macro is false, then both functions and macros will be
     *  taken into account. If it is true, then only macros will be
     *  taken into account. There is no way to search for functions
     *  only, because there is no instance where only functions are
     *  relevant.
     */
    Element::Function *getFunction(const char *name,
                                   std::vector<Element::Type *> *types,
                                   Element::Function **pclosest_fn,
                                   bool is_macro);
    /*! Get a variable from this namespace.
     *  @param name The variable name. */
    Element::Variable *getVariable(const char *name);
    /*! Get a struct from this namespace.
     *  @param name The struct name. */
    Element::Struct *getStruct(const char *name);
    /*! Get an enum from this namespace.
     *  @param name The enum name. */
    Element::Enum *getEnum(const char *name); 

    /*! Get all of the variables from this namespace.
     *  @param vars A vector to which the variables will be added.
     *
     *  The variables are added to the vector in reverse order. */
    void getVariables(std::vector<Element::Variable *> *vars);
    /*! Get all of the function names from this namespace.
     *  @param names A set to which the function names will be added. */
    void getFunctionNames(std::set<std::string> *names);

    /*! Get all of the variables after a certain index.
     *  @param index The lower-bound for the index.
     *  @param vars A vector to which the variables will be added.
     *
     *  'index' in this context means a label-variable index. This function
     *  recurses upwards. */
    void getVarsAfterIndex(int index,
                           std::vector<Element::Variable *> *vars);
    /*! Get all of the variables before a certain index.
     *  @param index The upper-bound for the index.
     *  @param vars A vector to which the variables will be added.
     *
     *  'index' in this context means a label-variable index. This function
     *  recurses upwards. getVarsAfterIndex and getVarsBeforeIndex are
     *  used only for determining the set of variables that need to be
     *  destructed as at a particular point. */
    void getVarsBeforeIndex(int index,
                           std::vector<Element::Variable *> *vars);

    /*! Construct a symbol name for a given name.
     *  @param name The bare name.
     *  @param new_name A string object for the symbol name.
     *
     *  This function 'mangles' the given name, by encoding the given
     *  name and adding an appropriate namespace wrapper. It should be
     *  used for all bindings other than functions.
     */
    void nameToSymbol(const char *name, std::string *new_name);
    /*! Construct a symbol name for a given function name.
     *  @param name The bare name.
     *  @param new_name A string object for the symbol name.
     *  @param linkage The function's linkage type.
     *  @param types The function's parameter types.
     *
     *  Since functions may be overloaded, symbol name determination
     *  is more complicated than for the other binding types. If the
     *  linkage denotes an extern-c function, then no mangling is
     *  performed.
     */
    void functionNameToSymbol(const char *name, std::string *new_name,
                              int linkage,
                              std::vector<Element::Variable *> *types);

    /*! Erase all LLVM macros from the namespace.
     *
     *  For all macros that contain an LLVM function, erase that
     *  function from the parent LLVM module. Note that this does not
     *  remove the function binding itself from the namespace. This is
     *  used by the Generator when it writes the 'no-macros' module.
     */
    void eraseLLVMMacros(void);
    /*! Erase all LLVM macros and CTO functions from the namespace.
     *
     *  As per eraseLLVMMacros, but also erases functions that have
     *  been marked as compile-time only.
     */
    void eraseLLVMMacrosAndCTOFunctions(void);

    /*! Set the namespace names for the current namespace.
     *  @param namespaces A vector to which the namespace names will be added.
     *
     *  For example, if the current namespace's fully-qualified name
     *  is 'a.b.c', the namespaces vector will contain the strings
     *  'a', 'b' and 'c' (in that order) after this method has been
     *  called.
     */
    bool setNamespaces(std::vector<std::string> *namespaces);

    /*! Merge another namespace into this namespace.  
     *  @param other The other namespace.
     *
     *  All extern bindings from the other namespace will be added to
     *  the current namespace. Note that the two namespaces must have
     *  the same name, but this does not check the parent namespaces'
     *  names (it's just a simple sanity check).
     */
    bool merge(Namespace *other);
    bool regetStructPointers(llvm::Module *mod);
    bool regetVariablePointers(llvm::Module *mod);
    bool regetFunctionPointers(llvm::Module *mod);
    bool regetPointers(llvm::Module *mod);

    bool eraseOnceFunctions(std::set<std::string> *once_tags,
                            llvm::Module *mod);
    bool eraseOnceVariables(std::set<std::string> *once_tags,
                            llvm::Module *mod);

    bool removeUnneededStructs(std::set<std::string> *forms,
                       std::set<std::string> *found_forms);
    bool removeUnneededVariables(std::set<std::string> *forms,
                                 std::set<std::string> *found_forms);
    bool removeUnneededFunctions(std::set<std::string> *forms,
                                 std::set<std::string> *found_forms);
    bool removeUnneededEnums(std::set<std::string> *forms,
                             std::set<std::string> *found_forms);
    bool removeUnneeded(std::set<std::string> *forms,
                        std::set<std::string> *found_forms);

    void print(void);
};
}

#endif
