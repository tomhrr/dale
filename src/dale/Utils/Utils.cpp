#include "Utils.h"

#include <sys/stat.h>

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "llvm/Support/DynamicLibrary.h"

#include "../Introspection/Introspection.h"
#include "Config.h"

namespace dale {
const char *progname = NULL;

bool isSimpleFloat(const char *str) {
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

bool isSimpleInt(const char *str) {
    int j;
    int len = strlen(str);
    if ((len >= 3) && (str[0] == '0') && (str[1] == 'x')) {
        for (j = 2; j < len; j++) {
            char c = str[j];
            if (!isxdigit(c)) {
                return false;
            }
        }
    } else if ((len >= 4) && (str[0] == '-') && (str[1] == '0') &&
               (str[2] == 'x')) {
        for (j = 3; j < len; j++) {
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

bool stringFitsInInt(const char *str) {
    if ((strlen(str) == 1) && isdigit(str[0])) {
        return true;
    }

    char buf[20];
    int len1, len2;

    snprintf(buf, sizeof(buf), "%d", (*str == '-' ? INT_MIN : INT_MAX));

    while (*str++ == '0') {
    }

    len1 = strlen(str);
    len2 = strlen(buf);

    return (len1 > len2) ? 0
                         : (len1 < len2) ? 1 : (strcmp(str, buf) <= 0);
}

bool filesAreEquivalent(const char *path1, const char *path2) {
    if (!strcmp(path1, path2)) {
        return true;
    }

    struct stat f1;
    struct stat f2;

    if (stat(path1, &f1) == -1) {
        printf("Unable to stat %s: %s\n", path1, strerror(errno));
        return false;
    }

    if (stat(path2, &f2) == -1) {
        printf("Unable to stat %s: %s\n", path2, strerror(errno));
        return false;
    }

    return (f1.st_ino == f2.st_ino);
}

void appendInt(std::string *to, int num) {
    char numstr[255];
    snprintf(numstr, sizeof(numstr), "%d", num);
    to->append(numstr);
    return;
}

void splitString(std::string *str, std::vector<std::string> *lst,
                 char c) {
    int index = 0;
    int len = str->length();

    while (index < len) {
        int found = str->find(c, index);
        if (found == -1) {
            found = str->length();
        }
        std::string part(str->substr(index, found - index));
        lst->push_back(part);
        index = found + 1;
    }
}

void encodeStandard(const std::string *from, std::string *to) {
    char buf[5];

    for (std::string::const_iterator b = from->begin(), e = from->end();
         b != e; ++b) {
        char c = *b;
        snprintf(buf, sizeof(buf),
                 ((isalnum(c) || c == '_') ? "%c" : "$%x"), c);
        to->append(buf);
    }

    return;
}

bool isValidModuleName(const std::string *name) {
    int i;
    for (i = 0; i < static_cast<int>(name->length()); ++i) {
        char c = (*name)[i];
        if (!(isalnum(c) || (c == '-') || (c == '_') || (c == '.'))) {
            return false;
        }
    }
    return true;
}

bool typesToString(std::vector<Type *>::iterator begin,
                   std::vector<Type *>::iterator end,
                   std::string *buf) {
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

bool typesToString(std::vector<Type *> *types, std::string *buf) {
    return typesToString(types->begin(), types->end(), buf);
}

bool typesToString(std::vector<Variable *>::iterator begin,
                   std::vector<Variable *>::iterator end,
                   std::string *buf) {
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

bool typesToString(std::vector<Variable *> *vars, std::string *buf) {
    return typesToString(vars->begin(), vars->end(), buf);
}

void error(const char *error_msg, bool show_perror) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s: %s", progname, error_msg);
    if (show_perror) {
        perror(buf);
    } else {
        fprintf(stderr, "%s\n", buf);
    }
    exit(1);
}

void error(const char *error_msg, const char *str1, bool show_perror) {
    char buf[1024];
    snprintf(buf, sizeof(buf), error_msg, str1);
    error(buf, show_perror);
}

void printVersion() {
    printf("%d.%d", DALE_VERSION_MAJOR, DALE_VERSION_MINOR);
    if (!strcmp("git", DALE_VERSION_TYPE)) {
        printf(" (rev %s)", DALE_VERSION_REV);
    }
}

void *fail(void) {
    fprintf(stderr, "Unable to resolve symbol.\n");
    abort();
}

void *lazyFunctionCreator(const std::string &name) {
    void *fn_pointer = find_introspection_function(name.c_str());
    if (fn_pointer) {
        return fn_pointer;
    }

    fn_pointer = llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(
        name.c_str());
    if (fn_pointer) {
        return fn_pointer;
    }

    if (name[0] != '_') {
        /* Try for one beginning with an underscore (OS X-specific). */
        std::string osx_name;
        osx_name.append("_");
        osx_name.append(name);

        fn_pointer =
            llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(
                osx_name);
        if (fn_pointer) {
            return fn_pointer;
        }
    }

    return (void *) fail;
}
}
