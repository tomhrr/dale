#include "ErrorReporter.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace dale
{
typedef std::vector<Node*> symlist;

ErrorReporter::ErrorReporter(const char *new_current_filename)
{
    current_filename = new_current_filename;
    errors = new std::vector<Error *>;
    error_index = 0;
}

ErrorReporter::~ErrorReporter()
{
    delete errors;
}

void ErrorReporter::addError(Error& err)
{
    Error *eee = new Error(err);
    if (!eee->filename) {
        eee->filename = "<unknown>";
    }
    if (!strcmp(eee->filename, "")) {
        eee->filename = "<unknown>";
    }
    errors->push_back(eee);
}

void ErrorReporter::addError(Error *err)
{
    if (!err->filename) {
        err->filename = "<unknown>";
    }
    if (!strcmp(err->filename, "")) {
        err->filename = "<unknown>";
    }
    errors->push_back(err);
}

void ErrorReporter::flush(void)
{
    if (error_index > (int) errors->size()) {
        fprintf(stderr,
                "Internal error: error index (%d) is larger "
                "than the current error count (%d).\n",
                error_index, (int) errors->size());
        abort();
    }

    std::vector<Error *>::iterator b = errors->begin(),
                                   e = errors->end();
    std::advance(b, error_index);
    if (b == e) {
        return;
    }

    std::string temp;

    for (; b != e; ++b) {
        (*b)->toString(&temp);
        fprintf(stderr, "%s\n", temp.c_str());
        temp.clear();
        ++error_index;
    }
}

int ErrorReporter::getErrorTypeCount(int error_type)
{
    int total = 0;

    for (std::vector<Error *>::iterator b = errors->begin(),
            e = errors->end(); b != e; ++b) {
        if ((*b)->getType() == error_type) {
            ++total;
        }
    }

    return total;
}

int ErrorReporter::getErrorCount(void)
{
    return errors->size();
}

Error *ErrorReporter::popLastError(void)
{
    Error *last_error = errors->back();
    errors->pop_back();
    return last_error;
}

void ErrorReporter::popErrors(int original_count)
{
    int diff = getErrorTypeCount(ErrorType::Error) - original_count;
    while (diff--) {
        popLastError();
    }
}

bool ErrorReporter::assertIsIntegerType(const char *form_name,
                                        Node *n,
                                        Element::Type *type,
                                        const char *arg_number)
{
    if (type->base_type == BaseType::Int) {
        return true;
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "int", arg_number, temp.c_str()
    );
    addError(e);
    return false;
}

bool ErrorReporter::assertIsPointerOrIntegerType(const char *form_name,
                                                 Node *n,
                                                 Element::Type *type,
                                                 const char *arg_number)
{
    if (type->points_to || type->isIntegerType()) {
        return true;
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a pointer or integer", arg_number, temp.c_str()
    );
    addError(e);
    return false;
}

bool ErrorReporter::assertIsPointerType(const char *form_name,
                                        Node *n,
                                        Element::Type *type,
                                        const char *arg_number)
{
    if (type->points_to) {
        return true;
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a pointer", arg_number, temp.c_str()
    );
    addError(e);
    return false;
}

bool ErrorReporter::assertTypeEquality(const char *form_name,
                                       Node *n,
                                       Element::Type *got,
                                       Element::Type *expected,
                                       int ignore_arg_constness)
{
    if (got->isEqualTo(expected, ignore_arg_constness)) {
        return true;
    }

    std::string got_str;
    std::string exp_str;
    got->toStringProper(&got_str);
    expected->toStringProper(&exp_str);

    Error *e = new Error(
        ((!strcmp(form_name, "return"))
         ? ErrorInst::Generator::IncorrectReturnType
         : ErrorInst::Generator::IncorrectType),
        n,
        exp_str.c_str(), got_str.c_str()
    );
    addError(e);
    return false;
}

bool ErrorReporter::assertAtomIsStringLiteral(const char *form_name,
                                              Node *n,
                                              const char *arg_number)
{
    if (n->token && (n->token->type == TokenType::StringLiteral)) {
        return true;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a string literal", arg_number, n->token->tokenType()
    );
    addError(e);
    return false;
}

/* This always assumes that n is a list.
   If max_args is -1, then max_args is not checked. */
bool ErrorReporter::assertArgNums(const char *form_name,
                                  Node *n,
                                  int min_args,
                                  int max_args)
{
    symlist *lst = n->list;

    char buf1[100];
    char buf2[100];

    if (!lst) {
        fprintf(stderr,
                "Internal error: no list to assert arg nums.\n");
        abort();
    }

    int num_args = (int) lst->size() - 1;

    if (min_args == max_args) {
        if (num_args == min_args) {
            return 1;
        } else {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                n,
                form_name
            );
            sprintf(buf1, "%d", min_args);
            sprintf(buf2, "%d", num_args);
            e->addArgString(buf1);
            e->addArgString(buf2);
            addError(e);
            return false;
        }
    }

    if (num_args < min_args) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            n,
            form_name
        );
        sprintf(buf1, "%d", min_args);
        sprintf(buf2, "%d", num_args);
        e->addArgString(buf1);
        e->addArgString(buf2);
        addError(e);
        return false;
    }

    if ((max_args != -1) && (num_args > max_args)) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMaximumNumberOfArgs,
            n,
            form_name
        );
        sprintf(buf1, "%d", max_args);
        sprintf(buf2, "%d", num_args);
        e->addArgString(buf1);
        e->addArgString(buf2);
        addError(e);
        return false;
    }

    return true;
}

bool ErrorReporter::assertArgIsAtom(const char *form_name,
                                    Node *n,
                                    const char *arg_number)
{
    if (n->is_token) {
        return true;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "an atom", arg_number, "a list"
    );
    addError(e);
    return false;
}

bool ErrorReporter::assertArgIsList(const char *form_name,
                                    Node *n,
                                    const char *arg_number)
{
    if (n->is_list) {
        return true;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a list", arg_number, "a symbol"
    );
    addError(e);
    return false;
}

bool ErrorReporter::assertAtomIsSymbol(const char *form_name,
                                       Node *n,
                                       const char *arg_number)
{
    if (n->token && (n->token->type == TokenType::String)) {
        return true;
    }

    Error *e = new Error(
        ErrorInst::Generator::IncorrectArgType,
        n,
        form_name, "a symbol", arg_number, n->token->tokenType()
    );
    addError(e);
    return false;
}
}
