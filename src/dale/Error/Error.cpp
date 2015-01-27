#include "Error.h"

#include <cstdio>
#include <cstdlib>
#include "../ErrorType/ErrorType.h"

namespace dale
{
Error::Error(int new_instance,
             Node *node)
{
    init(new_instance, node);
}

Error::Error(int new_instance,
             Node *node,
             const char *str1)
{
    init(new_instance, node);
    arg_strings.push_back(str1);
}

Error::Error(int new_instance,
             Node *node,
             const char *str1,
             int str2,
             int str3)
{
    init(new_instance, node);
    arg_strings.push_back(str1);

    char buf[100];
    sprintf(buf, "%d", str2);
    arg_strings.push_back(buf);
    sprintf(buf, "%d", str3);
    arg_strings.push_back(buf);
}

Error::Error(int new_instance,
             Node *node,
             int str1,
             int str2)
{
    init(new_instance, node);

    char buf[100];
    sprintf(buf, "%d", str1);
    arg_strings.push_back(buf);
    sprintf(buf, "%d", str2);
    arg_strings.push_back(buf);
}

Error::Error(int new_instance,
             Node *node,
             const char *str1,
             const char *str2)
{
    init(new_instance, node);

    arg_strings.push_back(str1);
    arg_strings.push_back(str2);
}

Error::Error(int new_instance,
             Node *node,
             const char *str1,
             const char *str2,
             const char *str3)
{
    init(new_instance, node);

    arg_strings.push_back(str1);
    arg_strings.push_back(str2);
    arg_strings.push_back(str3);
}

Error::Error(int new_instance,
             Node *node,
             const char *str1,
             const char *str2,
             const char *str3,
             const char *str4)
{
    init(new_instance, node);

    arg_strings.push_back(str1);
    arg_strings.push_back(str2);
    arg_strings.push_back(str3);
    arg_strings.push_back(str4);
}

Error::~Error()
{
}

void Error::addArgString(std::string *str)
{
    arg_strings.push_back(*str);
}

void Error::addArgString(const char *str)
{
    arg_strings.push_back(str);
}

void Error::toString(std::string *str)
{
    char first_err_buf[256];
    char buf[256];

    int type = getType();
    const char *type_string;
    switch (type) {
    case ErrorType::Error:
        type_string = "error";
        break;
    case ErrorType::Warning:
        type_string = "warning";
        break;
    case ErrorType::Diagnostic:
        type_string = "diagnostic";
        break;
    default:
        type_string = "unknown";
    }

    const char *main_err_str = errorInstanceToString(instance);

    if (arg_strings.size() == 0) {
        sprintf(first_err_buf, "%s", main_err_str);
    } else if (arg_strings.size() == 1) {
        sprintf(first_err_buf, main_err_str, arg_strings[0].c_str());
    } else if (arg_strings.size() == 2) {
        sprintf(first_err_buf, main_err_str, arg_strings[0].c_str(),
                arg_strings[1].c_str());
    } else if (arg_strings.size() == 3) {
        sprintf(first_err_buf, main_err_str, arg_strings[0].c_str(),
                arg_strings[1].c_str(),
                arg_strings[2].c_str());
    } else if (arg_strings.size() == 4) {
        sprintf(first_err_buf, main_err_str, arg_strings[0].c_str(),
                arg_strings[1].c_str(),
                arg_strings[2].c_str(),
                arg_strings[3].c_str());
    } else {
        fprintf(stderr,
                "Internal error: too many strings (>4) "
                "in the error.\n");
        abort();
    }

    char macbuf[256];

    if (macro_begin.getLineNumber() != 0) {
        sprintf(macbuf, " (see macro at %d:%d)",
                macro_begin.getLineNumber(),
                macro_begin.getColumnNumber());
    } else {
        macbuf[0] = '\0';
    }

    sprintf(buf,
            "%s:%d:%d: %s: %s%s",
            filename,
            begin.getLineNumber(),
            begin.getColumnNumber(),
            type_string,
            first_err_buf,
            macbuf);

    str->append(buf);
}

int Error::getType(void)
{
    switch (instance) {
    case ErrorInst::Lexer::Null:
        return ErrorType::Diagnostic;
    case ErrorInst::Generator::StructContainsPadding:
        return ErrorType::Warning;
    default:
        return ErrorType::Error;
    }
}

void Error::init(int new_instance, Node *node)
{
    setFromNode(node);
    instance = new_instance;
}

void Error::setFromNode(Node *node)
{
    filename = node->filename;
    node->getBeginPos()->copyTo(&begin);
    node->getEndPos()->copyTo(&end);
    if (node->macro_begin.line_number) {
        macro_begin.setLineAndColumn(node->macro_begin.line_number,
                                     node->macro_begin.column_number);
        macro_end.setLineAndColumn(node->macro_end.line_number,
                                   node->macro_end.column_number);
    } else {
        macro_begin.setLineAndColumn(0,0);
        macro_end.setLineAndColumn(0,0);
    }
}
}
