#ifndef DALE_BASICTYPES
#define DALE_BASICTYPES

#include "../Element/Type/Type.h"
#include "../Context/Context.h"
#include "../ParseResult/ParseResult.h"
#include "../Operation/Cast/Cast.h"

#include "../llvm_LLVMContext.h"
#include "../llvm_Module.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Linker.h"
#include "../llvm_Function.h"
#include "llvm/PassManager.h"
#include "../llvm_CallingConv.h"

namespace dale
{
namespace BasicTypes
{

Element::Function *
addSimpleUnaryFunction(Context *ctx,
                       llvm::Module *mod,
                       std::string *once_tag,
                       const char *name,
                       Element::Type *return_type,
                       Element::Type *type1);

Element::Function *
addSimpleBinaryFunction(Context *ctx,
                        llvm::Module *mod,
                        std::string *once_tag,
                        const char *name,
                        Element::Type *return_type,
                        Element::Type *type1,
                        Element::Type *type2);

void
makeFunction(Context *ctx,
             llvm::Module *mod,
             std::string *once_tag,
             const char *name,
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &),
             Element::Type *ret_type,
             Element::Type *type);

void
makeEnumFunction(Context *ctx,
                 llvm::Module *mod,
                 std::string *once_tag,
                 const char *name,
                 llvm::Value* (llvm::IRBuilder<>:: *method_name)
                     (llvm::Value*, llvm::Value*, const llvm::Twine &),
                 Element::Type *ret_type,
                 Element::Type *type,
                 int mylinkage);

void
addSignedInt(Context *ctx,
             llvm::Module *mod,
             std::string *once_tag,
             Element::Type *type);

void
addFloatingPoint(Context *ctx,
                 llvm::Module *mod,
                 std::string *once_tag,
                 Element::Type *type);

void
addUnsignedInt(Context *ctx,
               llvm::Module *mod,
               std::string *once_tag,
               Element::Type *type);

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
