#ifndef DALE_ERROR
#define DALE_ERROR

#include <string>
#include <vector>
#include "../Node/Node.h"
#include "../Position/Position.h"
#include "../ErrorType/ErrorType.h"

namespace dale
{
/*! Error

    A class for storing the details of an error.

    The 'reference macro' positions are set when an error occurs
    within a macro body.  They sometimes assist in finding the 'real'
    cause of the error when that is not obvious from the standard
    begin/end positions.
*/
class Error
{
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
    std::vector<std::string> *arg_strings;

    Error(int new_instance,
          Node *node);
    Error(int new_instance,
          Node *node,
          const char *str1);
    Error(int new_instance,
          Node *node,
          int str1,
          int str2);
    Error(int new_instance,
          Node *node,
          const char *str1,
          int str2,
          int str3);
    Error(int new_instance,
          Node *node,
          const char *str1,
          const char *str2);
    Error(int new_instance,
          Node *node,
          const char *str1,
          const char *str2,
          const char *str3);
    Error(int new_instance,
          Node *node,
          const char *str1,
          const char *str2,
          const char *str3,
          const char *str4);
    ~Error();
    /* See constructor notes for Token - these classes are
     * also pretty similar and could possibly be refactored
     * into a single class. */
    const char *getPrintfString(void);
    int getType(void);
    void addArgString(std::string *str);
    void addArgString(const char *str);
    void toString(std::string *str);
};
}

#endif
