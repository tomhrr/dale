#include "Function.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../STL/STL.h"

namespace dale {
Function::Function() {
    is_macro = false;
    always_inline = false;
    cto = false;
    is_destructor = false;
    is_setf_fn = false;
    serialise = true;
    index = 0;
}

Function::Function(Type *return_type,
                   std::vector<Variable *> *parameters,
                   llvm::Function *llvm_function, bool is_macro,
                   std::string *symbol, bool always_inline) {
    this->return_type = return_type;
    this->parameters = *parameters;
    this->llvm_function = llvm_function;
    this->is_macro = is_macro;
    this->symbol = *symbol;
    this->always_inline = always_inline;

    cto = false;
    is_destructor = false;
    is_setf_fn = false;
    serialise = true;
    index = 0;
}

Function::~Function() {
    if (parameters.size()) {
        STL::deleteElements(&parameters);
    }
}

Label *Function::getLabel(const char *name) {
    std::map<std::string, Label *>::iterator b = labels.find(name);
    if (b == labels.end()) {
        return NULL;
    }
    return b->second;
}

bool Function::addLabel(const char *name, Label *label) {
    labels.insert(std::pair<std::string, Label *>(name, label));
    return true;
}

bool Function::isVarArgs() {
    if (parameters.size() == 0) {
        return false;
    }

    Variable *back = parameters.back();

    return (back->type->base_type == BaseType::VarArgs);
}

int Function::numberOfRequiredArgs() {
    if (parameters.size() == 0) {
        return 0;
    }

    int num_of_args = parameters.size();

    if (isVarArgs()) {
        num_of_args -= 1;
    }

    return num_of_args;
}

bool Function::isEqualTo(Function *other_fn) {
    if (!return_type->isEqualTo(other_fn->return_type)) {
        return false;
    }

    return dale::STL::isEqualTo(&parameters, &(other_fn->parameters));
}

bool Function::attrsAreEqual(Function *other_fn) {
    return !((always_inline ^ other_fn->always_inline) ||
             (cto ^ other_fn->cto));
}

bool Function::isDeclaration() {
    return (!llvm_function || (llvm_function->size() == 0));
}

bool Function::hasRetval() { return (return_type->is_retval != 0); }
}
