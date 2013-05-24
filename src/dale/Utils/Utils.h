#ifndef DALE_UTILS
#define DALE_UTILS

#include <climits>
#include <cerrno>
#include <sys/stat.h>

namespace dale
{
int is_simple_float(const char *str);
int is_simple_int(const char *str);
int string_fits_in_int(const char *str);
int files_are_equivalent(const char *filename1,
                         const char *filename2);
ino_t get_inode(const char *filename1);
ino_t get_inode(int fildes);
}

#endif
