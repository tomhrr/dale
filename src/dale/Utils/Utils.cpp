#include "Utils.h"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cctype>
#include <sys/stat.h>

namespace dale
{
bool
isSimpleFloat(const char *str)
{
    int j = 0;
    int hit_period = 0;
    int len = strlen(str);
    if ((len >= 1) && (str[0] == '-')) {
        j++;
    }
    for (; j < len; j++) {
        char c = str[j];
        if (!isdigit(c)) {
            if (c == '.') {
                if (hit_period) {
                    return false;
                } else {
                    hit_period = 1;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}

bool
isSimpleInt(const char *str)
{
    int j;
    int len = strlen(str);
    if ((len >= 3)
            && (str[0] == '0')
            && (str[1] == 'x')) {
        for (j = 2; j < len; j++) {
            char c = str[j];
            if (!isxdigit(c)) {
                return false;
            }
        }
    } else {
        for (j = 0; j < len; j++) {
            char c = str[j];
            if ((j == 0) && c == '-') {
                continue;
            } else if (!isdigit(c)) {
                return false;
            }
        }
    }
    return true;
}

bool
stringFitsInInt(const char *str)
{
    if ((strlen(str) == 1) && isdigit(str[0])) {
        return true;
    }

    char buf[20];
    int len1, len2;

    sprintf(buf, "%d", (*str == '-' ? INT_MIN : INT_MAX));

    while (*str++ == '0') {
    }

    len1 = strlen(str);
    len2 = strlen(buf);

    return (len1 > len2) ? 0
           : (len1 < len2) ? 1
           : (strcmp(str, buf) <= 0);
}

bool
filesAreEquivalent(const char *path1, const char *path2)
{
    if (!strcmp(path1, path2)) {
        return true;
    }

    struct stat f1;
    struct stat f2;

    if (stat(path1, &f1) == -1) {
        printf("Unable to stat %s: %s\n",
               path1,
               strerror(errno));
        return false;
    }

    if (stat(path2, &f2) == -1) {
        printf("Unable to stat %s: %s\n",
               path2,
               strerror(errno));
        return false;
    }

    return (f1.st_ino == f2.st_ino);
}

void
appendInt(std::string *to, int num)
{
    char numstr[255];
    sprintf(numstr, "%d", num);
    to->append(numstr);
    return;
}

void
splitString(std::string *str, std::vector<std::string> *lst, char c)
{
    int index = 0;
    int len = str->length();

    while (index < len) {
        int found = str->find(c, index);
        if (found == -1) {
            found = str->length();
        }
        std::string temp(str->substr(index, found - index));
        lst->push_back(temp);
        index = found + 1;
    }
}

void
encodeStandard(const std::string *from, std::string *to)
{
    char buf[5];

    for (std::string::const_iterator b = from->begin(),
                                     e = from->end();
            b != e;
            ++b) {
        char c = *b;
        sprintf(buf, ((isalnum(c) || c == '_') ? "%c" : "$%x"), c);
        to->append(buf);
    }

    return;
}

bool
isValidModuleName(const std::string *name)
{
    int i;
    for (i = 0; i < (int) name->length(); ++i) {
        char c = (*name)[i];
        if (!(isalnum(c) || (c == '-') || (c == '_') || (c == '.'))) {
            return false;
        }
    }
    return true;
}

llvm::FunctionType *
getFunctionType(llvm::Type *t, std::vector<llvm::Type*> &v, bool b) {
    llvm::ArrayRef<llvm::Type*> temp(v);
    return llvm::FunctionType::get(t, temp, b);
}

bool
typesToString(std::vector<Type *>::iterator begin,
              std::vector<Type *>::iterator end,
              std::string *buf)
{
    if (begin == end) {
        buf->append("void");
        return true;
    }

    for (; begin != end; ++begin) {
        (*begin)->toString(buf);
        if ((begin + 1) != end) {
            buf->append(" ");
        }
    }

    return true;
}

bool
typesToString(std::vector<Type *> *types, std::string *buf)
{
    return typesToString(types->begin(), types->end(), buf);
}

bool
typesToString(std::vector<Variable *>::iterator begin,
              std::vector<Variable *>::iterator end,
              std::string *buf)
{
    if (begin == end) {
        buf->append("void");
        return true;
    }

    for (; begin != end; ++begin) {
        (*begin)->type->toString(buf);
        if ((begin + 1) != end) {
            buf->append(" ");
        }
    }

    return true;
}

bool
typesToString(std::vector<Variable *> *vars, std::string *buf)
{
    return typesToString(vars->begin(), vars->end(), buf);
}

llvm::Constant *
getStringConstantArray(const char *data)
{
    return
        llvm::cast<llvm::Constant>(
#if D_LLVM_VERSION_MINOR < 2
            llvm::ConstantArray::get(
#else
            llvm::ConstantDataArray::getString(
#endif
                llvm::getGlobalContext(),
                data,
                true
            )
        );
}
}
