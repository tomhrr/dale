#ifndef DALE_ELEMENT_FUNCTION
#define DALE_ELEMENT_FUNCTION

#include "../Type/Type.h"
#include "../Variable/Variable.h"
#include "../Label/Label.h"
#include "../../DeferredGoto/DeferredGoto.h"
#include <vector>

namespace dale
{
namespace Element
{
class Function
{
public:
    Element::Type *return_type;
    std::vector<Element::Variable *> *parameter_types;
    std::vector<DeferredGoto *> *defgotos;
    std::map<std::string, Element::Label *> *labels;
    llvm::Function *llvm_function;
    int is_macro;
    std::string *internal_name;
    int always_inline;
    int retval;
    std::string once_tag;
    int cto;
    int is_destructor;
    int is_setf_fn;
    int linkage;
    int index;
    bool serialise;

    Function();
    Function(Element::Type *return_type,
             std::vector<Element::Variable *> *parameter_types,
             llvm::Function *llvm_function,
             int is_macro,
             std::string *internal_name,
             int always_inline = 0,
             int retval = 0);
    ~Function();

    int isVarArgs(void);
    unsigned int numberOfRequiredArgs(void);
    int isEqualTo(Element::Function *fn);
    int attrsAreEqual(Element::Function *fn);
    Element::Label *getLabel(const char *str);
    bool addLabel(const char *str, Element::Label *label);
    bool isDeclaration(void);
};
}
}

#endif
