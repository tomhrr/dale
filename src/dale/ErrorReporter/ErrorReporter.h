#ifndef DALE_ERRORREPORTER
#define DALE_ERRORREPORTER

#include <vector>
#include "../Position/Position.h"
#include "../Error/Error.h"

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
    void flush(void);
};
}

#endif
