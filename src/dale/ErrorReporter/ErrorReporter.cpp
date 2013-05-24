#include "ErrorReporter.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace dale
{
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
}
