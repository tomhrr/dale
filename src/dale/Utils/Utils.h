#ifndef DALE_UTILS
#define DALE_UTILS

#include <sys/stat.h>
#include <cerrno>
#include <climits>
#include <string>
#include <vector>

#include "../Type/Type.h"
#include "../Variable/Variable.h"

#define _unused(x) ((void)x)

#define STRTOUL_FAILED(ret, str, end)                                \
    (((((ret) == ULONG_MAX || ((ret) == 0)) && (errno == ERANGE)) || \
      (((ret) == 0) && ((str) == (end)))))
#define DECIMAL_RADIX 10

/* Utility functions that do not depend on LLVM classes. */
namespace dale {
/*! The current executable name (i.e. argv[0]). */
extern const char *progname;
/*! Check whether a string represents a decimal number.
 *  @param str The string.
 */
bool isSimpleFloat(const char *str);
/*! Check whether a string represents an integer.
 *  @param str The string.
 */
bool isSimpleInt(const char *str);
/*! Check whether an integer string is small enough to be converted to
 * int.
 *  @param str The string.
 */
bool stringFitsInInt(const char *str);
/*! Check whether two file paths are equivalent.
 *  @param path1 The first path.
 *  @param path2 The second path.
 *
 *  Two paths are equivalent if they are the same path, or if they
 *  point to the same file.
 */
bool filesAreEquivalent(const char *path1, const char *path2);
/*! Append the integer to the string.
 *  @param to The string.
 *  @param num The integer.
 */
void appendInt(std::string *to, int num);

/*! Split a string using the given separator.
 *  @param str The string.
 *  @param lst The string list buffer, for the split parts.
 *  @param c The separator.
 */
void splitString(std::string *str, std::vector<std::string> *lst,
                 char c);
/*! Encode a name so it may be used in a symbol.
 *  @param from The string.
 *  @param to The buffer for the result.
 */
void encodeStandard(const std::string *from, std::string *to);
/*! Check whether a name is a valid module name.
 *  @param name The module name.
 */
bool isValidModuleName(const std::string *name);

/*! Stringify a collection of types.
 *  @param begin The beginning iterator.
 *  @param end The ending iterator.
 *  @param buf The buffer for the result.
 */
bool typesToString(std::vector<Type *>::iterator begin,
                   std::vector<Type *>::iterator end, std::string *buf);
/*! Stringify a collection of types.
 *  @param types The types to stringify.
 *  @param buf The buffer for the result.
 */
bool typesToString(std::vector<Type *> *types, std::string *buf);
/*! Stringify a collection of types.
 *  @param begin The beginning iterator.
 *  @param end The ending iterator.
 *  @param buf The buffer for the result.
 */
bool typesToString(std::vector<Variable *>::iterator begin,
                   std::vector<Variable *>::iterator end,
                   std::string *buf);
/*! Stringify a collection of types.
 *  @param vars The variables containing the types to stringify.
 *  @param buf The buffer for the result.
 */
bool typesToString(std::vector<Variable *> *types, std::string *buf);
/*! Report a fatal error and exit.
 *  @param error_msg The error message.
 *  @param show_perror Whether to use perror to show the current error
 *                     message prior to printing msg.
 */
void error(const char *error_msg, bool show_perror = false);
/*! Report a fatal error and exit.
 *  @param error_msg The error message (format string).
 *  @param str1 The first argument to the format string.
 *  @param show_perror Whether to use perror to show the current error
 *  message prior to printing msg.
 */
void error(const char *error_msg, const char *str1,
           bool show_perror = false);
/*! Print the current version number.
 */
void printVersion();
/*! Find and return the address of the given function.
 *  @param name The function name.
 */
void *lazyFunctionCreator(const std::string &name);
}

#endif
