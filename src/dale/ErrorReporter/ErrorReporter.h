#ifndef DALE_ERRORREPORTER
#define DALE_ERRORREPORTER

#include <vector>
#include "../Error/Error.h"
#include "../Node/Node.h"
#include "../Position/Position.h"
#include "../Type/Type.h"

namespace dale {
/*! ErrorReporter

    The ErrorReporter collects errors and provides various assertion
    methods that generate errors when their conditions are not met.
    It allows for delayed reporting and 'rolling back' to previous
    states (by way of error counts).
*/
class ErrorReporter {
    public:
    /*! The filename of the file currently being processed. */
    const char *current_filename;
    /*! The current list of errors. */
    std::vector<Error *> errors;
    /*! The index of the next error to be flushed in errors. */
    int error_index;

    /*! Construct a new ErrorReporter.
     *  @param current_filename The current filename.
     */
    explicit ErrorReporter(const char *current_filename);
    ~ErrorReporter();

    /*! Add an error.
     *  @param err The error.
     *
     *  This does not take ownership of the error.
     */
    void addError(Error const &err);
    /*! Add an error.
     *  @param err The error.
     *
     *  This takes ownership of the error.
     */
    void addError(Error *err);
    /*! Get the number of errors of the given type present in the
     * reporter.
     *  @param error_type The error type (see ErrorType).
     */
    int getErrorTypeCount(int error_type);
    /*! Get the number of errors present in the reporter.
     */
    int getErrorCount();
    /*! Pop the last-added error from the reporter.
     */
    Error *popLastError();
    /*! Pop errors until the reporter contains the given number of
     * errors.
     *  @param original_count The number of errors that should be
     *  present after the method completes.
     */
    void popErrors(int original_count);
    /*! Print all errors to standard error.
     */
    void flush();

    /*! Confirm that the argument node represents the native integer
     * type.
     *  @param form_name The form name (for the error message).
     *  @param n The node (for the error message).
     *  @param type The type of the node.
     *  @param arg_number The argument number of the node.
     */
    bool assertIsIntegerType(const char *form_name, Node *n, Type *type,
                             const char *arg_number);
    /*! Confirm that the argument node represents the native integer
     *  type or a pointer.
     *  @param form_name The form name (for the error message).
     *  @param n The node (for the error message).
     *  @param type The type of the node.
     *  @param arg_number The argument number of the node.
     */
    bool assertIsPointerOrIntegerType(const char *form_name, Node *n,
                                      Type *type,
                                      const char *arg_number);
    /*! Confirm that the argument node represents a pointer.
     *  @param form_name The form name (for the error message).
     *  @param n The node (for the error message).
     *  @param type The type of the node.
     *  @param arg_number The argument number of the node.
     */
    bool assertIsPointerType(const char *form_name, Node *n, Type *type,
                             const char *arg_number);
    /*! Confirm that the two types are equal.
     *  @param form_name The form name (for the error message).
     *  @param n The node (for the error message).
     *  @param got The type of the node.
     *  @param expected The type that the node should have.
     *  @param ignore_arg_constness Whether to disregard the constness
     *  of the expected type in the comparison.
     */
    bool assertTypeEquality(const char *form_name, Node *n, Type *got,
                            Type *expected, bool ignore_arg_constness);
    /*! Confirm that the node is a string literal.
     *  @param form_name The form name (for the error message).
     *  @param n The node.
     *  @param arg_number The argument number of the node.
     */
    bool assertAtomIsStringLiteral(const char *form_name, Node *n,
                                   const char *arg_number);
    /*! Confirm that the list node's argument count is within a range.
     *  @param form_name The form name (for the error message).
     *  @param n The node.
     *  @param min The minimum number of arguments the form must have.
     *  @param max The maximum number of arguments the form may have.
     *
     *  If the form does not have a maximum number of arguments, the
     *  value -1 should be provided for max.
     */
    bool assertArgNums(const char *form_name, Node *n, int min_args,
                       int max_args);
    /*! Confirm that the node is an atom.
     *  @param form_name The form name (for the error message).
     *  @param n The node.
     *  @param arg_number The argument number of the node.
     */
    bool assertArgIsAtom(const char *form_name, Node *n,
                         const char *arg_number);
    /*! Confirm that the node is a list.
     *  @param form_name The form name (for the error message).
     *  @param n The node.
     *  @param arg_number The argument number of the node.
     */
    bool assertArgIsList(const char *form_name, Node *n,
                         const char *arg_number);
    /*! Confirm that the atom node is a symbol.
     *  @param form_name The form name (for the error message).
     *  @param n The node.
     *  @param arg_number The argument number of the node.
     */
    bool assertAtomIsSymbol(const char *form_name, Node *n,
                            const char *arg_number);
};
}

#endif
