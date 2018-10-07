#ifndef DALE_BASICTYPES
#define DALE_BASICTYPES

#include <string>

#include "../Context/Context.h"
#include "../Operation/Cast/Cast.h"
#include "../ParseResult/ParseResult.h"
#include "../Type/Type.h"
#include "../llvm_Module.h"

namespace dale {
/*! BasicTypes

    Provides a set of functions for instantiating the functions
    required for core types, such as integers and floating-point
    numbers.
*/
namespace BasicTypes {
extern std::vector<std::string> basic_type_form_strings;

/*! Initialise the basic type form string array.
 */
void initBasicTypeFormStrings(void);

/*! Add a unary function to the context and module.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param name The name of the function.
 *  @param return_type The return type for the new function.
 *  @param type1 The type of the single parameter of the new function.
 *
 *  The caller has to add instructions to the body of the function, if
 *  required.
 */
Function *addSimpleUnaryFunction(Context *ctx, llvm::Module *mod,
                                 std::string *once_tag,
                                 const char *name, Type *return_type,
                                 Type *type1);

/*! Add a binary function to the context and module.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param name The name of the function.
 *  @param return_type The return type for the new function.
 *  @param type1 The type of the first parameter of the new function.
 *  @param type2 The type of the second parameter of the new function.
 *
 *  As with addSimpleUnaryFunction, the caller has to add instructions
 *  to the body of the function, if required.
 */
Function *addSimpleBinaryFunction(Context *ctx, llvm::Module *mod,
                                  std::string *once_tag,
                                  const char *name, Type *return_type,
                                  Type *type1, Type *type2);

/*! Instantiate the functions required for the given signed integer
 * type.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param type The signed integer type.
 *
 *  At the moment, these functions are +, -, /, *, =, !=, <, <=, >,
 *  >=, <<, >>, &, | and ^.
 */
void addSignedInt(Context *ctx, llvm::Module *mod,
                  std::string *once_tag, Type *type);

/*! Instantiate the functions required for the given floating point
 * type.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param type The floating point type.
 *
 *  At the moment, these functions are +, -, /, *, =, !=, <, <=, > and
 *  >=.
 */
void addFloatingPoint(Context *ctx, llvm::Module *mod,
                      std::string *once_tag, Type *type);

/*! Instantiate the functions required for the given unsigned integer
 * type.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param type The unsigned integer type.
 *
 *  The functions defined are the same as those for signed integers,
 *  except that complement (~) is also defined over unsigned integers.
 */
void addUnsignedInt(Context *ctx, llvm::Module *mod,
                    std::string *once_tag, Type *type);
}
}

#endif
