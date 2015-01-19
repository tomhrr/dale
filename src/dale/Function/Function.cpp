#include "Function.h"

#include "../STLUtils/STLUtils.h"

namespace dale
{
Function::Function()
{
    is_macro      = false;
    always_inline = false;
    cto           = false;
    is_destructor = false;
    is_setf_fn    = false;
    serialise     = true;
}

Function::Function(
    Type *return_type,
    std::vector<Variable *> *parameter_types,
    llvm::Function *llvm_function,
    bool is_macro,
    std::string *internal_name,
    bool always_inline)
{
    this->return_type     = return_type;
    this->parameter_types = *parameter_types;
    this->llvm_function   = llvm_function;
    this->is_macro        = is_macro;
    this->internal_name   = *internal_name;
    this->always_inline   = always_inline;

    cto           = false;
    is_destructor = false;
    is_setf_fn    = false;
    serialise     = true;
}

Function::~Function()
{
    if (parameter_types.size()) {
        stl::deleteElements(&parameter_types);
    }
}

Label *Function::getLabel(const char *name)
{
    std::map<std::string, Label*>::iterator b = labels.find(name);
    if (b == labels.end()) {
        return NULL;
    }
    return b->second;
}

bool Function::addLabel(const char *name, Label *label)
{
    labels.insert(std::pair<std::string, Label *>(name, label));
    return true;
}

bool Function::isVarArgs(void)
{
    if (parameter_types.size() == 0) {
        return false;
    }

    Variable *back = parameter_types.back();

    return (back->type->base_type == BaseType::VarArgs);
}

int Function::numberOfRequiredArgs(void)
{
    if (parameter_types.size() == 0) {
        return 0;
    }

    int num_of_args = parameter_types.size();

    if (isVarArgs()) {
        num_of_args -= 1;
    }

    return num_of_args;
}

bool Function::isEqualTo(Function *other_fn)
{
    if (!return_type->isEqualTo(other_fn->return_type)) {
        return false;
    }

    return dale::stl::isEqualTo(
        &parameter_types,
        &(other_fn->parameter_types)
    );
}

bool Function::attrsAreEqual(Function *other_fn)
{
    return !((always_inline ^ other_fn->always_inline)
            || (cto ^ other_fn->cto));
}

bool Function::isDeclaration(void)
{
    return (!llvm_function || (llvm_function->size() == 0));
}

bool Function::hasRetval(void)
{
    return (return_type->is_retval != 0);
}
}
