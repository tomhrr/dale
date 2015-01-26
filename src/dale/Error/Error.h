#ifndef DALE_ERROR
#define DALE_ERROR

#include <string>
#include <vector>
#include "../Node/Node.h"
#include "../Position/Position.h"
#include "../ErrorType/ErrorType.h"

namespace dale
{
class Error
{
public:
    int       instance;
    Position  begin;
    Position  end;
    Position  macro_begin;
    Position  macro_end;
    const char *filename;
    std::vector<std::string> *arg_strings;

    Error(const char *filename,
          int new_instance,
          int begin_line_number,
          int begin_column_number,
          int end_line_number,
          int end_column_number);
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
