#include "ErrorReporter.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <vector>

namespace dale {
ErrorReporter::ErrorReporter(const char *current_filename) {
    if (!strcmp(current_filename, "")) {
        current_filename = "<unknown>";
    }
    this->current_filename = current_filename;
    error_index = 0;
}

ErrorReporter::~ErrorReporter() {}

static void setDefaultFilename(Error *err) {
    if (!err->filename || !strcmp(err->filename, "")) {
        err->filename = "<unknown>";
    }
}

void ErrorReporter::addError(Error *err) {
    setDefaultFilename(err);
    errors.push_back(err);
}

void ErrorReporter::addError(Error const &err) {
    Error *new_err = new Error(err);
    addError(new_err);
}

void ErrorReporter::flush() {
    assert(error_index <= static_cast<int>(errors.size()));

    std::vector<Error *>::iterator b = errors.begin(), e = errors.end();
    std::advance(b, error_index);
    if (b == e) {
        return;
    }

    std::string error_message;
    for (; b != e; ++b, ++error_index) {
        (*b)->toString(&error_message);
        fprintf(stderr, "%s\n", error_message.c_str());
        error_message.clear();
    }
}

int ErrorReporter::getErrorTypeCount(int error_type) {
    int total = 0;
    for (std::vector<Error *>::iterator b = errors.begin(),
                                        e = errors.end();
         b != e; ++b) {
        if ((*b)->getType() == error_type) {
            ++total;
        }
    }

    return total;
}

int ErrorReporter::getErrorCount() { return errors.size(); }

Error *ErrorReporter::popLastError() {
    Error *last_error = errors.back();
    errors.pop_back();
    return last_error;
}

void ErrorReporter::popErrors(int original_count) {
    int diff = getErrorTypeCount(ErrorType::Error) - original_count;
    while (diff--) {
        popLastError();
    }
}

bool ErrorReporter::assertIsIntegerType(const char *form_name, Node *n,
                                        Type *type,
                                        const char *arg_number) {
    if (type->base_type == BaseType::Int) {
        return true;
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(ErrorInst::IncorrectArgType, n, form_name,
                         "int", arg_number, type_str.c_str());
    addError(e);
    return false;
}

bool ErrorReporter::assertIsPointerOrIntegerType(
    const char *form_name, Node *n, Type *type,
    const char *arg_number) {
    if (type->points_to || type->isIntegerType()) {
        return true;
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e =
        new Error(ErrorInst::IncorrectArgType, n, form_name,
                  "a pointer or integer", arg_number, type_str.c_str());
    addError(e);
    return false;
}

bool ErrorReporter::assertIsPointerType(const char *form_name, Node *n,
                                        Type *type,
                                        const char *arg_number) {
    if (type->points_to) {
        return true;
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(ErrorInst::IncorrectArgType, n, form_name,
                         "a pointer", arg_number, type_str.c_str());
    addError(e);
    return false;
}

bool ErrorReporter::assertTypeEquality(const char *form_name, Node *n,
                                       Type *got, Type *expected,
                                       bool ignore_arg_constness) {
    if (got->isEqualTo(expected, ignore_arg_constness)) {
        return true;
    }

    std::string got_type_str;
    std::string expected_type_str;
    got->toString(&got_type_str);
    expected->toString(&expected_type_str);

    Error *e = new Error(
        ((!strcmp(form_name, "return")) ? ErrorInst::IncorrectReturnType
                                        : ErrorInst::IncorrectType),
        n, expected_type_str.c_str(), got_type_str.c_str());
    addError(e);
    return false;
}

bool ErrorReporter::assertAtomIsStringLiteral(const char *form_name,
                                              Node *n,
                                              const char *arg_number) {
    if (n->token && (n->token->type == TokenType::StringLiteral)) {
        return true;
    }

    Error *e = new Error(ErrorInst::IncorrectArgType, n, form_name,
                         "a string literal", arg_number,
                         n->token->tokenType());
    addError(e);
    return false;
}

bool ErrorReporter::assertArgNums(const char *form_name, Node *n,
                                  int min_args, int max_args) {
    std::vector<Node *> *lst = n->list;
    assert(lst);

    int num_args = static_cast<int>(lst->size()) - 1;

    if (min_args == max_args) {
        if (num_args == min_args) {
            return 1;
        } else {
            Error *e = new Error(ErrorInst::IncorrectNumberOfArgs, n,
                                 form_name, min_args, num_args);
            addError(e);
            return false;
        }
    }

    if (num_args < min_args) {
        Error *e = new Error(ErrorInst::IncorrectMinimumNumberOfArgs, n,
                             form_name, min_args, num_args);
        addError(e);
        return false;
    }

    if ((max_args != -1) && (num_args > max_args)) {
        Error *e = new Error(ErrorInst::IncorrectMaximumNumberOfArgs, n,
                             form_name, max_args, num_args);
        addError(e);
        return false;
    }

    return true;
}

bool ErrorReporter::assertArgIsAtom(const char *form_name, Node *n,
                                    const char *arg_number) {
    if (n->is_token) {
        return true;
    }

    Error *e = new Error(ErrorInst::IncorrectArgType, n, form_name,
                         "an atom", arg_number, "a list");
    addError(e);
    return false;
}

bool ErrorReporter::assertArgIsList(const char *form_name, Node *n,
                                    const char *arg_number) {
    if (n->is_list) {
        return true;
    }

    Error *e = new Error(ErrorInst::IncorrectArgType, n, form_name,
                         "a list", arg_number, "a symbol");
    addError(e);
    return false;
}

bool ErrorReporter::assertAtomIsSymbol(const char *form_name, Node *n,
                                       const char *arg_number) {
    if (n->token && (n->token->type == TokenType::String)) {
        return true;
    }

    Error *e = new Error(ErrorInst::IncorrectArgType, n, form_name,
                         "a symbol", arg_number, n->token->tokenType());
    addError(e);
    return false;
}
}
