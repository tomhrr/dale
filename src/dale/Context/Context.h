#ifndef DALE_CONTEXT
#define DALE_CONTEXT

#include "../Namespace/Namespace.h"
#include "../Linkage/Linkage.h"
#include "../Element/Function/Function.h"
#include "../Element/Variable/Variable.h"
#include "../Element/Struct/Struct.h"
#include "../Element/Enum/Enum.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Node/Node.h"
#include "../NativeTypes/NativeTypes.h"
#include "../TypeRegister/TypeRegister.h"
#include "../STLUtils/STLUtils.h"

#include <vector>
#include <string>
#include <map>

namespace dale
{
struct NSNode
{
    Namespace *ns;
    std::map<std::string, NSNode *> children;
};

/*! Context

    A class for containing the details of a tree of namespaces.
    Provides for activating/deactivating namespaces (named and
    anonymous), searching for bindings throughout the tree and
    constructing LLVM types from 'internal' types, amongst other
    things.

    Within this class, an 'active' namespace is one into which new
    bindings will be put, whereas a 'used' namespace is one that is
    only used for binding resolution. Note that all 'active'
    namespaces are also 'used' namespaces, for so long as they are
    active.
*/
class Context
{
public:
    /*! The native types for the context. Used primarily for type
     *  conversion. */
    NativeTypes *nt;
    /*! The type register for the context. */
    TypeRegister *tr;
    /*! The error reporter for this context. */
    ErrorReporter *er;
    /*! The node for the root namespace. */
    NSNode *namespaces;
    /*! The list of the currently-active nodes, with the most
     *  recently-activated node being at the back. */
    std::vector<NSNode *> active_ns_nodes;
    /*! The list of the currently-used nodes, with the most
     *  recently-used node being at the back. */
    std::vector<NSNode *> used_ns_nodes;
    /*! The current label-variable index for the context. */
    int lv_index;

    /*! The void constructor, intended solely for use by the
     *  deserialisation procedures. */
    Context(void);
    /*! The normal constructor.
     *  @param er The error reporter.
     *  @param nt The native types. */
    Context(ErrorReporter *er,
            NativeTypes *nt,
            TypeRegister *tr);
    ~Context(void);
 
    /*! Get the currently-active namespace. */
    Namespace *ns(void);
    /*! Pop active/used namespaces until the given namespace has been reached.
     *  @param ns The namespace that needs to be reached. */
    bool popUntilNamespace(Namespace *ns);

    /*! Activate the namespace with the given name.
     *  @param name The name of the namespace.
     * 
     *  If the namespace does not already exist, then a new namespace
     *  object will be created and added to the tree.
     */
    bool activateNamespace(const char *name);
    /*! Deactivate the namespace with the given name.
     *  @param name The name of the namespace.
     *
     *  Reports errors and returns false if the namespace is not the
     *  most recently activated and used namespace.
     */
    bool deactivateNamespace(const char *name);

    /*! Activate a new anonymous namespace. */
    bool activateAnonymousNamespace(void);
    /*! Deactivate an anonymous namespace.
     *
     *  The only difference between this method and
     *  deactivateNamespace is that the latter checks that the name of
     *  the namespace is 'correct'; i.e., deactivateNamespace may be
     *  used directly, as the pair to a corresponding
     *  activateAnonymousNamespace call, by fetching the namespace's
     *  name manually after activation. */
    bool deactivateAnonymousNamespace(void);

    NSNode *getNSNode(const char *name, 
                      bool ignore_last,
                      std::vector<std::string> *ns_parts);
    NSNode *getNSNode(const char *name, 
                      bool ignore_last);
    Namespace *getNamespace(const char *name,
                            bool ignore_last);

    bool useNamespace(const char *name);
    bool unuseNamespace(void);

    void eraseLLVMMacros(void);
    void eraseLLVMMacrosAndCTOFunctions(void);

    bool existsExternCFunction(const char *name);
    bool existsNonExternCFunction(const char *name);
    bool isOverloadedFunction(const char *name);

    Element::Function *getFunction(const char *name,
                                   std::vector<Element::Type *> *types,
                                   Element::Function **closest_fn,
                                   bool is_macro);
    Element::Function *getFunction(const char *name,
                                   std::vector<Element::Type *> *types,
                                   bool is_macro);
    Element::Variable *getVariable(const char *name);
    Element::Struct *getStruct(const char *name);
    Element::Struct *getStruct(const char *name,
                               std::vector<std::string> *namespaces);
    Element::Enum *getEnum(const char *name);

    void getFunctionNames(std::set<std::string> *names,
                          std::string *prefix);

    bool setNamespacesForStruct(const char *name,
                                std::vector<std::string> *namespaces);
    bool setNamespacesForEnum(const char *name,
                              std::vector<std::string> *namespaces);
    bool setFullyQualifiedStructName(const char *name,
                                     std::string *fqsn);

    bool merge(Context *other);

    llvm::Type *toLLVMType_(Element::Type *type,
                            Node *n,
                            bool refs_to_pointers);
    llvm::Type *toLLVMType(Element::Type *type,
                           Node *n,
                           bool allow_non_first_class = true,
                           bool externally_defined = false,
                           bool refs_to_pointers = false);
    llvm::Type *toLLVMTypeStruct(Element::Type *type,
                                 Node *n);
    llvm::Type *toLLVMTypeBase(Element::Type *type,
                               Node *n);
    llvm::Type *toLLVMTypeFunction(Element::Type *type,
                                   Node *n,
                                   bool refs_to_pointers);
    llvm::Type *toLLVMTypePointer(Element::Type *type,
                                  Node *n,
                                  bool refs_to_pointers);
    llvm::Type *toLLVMTypeArray(Element::Type *type,
                                Node *n);
    llvm::GlobalValue::LinkageTypes toLLVMLinkage(int linkage);

    bool regetPointers(llvm::Module *mod);
    bool regetPointersForNewModule(llvm::Module *mod);
    bool rebuildFunctions(llvm::Module *mod, NSNode *node);
    bool rebuildVariables(llvm::Module *mod, NSNode *node);

    bool removeDeserialised(void);
    bool removeUnneeded(std::set<std::string> *forms,
                        std::set<std::string> *found_forms);

    bool eraseOnceForms(std::set<std::string> *once_tags,
                        llvm::Module *mod);
    bool deleteAnonymousNamespaces(void);
    void deleteNamespaces(NSNode *nsnode);

    void relink(void);

    void print(void);
};
}

#endif
