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

class Context
{
public:
    NativeTypes *nt;
    ErrorReporter *er;
    NSNode *namespaces;
    std::vector<NSNode *> active_ns_nodes;
    std::vector<NSNode *> used_ns_nodes;
    int lv_index;

    Context(void);
    Context(ErrorReporter *erep,
            NativeTypes *nt);
    ~Context(void);

    int getNextLVIndex(void);
    
    // Short, because it will be used often (get currently-active
    // namespace).
    Namespace *ns(void);
    bool popUntilNamespace(Namespace *ns);

    bool activateNamespace(const char *name);
    bool deactivateNamespace(const char *name);

    bool activateAnonymousNamespace(void);
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

    void getFunctionNames(std::set<std::string> *names);

    bool setNamespacesForStruct(const char *name,
                                std::vector<std::string> *namespaces);
    bool setNamespacesForEnum(const char *name,
                              std::vector<std::string> *namespaces);

    bool merge(Context *other);

    llvm::Type *toLLVMType(Element::Type *type,
                           Node *n,
                           bool allow_non_first_class,
                           bool externally_defined);
    llvm::Type *toLLVMType(Element::Type *type,
                           Node *n);
    llvm::Type *toLLVMTypeStruct(Element::Type *type,
                                 Node *n);
    llvm::Type *toLLVMTypeBase(Element::Type *type,
                               Node *n);
    llvm::Type *toLLVMTypeFunction(Element::Type *type,
                                   Node *n);
    llvm::Type *toLLVMTypePointer(Element::Type *type,
                                  Node *n);
    llvm::Type *toLLVMTypeArray(Element::Type *type,
                                Node *n);

    bool regetPointers(llvm::Module *mod);
    bool regetPointersForNewModule(llvm::Module *mod);
    bool rebuildFunctions(llvm::Module *mod, NSNode *node);
    bool rebuildVariables(llvm::Module *mod, NSNode *node);

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
