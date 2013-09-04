#ifndef DALE_ERRORREPORTER
#define DALE_ERRORREPORTER

#include <vector>
#include "../Position/Position.h"
#include "../Error/Error.h"
#include "../Element/Type/Type.h"
#include "../Node/Node.h"

namespace dale
{
class ErrorReporter
{
public:
    const char *current_filename;
    std::vector<Error *> *errors;
    int error_index;

    ErrorReporter(const char *current_filename);
    ~ErrorReporter();
    void addError(Error& err);
    void addError(Error *new_error);
    int getErrorTypeCount(int error_type);
    int getErrorCount(void);
    Error *popLastError(void);
    void popErrors(int original_count);
    void flush(void);
    
    bool assertIsIntegerType(const char *form_name,
                             Node *n,
                             Element::Type *type,
                             const char *arg_number);
    bool assertIsPointerOrIntegerType(const char *form_name,
                                      Node *n,
                                      Element::Type *type,
                                      const char *arg_number);
    bool assertIsPointerType(const char *form_name,
                             Node *n,
                             Element::Type *type,
                             const char *arg_number);
    bool assertTypeEquality(const char *form_name,
                            Node *n,
                            Element::Type *got,
                            Element::Type *expected,
                            int ignore_arg_constness);
    bool assertAtomIsStringLiteral(const char *form_name,
                                   Node *n,
                                   const char *arg_number);
    bool assertArgNums(const char *form_name,
                       Node *n,
                       int min_args,
                       int max_args);
    bool assertArgIsAtom(const char *form_name,
                         Node *n,
                         const char *arg_number);
    bool assertArgIsList(const char *form_name,
                         Node *n,
                         const char *arg_number);
    bool assertAtomIsSymbol(const char *form_name,
                            Node *n,
                            const char *arg_number);
};
}

#endif
