#ifndef DALE_UTILS
#define DALE_UTILS

#include <climits>
#include <cerrno>
#include <sys/stat.h>
#include <vector>
#include <string>

#include "../llvm_Module.h"
#include "../Type/Type.h"
#include "../Variable/Variable.h"

#define _unused(x) ((void)x)

#define STRTOUL_FAILED(ret, str, end) \
    (((((ret) == ULONG_MAX || ((ret) == 0)) && (errno == ERANGE)) \
                || (((ret) == 0) && ((str) == (end)))))
#define DECIMAL_RADIX 10

namespace dale
{
/*! Check whether a string represents a decimal number.
 *  @param str The string.
 */
bool isSimpleFloat(const char *str);
/*! Check whether a string represents an integer.
 *  @param str The string.
 */
bool isSimpleInt(const char *str);
/*! Check whether an integer string is small enough to be converted to int.
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
bool filesAreEquivalent(const char *path1,
                          const char *path2);
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
void splitString(std::string *str, std::vector<std::string> *lst, char c);
/*! Encode a name so it may be used in a symbol.
 *  @param from The string.
 *  @param to The buffer for the result.
 */
void encodeStandard(const std::string *from, std::string *to);
/*! Check whether a name is a valid module name.
 *  @param name The module name.
 */
bool isValidModuleName(const std::string *name);

/*! Get an LLVM function type.
 *  @param t The return type.
 *  @param v The parameter types.
 *  @param b Whether the function is a varargs function.
 */
llvm::FunctionType *getFunctionType(llvm::Type *t,
                                    std::vector<llvm::Type*> &v, bool b);

/*! Stringify a collection of types.
 *  @param begin The beginning iterator.
 *  @param end The ending iterator.
 *  @param buf The buffer for the result.
 */
bool typesToString(std::vector<Type *>::iterator begin,
                   std::vector<Type *>::iterator end,
                   std::string *buf);
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
/*! Construct an LLVM string constant data array.
 *  @param data The data for the array.
 */
llvm::Constant *getStringConstantArray(const char *data);
/*! Construct an LLVM null pointer for the type.
 *  @param type The type.
 */
llvm::ConstantPointerNull *getNullPointer(llvm::Type *type);
}

#endif
