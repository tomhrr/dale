#ifndef DALE_BASICTYPES
#define DALE_BASICTYPES

#include "../Element/Type/Type.h"
#include "../Context/Context.h"
#include "../ParseResult/ParseResult.h"
#include "../Operation/Cast/Cast.h"

#include "../llvm_LLVMContext.h"
#include "../llvm_Module.h"
#include "llvm/LinkAllPasses.h"
#include "../llvm_Linker.h"
#include "../llvm_Function.h"
#include "llvm/PassManager.h"
#include "../llvm_CallingConv.h"

namespace dale
{
/*! BasicTypes

    Provides a set of functions for instantiating the functions
    required for core types, such as integers and floating-point
    numbers.
*/
namespace BasicTypes
{
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
Element::Function *
addSimpleUnaryFunction(Context *ctx,
                       llvm::Module *mod,
                       std::string *once_tag,
                       const char *name,
                       Element::Type *return_type,
                       Element::Type *type1);

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
Element::Function *
addSimpleBinaryFunction(Context *ctx,
                        llvm::Module *mod,
                        std::string *once_tag,
                        const char *name,
                        Element::Type *return_type,
                        Element::Type *type1,
                        Element::Type *type2);

/*! Add a binary function, mapping to an IRBuilder instruction.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param name The name of the function.
 *  @param method_name An IRBuilder method pointer.
 *  @param return_type The return type for the new function.
 *  @param type The type of the two parameters of the new function.
 */
void
makeFunction(Context *ctx,
             llvm::Module *mod,
             std::string *once_tag,
             const char *name,
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &),
             Element::Type *return_type,
             Element::Type *type);

/*! Add a binary function for an enum type, mapping to an IRBuilder instruction.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param name The name of the function.
 *  @param method_name An IRBuilder method pointer.
 *  @param return_type The return type for the new function.
 *  @param type The type (an enum) of the two parameters of the new function.
 *  @param linkage The linkage for the new function.
 */
void
makeEnumFunction(Context *ctx,
                 llvm::Module *mod,
                 std::string *once_tag,
                 const char *name,
                 llvm::Value* (llvm::IRBuilder<>:: *method_name)
                     (llvm::Value*, llvm::Value*, const llvm::Twine &),
                 Element::Type *ret_type,
                 Element::Type *type,
                 int linkage);

/*! Instantiate the functions required for the given signed integer type.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param type The signed integer type.
 *
 *  At the moment, these functions are +, -, /, *, =, !=, <, <=, >,
 *  >=, <<, >>, &, | and ^.
 */
void
addSignedInt(Context *ctx,
             llvm::Module *mod,
             std::string *once_tag,
             Element::Type *type);

/*! Instantiate the functions required for the given floating point type.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param type The floating point type.
 *
 *  At the moment, these functions are +, -, /, *, =, !=, <, <=, > and
 *  >=.
 */
void
addFloatingPoint(Context *ctx,
                 llvm::Module *mod,
                 std::string *once_tag,
                 Element::Type *type);

/*! Instantiate the functions required for the given unsigned integer type.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param type The unsigned integer type.
 *
 *  The functions defined are the same as those for signed integers,
 *  except that complement (~) is also defined over unsigned integers.
 */
void
addUnsignedInt(Context *ctx,
               llvm::Module *mod,
               std::string *once_tag,
               Element::Type *type);

/*! Instantiate the functions required for the given enum type.
 *  @param ctx The context.
 *  @param mod The LLVM module.
 *  @param once_tag The current once tag, if applicable.
 *  @param type The enum type.
 *
 *  The functions defined are the same as those for signed integers.
 */
void
addEnum(Context *ctx,
        llvm::Module *mod,
        std::string *once_tag,
        Element::Type *enum_type,
        Element::Type *enum_int_type,
        llvm::Type *llvm_enum_int_type,
        int flinkage);
}
}

#endif
