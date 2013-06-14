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
class Namespace
{
public:
    std::map<std::string, std::vector<Element::Function *>* > functions;
    std::map<std::string, Element::Variable *> variables;
    std::map<std::string, Element::Struct   *> structs;
    std::map<std::string, Element::Enum     *> enums;
    ErrorReporter *er;
    int lv_index;
    std::string name;
    std::string symbol_prefix;
    bool has_symbol_prefix;

    Namespace(void);
    Namespace(ErrorReporter *er,
              std::string name,
              Namespace *parent_namespace,
              int lv_index);
    ~Namespace(void);

    Namespace *parent_namespace;

    bool addFunction(const char *name,
                     Element::Function *function,
                     Node *n);
    bool addVariable(const char *name, 
                     Element::Variable *variable);
    bool addStruct(const char *name, 
                   Element::Struct *element_struct);
    bool addEnum(const char *name, 
                 Element::Enum *element_enum);

    Element::Function *getFunction(const char *name,
                                   std::vector<Element::Type *> *types,
                                   Element::Function **pclosest_fn,
                                   bool is_macro);
    Element::Variable *getVariable(const char *name);
    Element::Struct *getStruct(const char *name);
    Element::Enum *getEnum(const char *name); 

    void getVariables(std::vector<Element::Variable *> *vars);

    void getVarsAfterIndex(int index,
                          std::vector<Element::Variable *> *vars);
    void getVarsBeforeIndex(int index,
                           std::vector<Element::Variable *> *vars);
    void nameToSymbol(const char *name, std::string *new_name);
    void functionNameToSymbol(const char *name, std::string *new_name,
                              int linkage,
                              std::vector<Element::Variable *> *types);
    void eraseLLVMMacros(void);
    void eraseLLVMMacrosAndCTOFunctions(void);
    void getFunctionNames(std::set<std::string> *names);
    bool setNamespaces(std::vector<std::string> *namespaces);

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
