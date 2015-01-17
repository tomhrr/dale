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
    std::vector<Variable *> *parameter_types;
    std::vector<DeferredGoto *> *defgotos;
    std::map<std::string, Label *> *labels;
    llvm::Function *llvm_function;
    int is_macro;
    std::string *internal_name;
    int always_inline;
    std::string once_tag;
    int cto;
    int is_destructor;
    int is_setf_fn;
    int linkage;
    int index;
    bool serialise;

    Function();
    Function(Type *return_type,
             std::vector<Variable *> *parameter_types,
             llvm::Function *llvm_function,
             int is_macro,
             std::string *internal_name,
             int always_inline = 0);
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
