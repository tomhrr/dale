#include "BasicTypes.h"
#include "Config.h"
#include "../Form/Utils/Utils.h"

#define ADD_INTF(name, fn) makeFunction(ctx, mod, once_tag, name, fn, type, type);
#define ADD_FLTF(name, fn) makeFloatFunction(ctx, mod, once_tag, name, fn, type, type);
#define ADD_CMPF(name, fn) makeFunction(ctx, mod, once_tag, name, fn, type_bool, type);

namespace dale
{
namespace BasicTypes
{
void
setStandardAttributes(llvm::Function *fn)
{
#if D_LLVM_VERSION_MINOR == 2
    fn->addFnAttr(llvm::Attributes::NoUnwind);
    fn->addFnAttr(llvm::Attributes::ReadOnly);
    fn->addFnAttr(llvm::Attributes::AlwaysInline);
#else
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    fn->addFnAttr(llvm::Attribute::ReadOnly);
    fn->addFnAttr(llvm::Attribute::AlwaysInline);
#endif
}

Function *
addSimpleFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                  const char *name, int linkage, std::string *symbol_name,
                  Type *return_type, std::vector<Variable *> *params)
{
    std::vector<llvm::Type *> llvm_param_types;
    for (std::vector<Variable *>::iterator b = params->begin(),
                                           e = params->end();
            b != e;
            ++b) {
        llvm::Type *arg_type = ctx->toLLVMType((*b)->type, NULL, false);
        assert(arg_type && "unable to resolve argument type");
        llvm_param_types.push_back(arg_type);
    }

    llvm::Type *llvm_ret_type = ctx->toLLVMType(return_type, NULL, false);
    assert(llvm_ret_type && "unable to resolve return type");

    llvm::ArrayRef<llvm::Type*> llvm_array_param_types(llvm_param_types);
    llvm::FunctionType *ft =
        llvm::FunctionType::get(llvm_ret_type, llvm_array_param_types, false);

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name, &new_name, linkage, params);

    llvm::Function *llvm_fn =
        llvm::cast<llvm::Function>(
            mod->getOrInsertFunction(new_name.c_str(), ft)
        );

    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(llvm::GlobalValue::WeakODRLinkage);
    setStandardAttributes(llvm_fn);

    Function *fn = new Function(return_type, params, llvm_fn,
                                0, &new_name);
    fn->linkage = linkage;
    fn->once_tag = *once_tag;

    ctx->ns()->addFunction(name, fn, NULL);
    linkVariablesToFunction(params, llvm_fn);

    return fn;
}

Function *
addSimpleUnaryFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                       const char *name, Type *return_type, Type *type1)
{
    int linkage = Linkage::Extern;

    type1 = ctx->tr->getConstType(type1);

    std::vector<Variable*> new_args_ctx;
    new_args_ctx.push_back(new Variable((char *) "a", type1));

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name, &new_name, linkage, &new_args_ctx);

    return addSimpleFunction(ctx, mod, once_tag, name, linkage,
                             &new_name, return_type, &new_args_ctx);
}

Function *
addSimpleBinaryFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                        const char *name, Type *return_type, Type *type1,
                        Type *type2)
{
    int linkage = Linkage::Extern;

    type1 = ctx->tr->getConstType(type1);
    type2 = ctx->tr->getConstType(type2);

    std::vector<Variable *> new_args_ctx;
    new_args_ctx.push_back(new Variable((char *) "a", type1));
    new_args_ctx.push_back(new Variable((char *) "b", type2));

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name, &new_name, linkage, &new_args_ctx);

    return addSimpleFunction(ctx, mod, once_tag, name, linkage,
                             &new_name, return_type, &new_args_ctx);
}

void
makeFloatFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                  const char *name,
                  llvm::Value* (llvm::IRBuilder<>:: *method_name)
                      (llvm::Value*, llvm::Value*, const llvm::Twine &
#if D_LLVM_VERSION_MINOR >= 2
                      , llvm::MDNode *
#endif
                      ),
                  Type *ret_type, Type *type)
{
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))((*iter)->value,
                                       (*(iter + 1))->value,
                                       unused_twine
#if D_LLVM_VERSION_MINOR >= 2
                                       , NULL
#endif
                                       )
        );

    builder.CreateRet(ret_val);
}

void
makeFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
             const char *name,
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &),
             Type *ret_type, Type *type)
{
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))((*iter)->value,
                                       (*(iter + 1))->value,
                                       unused_twine)
        );
    builder.CreateRet(ret_val);
}

void
makeFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
             const char *name,
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &, bool),
             Type *ret_type, Type *type)
{
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))((*iter)->value,
                                       (*(iter + 1))->value,
                                       unused_twine,
                                       false)
        );
    builder.CreateRet(ret_val);
}

void
makeFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
             const char *name,
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &, bool, bool),
             Type *ret_type, Type *type)
{
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))((*iter)->value,
                                       (*(iter + 1))->value,
                                       unused_twine,
                                       false,
                                       true)
        );
    builder.CreateRet(ret_val);
}

void
makeFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
             const char *name,
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &, bool, bool),
             Type *ret_type, Type *type1, Type *type2)
{
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type1, type2);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))((*iter)->value,
                                       (*(iter + 1))->value,
                                       unused_twine,
                                       false,
                                       true)
        );
    builder.CreateRet(ret_val);
}

void
makeShlFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                Type *type)
{
    Type *type_int  = ctx->tr->type_int;

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, "<<",
                                           type, type, type_int);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    ParseResult cast_pr;
    Operation::Cast(ctx, block, (*(iter + 1))->value,
                    type_int, type, NULL, false, &cast_pr);

    llvm::IRBuilder<> builder(cast_pr.block);
    llvm::Twine unused_twine;
    llvm::Value *res =
        llvm::cast<llvm::Value>(
            builder.CreateShl((*iter)->value, cast_pr.getValue(ctx),
                              unused_twine)
        );
    builder.CreateRet(res);
}

void
makeShrFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                Type *type)
{
    Type *type_int  = ctx->tr->type_int;

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, ">>",
                                           type, type, type_int);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    ParseResult cast_pr;
    Operation::Cast(ctx, block, (*(iter + 1))->value,
                    type_int, type, NULL, false, &cast_pr);

    llvm::IRBuilder<> builder(cast_pr.block);
    llvm::Twine unused_twine;
    llvm::Value *res =
        llvm::cast<llvm::Value>(
            builder.CreateLShr((*iter)->value, cast_pr.getValue(ctx),
                               unused_twine)
        );
    builder.CreateRet(res);
}

void
addSignedInt(Context *ctx, llvm::Module *mod, std::string *once_tag,
             Type *type)
{
    Type *type_bool = ctx->tr->type_bool;

    ADD_INTF("+", &llvm::IRBuilder<>::CreateAdd);
    ADD_INTF("-", &llvm::IRBuilder<>::CreateSub);
    ADD_INTF("/", &llvm::IRBuilder<>::CreateSDiv);
    ADD_INTF("*", &llvm::IRBuilder<>::CreateMul);
    ADD_INTF("%", &llvm::IRBuilder<>::CreateSRem);

    ADD_INTF("&", &llvm::IRBuilder<>::CreateAnd);
    ADD_INTF("|", &llvm::IRBuilder<>::CreateOr);
    ADD_INTF("^", &llvm::IRBuilder<>::CreateXor);

    ADD_CMPF("=",  &llvm::IRBuilder<>::CreateICmpEQ);
    ADD_CMPF("!=", &llvm::IRBuilder<>::CreateICmpNE);
    ADD_CMPF("<",  &llvm::IRBuilder<>::CreateICmpSLT);
    ADD_CMPF("<=", &llvm::IRBuilder<>::CreateICmpSLE);
    ADD_CMPF(">",  &llvm::IRBuilder<>::CreateICmpSGT);
    ADD_CMPF(">=", &llvm::IRBuilder<>::CreateICmpSGE);

    makeShlFunction(ctx, mod, once_tag, type);
    makeShrFunction(ctx, mod, once_tag, type);
}

void
makeNegateFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                   Type *type)
{
    Function *fn = addSimpleUnaryFunction(ctx, mod, once_tag, "~", type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return;
    }

    llvm::Value *zero = llvm::ConstantInt::get(llvm_type, 0);
    llvm::Value *max =
        builder.CreateSub(zero, llvm::ConstantInt::get(llvm_type, 1));

    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(
            builder.CreateXor((*iter)->value, max)
        );
    builder.CreateRet(ret_val);
}

void
addUnsignedInt(Context *ctx, llvm::Module *mod, std::string *once_tag,
               Type *type)
{
    Type *type_bool = ctx->tr->type_bool;

    ADD_INTF("+", &llvm::IRBuilder<>::CreateAdd);
    ADD_INTF("-", &llvm::IRBuilder<>::CreateSub);
    ADD_INTF("/", &llvm::IRBuilder<>::CreateUDiv);
    ADD_INTF("*", &llvm::IRBuilder<>::CreateMul);
    ADD_INTF("%", &llvm::IRBuilder<>::CreateURem);

    ADD_INTF("&", &llvm::IRBuilder<>::CreateAnd);
    ADD_INTF("|", &llvm::IRBuilder<>::CreateOr);
    ADD_INTF("^", &llvm::IRBuilder<>::CreateXor);

    ADD_CMPF("=",  &llvm::IRBuilder<>::CreateICmpEQ);
    ADD_CMPF("!=", &llvm::IRBuilder<>::CreateICmpNE);
    ADD_CMPF("<",  &llvm::IRBuilder<>::CreateICmpULT);
    ADD_CMPF("<=", &llvm::IRBuilder<>::CreateICmpULE);
    ADD_CMPF(">",  &llvm::IRBuilder<>::CreateICmpUGT);
    ADD_CMPF(">=", &llvm::IRBuilder<>::CreateICmpUGE);

    makeShlFunction(ctx, mod, once_tag, type);
    makeShrFunction(ctx, mod, once_tag, type);
    makeNegateFunction(ctx, mod, once_tag, type);
}

void
addFloatingPoint(Context *ctx, llvm::Module *mod, std::string *once_tag,
                 Type *type)
{
    Type *type_bool = ctx->tr->type_bool;

    ADD_FLTF("+", &llvm::IRBuilder<>::CreateFAdd);
    ADD_FLTF("-", &llvm::IRBuilder<>::CreateFSub);
    ADD_FLTF("/", &llvm::IRBuilder<>::CreateFDiv);
    ADD_FLTF("*", &llvm::IRBuilder<>::CreateFMul);
    ADD_FLTF("%", &llvm::IRBuilder<>::CreateFRem);

    ADD_CMPF("=",  &llvm::IRBuilder<>::CreateFCmpOEQ);
    ADD_CMPF("!=", &llvm::IRBuilder<>::CreateFCmpONE);
    ADD_CMPF("<",  &llvm::IRBuilder<>::CreateFCmpOLT);
    ADD_CMPF("<=", &llvm::IRBuilder<>::CreateFCmpOLE);
    ADD_CMPF(">",  &llvm::IRBuilder<>::CreateFCmpOGT);
    ADD_CMPF(">=", &llvm::IRBuilder<>::CreateFCmpOGE);
}
}
}
