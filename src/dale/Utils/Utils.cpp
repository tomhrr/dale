#include "Utils.h"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cctype>
#include <sys/stat.h>

namespace dale
{
int is_simple_float(const char *str)
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
                    return 0;
                } else {
                    hit_period = 1;
                }
            } else {
                return 0;
            }
        }
    }
    return 1;
}

int is_simple_int(const char *str)
{
    int j;
    int len = strlen(str);
    if ((len >= 3)
            && (str[0] == '0')
            && (str[1] == 'x')) {
        for (j = 2; j < len; j++) {
            char c = str[j];
            if (!isxdigit(c)) {
                return 0;
            }
        }
    } else {
        for (j = 0; j < len; j++) {
            char c = str[j];
            if ((j == 0) && c == '-') {
                continue;
            } else if (!isdigit(c)) {
                return 0;
            }
        }
    }
    return 1;
}

int string_fits_in_int(const char *str)
{
    if ((strlen(str) == 1) && isdigit(str[0])) {
        return 1;
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

int files_are_equivalent(const char *filename1,
                         const char *filename2)
{
    struct stat f1;
    struct stat f2;

    if (stat(filename1, &f1) == -1) {
        printf("Unable to stat %s: %s\n",
               filename1,
               strerror(errno));
        return 0;
    }

    if (stat(filename2, &f2) == -1) {
        printf("Unable to stat %s: %s\n",
               filename2,
               strerror(errno));
        return 0;
    }

    return (f1.st_ino == f2.st_ino);
}

ino_t get_inode(const char *filename1)
{
    struct stat f1;

    if (stat(filename1, &f1) == -1) {
        printf("Unable to stat %s: %s\n",
               filename1,
               strerror(errno));
        return 0;
    }

    return f1.st_ino;
}

ino_t get_inode(int fildes)
{
    struct stat f1;

    if (fstat(fildes, &f1) == -1) {
        printf("Unable to stat FD: %s\n",
               strerror(errno));
        return 0;
    }

    return f1.st_ino;
}
}
