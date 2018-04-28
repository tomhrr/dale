#ifndef DALE_ERROR
#define DALE_ERROR

#include <string>
#include <vector>
#include "../ErrorType/ErrorType.h"
#include "../Node/Node.h"
#include "../Position/Position.h"

namespace dale {
/*! Error

    A class for storing the details of an error.

    The 'reference macro' positions are set when an error occurs
    within a macro body.  They sometimes assist in finding the 'real'
    cause of the error when that is not obvious from the standard
    begin/end positions.
*/
class Error {
    public:
    /*! The error instance.  See ErrorType. */
    int instance;
    /*! The position at which the error begins. */
    Position begin;
    /*! The position at which the error ends. */
    Position end;
    /*! The position at which the reference macro begins. */
    Position macro_begin;
    /*! The position at which the reference macro ends. */
    Position macro_end;
    /*! The filename of the file in which the error occurred. */
    const char *filename;
    /*! Arguments for the error message printf string. */
    std::vector<std::string> arg_strings;

    /*! Construct a new error using the given instance and node.
     *  @param instance The error instance.
     *  @param node The reference node.
     *
     *  The node argument is used for identifying where the error
     *  occurred (filename and line/column number information).
     *
     *  The other error constructors take additional string and
     *  integer arguments, which values are used in toString to
     *  populate the format string for the error instance.
     */
    Error(int instance, Node *node);
    Error(int instance, Node *node, const char *str1);
    Error(int instance, Node *node, int num1, int num2);
    Error(int instance, Node *node, const char *str1, int num1,
          int num2);
    Error(int instance, Node *node, const char *str1, const char *str2,
          int num1);
    Error(int instance, Node *node, const char *str1, const char *str2,
          int num1, const char *str3);
    Error(int instance, Node *node, const char *str1, const char *str2);
    Error(int instance, Node *node, const char *str1, const char *str2,
          const char *str3);
    Error(int instance, Node *node, const char *str1, const char *str2,
          const char *str3, const char *str4);
    ~Error();

    /*! Get the error type for this error.
     *
     *  See ErrorType.
     */
    int getType();
    /*! Add an argument string to this error.
     *  @param str The argument string.
     *
     *  str is added to the list of arguments that will be used when
     *  populating the format string.
     */
    void addArgString(std::string *str);
    /*! Add an argument string to this error.
     *  @param str The argument string.
     *
     *  str is added to the list of arguments that will be used when
     *  populating the format string.
     */
    void addArgString(const char *str);
    /*! Get the error message string for this error.
     *  @param to The buffer for the string.
     *
     *  The error message string will be like (e.g.) "test.dt:5:15:
     *  error: label 'a' has already been defined in this scope".
     */
    void toString(std::string *to);
    /*! Initialise this error object using the given instance and node.
     *  @param instance The error instance.
     *  @param node The reference node.
     *
     *  Should only be called by constructors.
     */
    void init(int instance, Node *node);
    /*! Set position details using the given node.
     *  @param node The reference node.
     *
     *  Should only be called by constructors and internal methods.
     */
    void setFromNode(Node *node);
};
}

#endif
