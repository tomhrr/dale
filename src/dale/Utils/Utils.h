#ifndef DALE_UTILS
#define DALE_UTILS

#include <climits>
#include <cerrno>
#include <sys/stat.h>
#include <vector>
#include <string>

namespace dale
{
int is_simple_float(const char *str);
int is_simple_int(const char *str);
int string_fits_in_int(const char *str);
int files_are_equivalent(const char *filename1,
                         const char *filename2);
ino_t get_inode(const char *filename1);
ino_t get_inode(int fildes);
void append_int(std::string *to, int num);

void splitString(std::string *str, std::vector<std::string> *lst, char c);
void encodeStandard(const std::string *from, std::string *to);
}

#endif
