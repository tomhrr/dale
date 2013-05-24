#ifndef DALE_CONTEXT
#define DALE_CONTEXT

#include "../Element/Label/Label.h"
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

/* This class needs some template action for add/remove/get. Also need
 * to drop the macro stuff. */

namespace dale
{
class Context
{
public:
    NativeTypes *nt;
    std::map<std::string,
        std::vector<Element::Function *>* > *functions;
    std::map<std::string, Element::Variable *>   *variables;
    std::map<std::string, Element::Label *>      *labels;
    std::map<std::string, Element::Struct   *>   *structs;
    std::map<std::string, Element::Enum     *>   *enums;

    std::vector<std::string> *current_namespaces;

    std::vector<std::pair<std::string, Context*> > *active_namespaces;
    std::vector<std::pair<std::string, Context*> > *used_namespaces;
    std::map<std::string, Context*> *namespace_to_context;

    Context *current_context;
    Context *top_context;
    Context *parent_context;
    ErrorReporter *erep;

    const char *filename;

    int index;
    int next_sub_index;
    int lv_index;

    int getCurrentNamespaceIndex(void);

    Context(ErrorReporter *erep);
    int activateNamespace(const char *name);
    int deactivateNamespace(const char *name);

    int useNamespace(const char *name);
    int unuseNamespace(const char *name);

    int getNewName(const char *name, std::string *new_name);

    int addFunction(const char *name,
                    Element::Function *function,
                    Node *n);
    int addVariable(const char *name, Element::Variable *variable);
    int addLabel(const char *name, Element::Label *label);
    int addStruct(const char *name, Element::Struct *element_struct);
    int addEnum(const char *name, Element::Enum *element_enum);
    int removeVariable(const char *name);
    int removeLabel(const char *name);
    int removeStruct(const char *name);
    int removeEnum(const char *name);
    Element::Function *getFunction(const char *name,
                                   std::vector<Element::Type
                                   *> *types,
                                   int is_macro);
    Element::Variable *getVariable(const char *name);
    Element::Label *getLabel(const char *name);
    Element::Struct *getStruct(const char *name);
    Element::Enum *getEnum(const char *name);
    void clearLabels(void);
    int getUnusedVarname(std::string *str);
    int getUnusedLabelname(std::string *str);
    void toString(std::string *str);

    void printNamespaces(void);
    int getContextFromNamespace(const char *name, Context **ctx_ptr_ptr, int ignore_last);

    int activateAnonymousNamespace(void);
    int deactivateAnonymousNamespace(void);

    Element::Struct *getStructWithNamespaces(
        const char *name,
        std::vector<std::string> *namespaces
    );

    void setNamespacesForStruct(const char *name,
                                std::vector<std::string> *namespaces);
    void setNamespacesForEnum(const char *name,
                              std::vector<std::string> *namespaces);

    void setNamespaces(std::vector<std::string> *namespaces);
    int merge(Context *other);

    int encodeNonAN(const std::string *from, std::string *to);

    int getNewFunctionName(const char *name,
                           std::string *new_name,
                           int linkage,
                           std::vector<Element::Variable *> *params);
    void dump(void);

    llvm::Type *daleToLLVMType(Element::Type *type,
                               Node *n,
                               bool
                               allow_non_first_class,
                               bool
                               externally_defined);
    llvm::Type *daleToLLVMTypeInternal(Element::Type *type,
                                       Node *n);

    Element::Function *getFunctionBare(
        std::vector<Element::Function *> *function_list,
        std::vector<Element::Type *> *types,
        Element::Function **closest_fn,
        int is_macro
    );

    int getVariablesInCurrentScope(
        std::vector<Element::Variable *> *vars
    );

    bool existsExternCFunction(const char *name);
    bool existsNonExternCFunction(const char *name);
    bool existsNonExternCFunctionBare(std::vector<Element::Function *> *fns);
    bool existsExternCFunctionBare(std::vector<Element::Function *> *fns);

    int removeAllMacros(void);

    int isOverloadedFunctionBare(
        Element::Function *check_fn,
        std::vector<Element::Function *> *function_list
    );
    int getContextDepth(Context *myctx, int *depth);
    int getVarsAfterIndex(int index,
                          std::vector<Element::Variable
                          *> *vars);
    int getVarsBeforeIndex(int index,
                           std::vector<Element::Variable *> *vars);
    int isOverloadedFunction(const char *name);
    int regetPointers(llvm::Module *mod);
    Element::Function *getFunction(
        const char *name,
        std::vector<Element::Type *> *types,
        Element::Function **closest_fn,
        int is_macro
    );

    int getFunctionNames(
        std::set<std::string> *names
    );
    int getFunctionNamesInCurrentScope(
        std::set<std::string> *names
    );


    int popUntilNamespaceIndex(int index,
                               std::vector<std::string> *nss);

    ~Context();
};
}

#endif
