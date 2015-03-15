#include "BasicTypes.h"
#include "Config.h"

#define ADD_INTF(name, fn) makeFunction(ctx, mod, once_tag, name, fn, type, type);
#define ADD_FLTF(name, fn) makeFloatFunction(ctx, mod, once_tag, name, fn, type, type);
#define ADD_ENMF(name, fn) makeEnumFunction(ctx, mod, once_tag, name, fn, enum_type, enum_type, linkage, llvm_enum_int_type);
#define ADD_CMPF(name, fn) makeFunction(ctx, mod, once_tag, name, fn, type_bool, type);
#define ADD_ENMCMPF(name, fn) makeEnumFunction(ctx, mod, once_tag, name, fn, type_bool, enum_type, linkage);

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
        if (!arg_type) {
            return NULL;
        }
        llvm_param_types.push_back(arg_type);
    }

    llvm::Type *llvm_ret_type = ctx->toLLVMType(return_type, NULL, false);
    if (!llvm_ret_type) {
        return NULL;
    }

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
    llvm_fn->setLinkage(llvm::GlobalValue::LinkOnceAnyLinkage);
    setStandardAttributes(llvm_fn);

    Function *fn = new Function(return_type, params, llvm_fn,
                                0, &new_name);
    fn->linkage = linkage;
    fn->once_tag = *once_tag;

    ctx->ns()->addFunction(name, fn, NULL);

    llvm::Function::arg_iterator llvm_arg_iter = llvm_fn->arg_begin();
    for (std::vector<Variable *>::iterator b = params->begin(),
                                           e = params->end();
            b != e;
            ++b) {
        llvm::Value *llvm_value = llvm_arg_iter;
        ++llvm_arg_iter;
        llvm_value->setName((*b)->name.c_str());
        (*b)->value = llvm_value;
    }

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
    iter = fn->parameter_types.begin();

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
    iter = fn->parameter_types.begin();

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
    iter = fn->parameter_types.begin();

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
    iter = fn->parameter_types.begin();

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
    iter = fn->parameter_types.begin();

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
makeEnumFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                 const char *name,
                 llvm::Value* (llvm::IRBuilder<>:: *method_name)
                     (llvm::Value*, llvm::Value*, const llvm::Twine &,
                      bool, bool),
                 Type *ret_type, Type *type, int linkage,
                 llvm::Type *llvm_enum_int_type)
{
    std::vector<llvm::Value *> two_zero_indices;
    llvm::Value *llvm_native_zero = ctx->nt->getNativeInt(0);
    two_zero_indices.push_back(llvm_native_zero);
    two_zero_indices.push_back(llvm_native_zero);

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           type, type, type);
    fn->linkage = linkage;
    fn->llvm_function->setLinkage(ctx->toLLVMLinkage(linkage));

    std::vector<Variable *>::iterator iter;
    iter = fn->parameter_types.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);

    llvm::Value *new_ptr1 =
        llvm::cast<llvm::Value>(
            builder.CreateAlloca(ctx->toLLVMType(type, NULL, false))
        );
    llvm::Value *new_ptr2 =
        llvm::cast<llvm::Value>(
            builder.CreateAlloca(ctx->toLLVMType(type, NULL, false))
        );
    builder.CreateStore((*iter)->value,       new_ptr1);
    builder.CreateStore((*(iter + 1))->value, new_ptr2);

    llvm::Value *one =
        builder.CreateLoad(
            builder.CreateGEP(new_ptr1,
                              llvm::ArrayRef<llvm::Value*>(
                                  two_zero_indices
                              ))
        );
    llvm::Value *two =
        builder.CreateLoad(
            builder.CreateGEP(new_ptr2,
                              llvm::ArrayRef<llvm::Value*>(
                                  two_zero_indices
                              ))
        );

    llvm::Twine unused_twine;
    llvm::Value *res =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))(one, two, unused_twine, false,
                                       true)
        );

    llvm::Value *store_ptr =
        llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_enum_int_type));
    builder.CreateStore(res, store_ptr);

    llvm::Value *store_ptr_cast =
        builder.CreateBitCast(
            store_ptr, ctx->toLLVMType(ctx->tr->getPointerType(type),
                                       NULL, false)
        );
    llvm::Value *ret_val = builder.CreateLoad(store_ptr_cast);
    builder.CreateRet(ret_val);
}

void
makeEnumFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                 const char *name,
                 llvm::Value* (llvm::IRBuilder<>:: *method_name)
                     (llvm::Value*, llvm::Value*, const llvm::Twine &,
                      bool),
                 Type *ret_type, Type *type, int linkage,
                 llvm::Type *llvm_enum_int_type)
{
    std::vector<llvm::Value *> two_zero_indices;
    llvm::Value *llvm_native_zero = ctx->nt->getNativeInt(0);
    two_zero_indices.push_back(llvm_native_zero);
    two_zero_indices.push_back(llvm_native_zero);

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           type, type, type);
    fn->linkage = linkage;
    fn->llvm_function->setLinkage(ctx->toLLVMLinkage(linkage));

    std::vector<Variable *>::iterator iter;
    iter = fn->parameter_types.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);

    llvm::Value *new_ptr1 =
        llvm::cast<llvm::Value>(
            builder.CreateAlloca(ctx->toLLVMType(type, NULL, false))
        );
    llvm::Value *new_ptr2 =
        llvm::cast<llvm::Value>(
            builder.CreateAlloca(ctx->toLLVMType(type, NULL, false))
        );
    builder.CreateStore((*iter)->value,       new_ptr1);
    builder.CreateStore((*(iter + 1))->value, new_ptr2);

    llvm::Value *one =
        builder.CreateLoad(
            builder.CreateGEP(new_ptr1,
                              llvm::ArrayRef<llvm::Value*>(
                                  two_zero_indices
                              ))
        );
    llvm::Value *two =
        builder.CreateLoad(
            builder.CreateGEP(new_ptr2,
                              llvm::ArrayRef<llvm::Value*>(
                                  two_zero_indices
                              ))
        );

    llvm::Twine unused_twine;
    llvm::Value *res =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))(one, two, unused_twine, false)
        );

    llvm::Value *store_ptr =
        llvm::cast<llvm::Value>(builder.CreateAlloca(llvm_enum_int_type));
    builder.CreateStore(res, store_ptr);

    llvm::Value *store_ptr_cast =
        builder.CreateBitCast(
            store_ptr, ctx->toLLVMType(ctx->tr->getPointerType(type),
                                       NULL, false)
        );
    llvm::Value *ret_val = builder.CreateLoad(store_ptr_cast);
    builder.CreateRet(ret_val);
}

void
makeEnumFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                 const char *name,
                 llvm::Value* (llvm::IRBuilder<>:: *method_name)
                     (llvm::Value*, llvm::Value*, const llvm::Twine &),
                 Type *ret_type, Type *type, int linkage)
{
    std::vector<llvm::Value *> two_zero_indices;
    llvm::Value *llvm_native_zero = ctx->nt->getNativeInt(0);
    two_zero_indices.push_back(llvm_native_zero);
    two_zero_indices.push_back(llvm_native_zero);

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);
    fn->linkage = linkage;
    fn->llvm_function->setLinkage(ctx->toLLVMLinkage(linkage));

    std::vector<Variable *>::iterator iter;
    iter = fn->parameter_types.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);

    llvm::Value *new_ptr1 =
        llvm::cast<llvm::Value>(
            builder.CreateAlloca(ctx->toLLVMType(type, NULL, false))
        );
    llvm::Value *new_ptr2 =
        llvm::cast<llvm::Value>(
            builder.CreateAlloca(ctx->toLLVMType(type, NULL, false))
        );
    builder.CreateStore((*iter)->value,       new_ptr1);
    builder.CreateStore((*(iter + 1))->value, new_ptr2);

    llvm::Value *one =
        builder.CreateLoad(
            builder.CreateGEP(new_ptr1,
                              llvm::ArrayRef<llvm::Value*>(
                                  two_zero_indices
                              ))
        );
    llvm::Value *two =
        builder.CreateLoad(
            builder.CreateGEP(new_ptr2,
                              llvm::ArrayRef<llvm::Value*>(
                                  two_zero_indices
                              ))
        );

    llvm::Twine unused_twine;
    llvm::Value *res =
        llvm::cast<llvm::Value>(
            ((builder).*(method_name))(one, two, unused_twine)
        );

    if (ret_type != type) {
        builder.CreateRet(res);
    } else {
        llvm::Value *store_ptr =
            llvm::cast<llvm::Value>(
                builder.CreateAlloca(res->getType())
            );
        builder.CreateStore(res, store_ptr);
        llvm::Value *ptr_val =
            builder.CreateBitCast(
                store_ptr, ctx->toLLVMType(ctx->tr->getPointerType(type),
                                           NULL, false)
            );
        llvm::Value *ret_val = builder.CreateLoad(ptr_val);
        builder.CreateRet(ret_val);
    }
}

void
makeShlFunction(Context *ctx, llvm::Module *mod, std::string *once_tag,
                Type *type)
{
    Type *type_int  = ctx->tr->type_int;

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, "<<",
                                           type, type, type_int);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameter_types.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    ParseResult pr_cast;
    Operation::Cast(ctx, block, (*(iter + 1))->value,
                    type_int, type, NULL, false, &pr_cast);

    llvm::IRBuilder<> builder(pr_cast.block);
    llvm::Twine unused_twine;
    llvm::Value *res =
        llvm::cast<llvm::Value>(
            builder.CreateShl((*iter)->value, pr_cast.value, unused_twine)
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
    iter = fn->parameter_types.begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    ParseResult pr_cast;
    Operation::Cast(ctx, block, (*(iter + 1))->value,
                    type_int, type, NULL, false, &pr_cast);

    llvm::IRBuilder<> builder(pr_cast.block);
    llvm::Twine unused_twine;
    llvm::Value *res =
        llvm::cast<llvm::Value>(
            builder.CreateLShr((*iter)->value, pr_cast.value, unused_twine)
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
    iter = fn->parameter_types.begin();

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

    ADD_CMPF("=",  &llvm::IRBuilder<>::CreateFCmpOEQ);
    ADD_CMPF("!=", &llvm::IRBuilder<>::CreateFCmpONE);
    ADD_CMPF("<",  &llvm::IRBuilder<>::CreateFCmpOLT);
    ADD_CMPF("<=", &llvm::IRBuilder<>::CreateFCmpOLE);
    ADD_CMPF(">",  &llvm::IRBuilder<>::CreateFCmpOGT);
    ADD_CMPF(">=", &llvm::IRBuilder<>::CreateFCmpOGE);
}

void
addEnum(Context *ctx,
        llvm::Module *mod,
        std::string *once_tag,
        Type *enum_type,
        Type *enum_int_type,
        llvm::Type *llvm_enum_int_type,
        int linkage)
{
    std::vector<llvm::Value *> two_zero_indices;
    llvm::Value *llvm_native_zero = ctx->nt->getNativeInt(0);
    two_zero_indices.push_back(llvm_native_zero);
    two_zero_indices.push_back(llvm_native_zero);

    Type *type_bool = ctx->tr->type_bool;

    ADD_ENMF("+", &llvm::IRBuilder<>::CreateAdd);
    ADD_ENMF("-", &llvm::IRBuilder<>::CreateSub);
    ADD_ENMF("*", &llvm::IRBuilder<>::CreateMul);
    ADD_ENMF("/", &llvm::IRBuilder<>::CreateSDiv);

    ADD_ENMCMPF("=",  &llvm::IRBuilder<>::CreateICmpEQ);
    ADD_ENMCMPF("!=", &llvm::IRBuilder<>::CreateICmpNE);
    ADD_ENMCMPF("<",  &llvm::IRBuilder<>::CreateICmpSLT);
    ADD_ENMCMPF("<=", &llvm::IRBuilder<>::CreateICmpSLE);
    ADD_ENMCMPF(">",  &llvm::IRBuilder<>::CreateICmpSGT);
    ADD_ENMCMPF(">=", &llvm::IRBuilder<>::CreateICmpSGE);

    Type *type_int = ctx->tr->type_int;

    {
        Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "<<", enum_type, enum_type, type_int);
        fn->linkage = linkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(linkage)
        );

        std::vector<Variable *>::iterator iter;
        iter = fn->parameter_types.begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);

        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        builder.CreateStore((*iter)->value,
                            new_ptr1);

        llvm::Value *one =
            builder.CreateLoad(
                builder.CreateGEP(new_ptr1,
                                  llvm::ArrayRef<llvm::Value*>(
                                      two_zero_indices
                                  ))
            );

        ParseResult temp;
        Operation::Cast(ctx, block, (*(iter + 1))->value,
                                 type_int, enum_int_type, NULL, false, &temp);
        builder.SetInsertPoint(temp.block);

        llvm::Twine unused_twine;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateShl(one, temp.value, unused_twine)
                           );
        llvm::Value *store_ptr1 = llvm::cast<llvm::Value>(
                                      builder.CreateAlloca(llvm_enum_int_type)
                                  );
        builder.CreateStore(res,
                            store_ptr1);

        llvm::Value *sp =
            builder.CreateBitCast(
                store_ptr1, ctx->toLLVMType(ctx->tr->getPointerType(enum_type),
                                            NULL, false)
            );
        llvm::Value *newint =
            builder.CreateLoad(sp);
        builder.CreateRet(newint);
    }

    {
        Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, ">>", enum_type, enum_type, type_int);
        fn->linkage = linkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(linkage)
        );

        std::vector<Variable *>::iterator iter;
        iter = fn->parameter_types.begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);

        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        builder.CreateStore((*iter)->value,
                            new_ptr1);

        llvm::Value *one =
            builder.CreateLoad(
                builder.CreateGEP(new_ptr1,
                                  llvm::ArrayRef<llvm::Value*>(
                                      two_zero_indices
                                  ))
            );

        ParseResult temp;
        Operation::Cast(ctx, block, (*(iter + 1))->value,
                                 type_int, enum_int_type, NULL, false, &temp);
        builder.SetInsertPoint(temp.block);

        llvm::Twine unused_twine;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateLShr(one, temp.value, unused_twine)
                           );
        llvm::Value *store_ptr1 = llvm::cast<llvm::Value>(
                                      builder.CreateAlloca(llvm_enum_int_type)
                                  );
        builder.CreateStore(res,
                            store_ptr1);

        llvm::Value *sp =
            builder.CreateBitCast(
                store_ptr1, ctx->toLLVMType(ctx->tr->getPointerType(enum_type),
                                            NULL, false)
            );
        llvm::Value *newint =
            builder.CreateLoad(sp);
        builder.CreateRet(newint);
    }

    /*
    No more structural equivalence, so make_enum_function needs to not
    return a value of the underlying enum type here.  Just disabling
    it for now, because they aren't used anywhere important, IIRC.
    (This has since been fixed.)
    */

    makeEnumFunction(ctx, mod, once_tag, "&",  &llvm::IRBuilder<>::CreateAnd,     enum_type,       enum_type, linkage);
    makeEnumFunction(ctx, mod, once_tag, "|",  &llvm::IRBuilder<>::CreateOr,      enum_type,       enum_type, linkage);
    makeEnumFunction(ctx, mod, once_tag, "^",  &llvm::IRBuilder<>::CreateXor,     enum_type,       enum_type, linkage);

    return;
}
}
}
