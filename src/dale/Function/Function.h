#ifndef DALE_ELEMENT_FUNCTION
#define DALE_ELEMENT_FUNCTION

#include "../Type/Type.h"
#include "../Variable/Variable.h"
#include "../Label/Label.h"
#include "../DeferredGoto/DeferredGoto.h"
#include <vector>

namespace dale
{
class Function
{
public:
    Type *return_type;
    std::vector<Variable *> parameter_types;
    std::vector<DeferredGoto *> deferred_gotos;
    std::map<std::string, Label *> labels;
    llvm::Function *llvm_function;
    std::string internal_name;
    std::string once_tag;
    bool always_inline;
    bool is_macro;
    bool cto;
    bool is_destructor;
    bool is_setf_fn;
    bool serialise;
    int linkage;
    int index;

    Function();
    Function(Type *return_type,
             std::vector<Variable *> *parameter_types,
             llvm::Function *llvm_function,
             bool is_macro,
             std::string *internal_name,
             bool always_inline = false);
    ~Function();

    int isVarArgs(void);
    unsigned int numberOfRequiredArgs(void);
    int isEqualTo(Function *fn);
    int attrsAreEqual(Function *fn);
    Label *getLabel(const char *str);
    bool addLabel(const char *str, Label *label);
    bool isDeclaration(void);
    bool hasRetval(void);
};
}

#endif
