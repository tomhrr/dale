#include "BasicTypes.h"

namespace dale
{
namespace BasicTypes
{
void
setStandardAttributes(llvm::Function *fn)
{
#if LLVM_VERSION_MINOR == 2
    fn->addFnAttr(llvm::Attributes::NoUnwind);
    fn->addFnAttr(llvm::Attributes::ReadOnly);
    fn->addFnAttr(llvm::Attributes::AlwaysInline);
#else
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    fn->addFnAttr(llvm::Attribute::ReadOnly);
    fn->addFnAttr(llvm::Attribute::AlwaysInline);
#endif
}

Element::Function *
addSimpleUnaryFunction(Context *ctx,
                       llvm::Module *mod,
                       std::string *once_tag, 
                       const char *name,
                       Element::Type *return_type,
                       Element::Type *type1)
{
    int linkage = Linkage::Extern;

    type1 = ctx->tr->getConstType(type1);

    std::vector<Element::Variable*> *new_args_ctx =
        new std::vector<Element::Variable*>;

    new_args_ctx->push_back(
        new Element::Variable((char *) "a", type1));

    std::vector<llvm::Type*> new_args;

    llvm::Type *llvm_type1 = ctx->toLLVMType(type1, NULL, false);
    if (!llvm_type1) {
        return NULL;
    }
    new_args.push_back(llvm_type1);

    llvm::Type *llvm_ret_type =
        ctx->toLLVMType(return_type, NULL, false);
    if (!llvm_ret_type) {
        return NULL;
    }
    
    llvm::ArrayRef<llvm::Type*> llvm_new_args(new_args);
    llvm::FunctionType *new_ft =
        llvm::FunctionType::get(
            llvm_ret_type,
            llvm_new_args,
            false
        );

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name,
                                    &new_name,
                                    linkage,
                                    new_args_ctx);

    llvm::Function *new_fn =
        llvm::cast<llvm::Function>(
            mod->getOrInsertFunction(new_name.c_str(), new_ft)
        );

    new_fn->setCallingConv(llvm::CallingConv::C);
    new_fn->setLinkage(llvm::GlobalValue::LinkOnceAnyLinkage);
    setStandardAttributes(new_fn);

    Element::Function *myfn =
        new Element::Function(
        return_type,
        new_args_ctx,
        new_fn,
        0,
        &new_name
    );
    myfn->linkage = linkage;
    myfn->once_tag = *once_tag;

    ctx->ns()->addFunction(name, myfn, NULL);

    llvm::Function::arg_iterator largs = new_fn->arg_begin();
    std::vector<Element::Variable *>::iterator iter;
    iter = new_args_ctx->begin();

    while (iter != new_args_ctx->end()) {
        llvm::Value *temp = largs;
        ++largs;
        temp->setName((*iter)->name.c_str());
        (*iter)->value = temp;
        ++iter;
    }

    return myfn;
}

Element::Function *
addSimpleBinaryFunction(Context *ctx,
                        llvm::Module *mod,
                        std::string *once_tag, 
                        const char *name,
                        Element::Type *return_type,
                        Element::Type *type1,
                        Element::Type *type2)
{
    int linkage = Linkage::Extern;

    type1 = ctx->tr->getConstType(type1);
    type2 = ctx->tr->getConstType(type2);

    std::vector<Element::Variable*> *new_args_ctx =
        new std::vector<Element::Variable*>;
    new_args_ctx->reserve(2);

    new_args_ctx->push_back(
        new Element::Variable((char *) "a", type1));
    new_args_ctx->push_back(
        new Element::Variable((char *) "b", type2));

    std::vector<llvm::Type*> new_args;
    new_args.reserve(2);
    llvm::Type *llvm_type1 = ctx->toLLVMType(type1, NULL, false);
    if (!llvm_type1) {
        return NULL;
    }
    llvm::Type *llvm_type2 = ctx->toLLVMType(type2, NULL, false);
    if (!llvm_type2) {
        return NULL;
    }
    new_args.push_back(llvm_type1);
    new_args.push_back(llvm_type2);

    std::string new_name;
    new_name.reserve(20);
    ctx->ns()->functionNameToSymbol(name,
                                    &new_name,
                                    linkage,
                                    new_args_ctx);

    llvm::Type *llvm_return_type =
        ctx->toLLVMType(return_type, NULL, false);
    if (!llvm_return_type) {
        return NULL;
    }

    llvm::ArrayRef<llvm::Type*> llvm_new_args(new_args);
    llvm::FunctionType *new_ft =
        llvm::FunctionType::get(
            llvm_return_type,
            llvm_new_args,
            false
        );

    llvm::Function *new_fn =
        llvm::cast<llvm::Function>(
            mod->getOrInsertFunction(new_name.c_str(), new_ft)
        );

    new_fn->setCallingConv(llvm::CallingConv::C);
    new_fn->setLinkage(llvm::GlobalValue::LinkOnceAnyLinkage);
    setStandardAttributes(new_fn);

    Element::Function *myfn =
        new Element::Function(
            return_type,
            new_args_ctx,
            new_fn,
            0,
            &new_name
        );
    myfn->linkage = linkage;
    myfn->once_tag = *once_tag;

    ctx->ns()->addFunction(name, myfn, NULL);

    llvm::Function::arg_iterator largs = new_fn->arg_begin();
    std::vector<Element::Variable *>::iterator iter;
    iter = new_args_ctx->begin();

    while (iter != new_args_ctx->end()) {
        llvm::Value *temp = largs;
        ++largs;
        temp->setName((*iter)->name.c_str());
        (*iter)->value = temp;
        ++iter;
    }

    return myfn;
}

void
makeFunction(Context *ctx,
             llvm::Module *mod,
             std::string *once_tag, 
             const char *name,
#if LLVM_VERSION_MINOR >= 2
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &, llvm::MDNode *),
#else
             llvm::Value* (llvm::IRBuilder<>:: *method_name)
                (llvm::Value*, llvm::Value*, const llvm::Twine &),
#endif
             Element::Type *ret_type,
             Element::Type *type)
{
    Element::Function *fn =
        addSimpleBinaryFunction(ctx, mod, once_tag, 
                                name, ret_type, type, type);

    std::vector<Element::Variable *>::iterator iter;
    iter = fn->parameter_types->begin();

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine bling;
#if LLVM_VERSION_MINOR >= 2
    llvm::Value *res = llvm::cast<llvm::Value>(
                           ((builder).*(method_name))((*iter)->value, (*(iter +
                                   1))->value, bling, NULL));
#else
    llvm::Value *res = llvm::cast<llvm::Value>(
                           ((builder).*(method_name))((*iter)->value, (*(iter +
                                   1))->value, bling));
#endif
    builder.CreateRet(res);
}

void 
makeEnumFunction(Context *ctx,
                 llvm::Module *mod,
                 std::string *once_tag, 
                 const char *name,
                 llvm::Value* (llvm::IRBuilder<>:: *method_name)
                     (llvm::Value*, llvm::Value*, const llvm::Twine &),
                 Element::Type *ret_type,
                 Element::Type *type,
                 int mylinkage)
{
    std::vector<llvm::Value *> two_zero_indices;
    llvm::Value *llvm_native_zero = ctx->nt->getNativeInt(0);
    two_zero_indices.push_back(llvm_native_zero);
    two_zero_indices.push_back(llvm_native_zero);

    Element::Function *fn =
        addSimpleBinaryFunction(ctx, mod, once_tag, name, 
                                ret_type, type, type);
    fn->linkage = mylinkage;
    fn->llvm_function->setLinkage(
        ctx->toLLVMLinkage(mylinkage)
    );

    std::vector<Element::Variable *>::iterator iter;
    iter = fn->parameter_types->begin();

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
    builder.CreateStore((*iter)->value,
                        new_ptr1);
    builder.CreateStore((*(iter + 1))->value,
                        new_ptr2);

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

    llvm::Twine bling;
    llvm::Value *res = llvm::cast<llvm::Value>(
                           ((builder).*(method_name))(one, two, bling)
                       );

    if (ret_type != type) {
        /* It is assumed in this case that the method provided
         * will create a value of ret_type. */
        builder.CreateRet(res);
    } else {
        /* Otherwise, store the value created by the method,
         * bitcast to a pointer to type and load that value. */
        llvm::Value *store_ptr1 = llvm::cast<llvm::Value>(
                                      builder.CreateAlloca(res->getType())
                                  );
        builder.CreateStore(res,
                            store_ptr1);
        llvm::Value *sp =
            builder.CreateBitCast(
                store_ptr1, ctx->toLLVMType(ctx->tr->getPointerType(type),
                                            NULL, false)
            );
        llvm::Value *newint =
            builder.CreateLoad(sp);
        builder.CreateRet(newint);
    }
}

void
addSignedInt(Context *ctx,
             llvm::Module *mod,
             std::string *once_tag, 
             Element::Type *type)
{
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag,
                                    "+", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateAdd((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag,
                                    "-", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateSub((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag,
                                    "/", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateSDiv((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag,
                                    "*", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateMul((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }

    Element::Type *type_bool = ctx->tr->type_bool;
    Element::Type *type_int  = ctx->tr->type_int;

    makeFunction(ctx, mod, once_tag, "=",  
                 &llvm::IRBuilder<>::CreateICmpEQ,  type_bool, type);
    makeFunction(ctx, mod, once_tag, "!=", 
                 &llvm::IRBuilder<>::CreateICmpNE,  type_bool, type);
    makeFunction(ctx, mod, once_tag, "<",  
                 &llvm::IRBuilder<>::CreateICmpSLT, type_bool, type);
    makeFunction(ctx, mod, once_tag, "<=", 
                 &llvm::IRBuilder<>::CreateICmpSLE, type_bool, type);
    makeFunction(ctx, mod, once_tag, ">",  
                 &llvm::IRBuilder<>::CreateICmpSGT, type_bool, type);
    makeFunction(ctx, mod, once_tag, ">=", 
                 &llvm::IRBuilder<>::CreateICmpSGE, type_bool, type);

    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag,
                                    "<<", type, type, type_int);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        ParseResult temp;
        Operation::Cast::execute(ctx, block, (*(iter + 1))->value, 
                                 type_int, type, NULL, false, &temp);

        llvm::IRBuilder<> builder(temp.block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateShl((*iter)->value, temp.value,
                                                 bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag,
                                    ">>", type, type, type_int);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        ParseResult temp;
        Operation::Cast::execute(ctx, block, (*(iter + 1))->value, 
                                 type_int, type, NULL, false, &temp);

        llvm::IRBuilder<> builder(temp.block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateLShr((*iter)->value,
                                                  temp.value, bling)
                           );
        builder.CreateRet(res);
    }
    makeFunction(ctx, mod, once_tag, "&",  &llvm::IRBuilder<>::CreateAnd,     type,      type);
    makeFunction(ctx, mod, once_tag, "|",  &llvm::IRBuilder<>::CreateOr,      type,      type);
    makeFunction(ctx, mod, once_tag, "^",  &llvm::IRBuilder<>::CreateXor,     type,      type);
    return;
}

void
addFloatingPoint(Context *ctx,
                 llvm::Module *mod,
                 std::string *once_tag,
                 Element::Type *type)
{
    makeFunction(ctx, mod, once_tag, "+",  &llvm::IRBuilder<>::CreateFAdd, type, type);
    makeFunction(ctx, mod, once_tag, "-",  &llvm::IRBuilder<>::CreateFSub, type, type);
    makeFunction(ctx, mod, once_tag, "/",  &llvm::IRBuilder<>::CreateFDiv, type, type);
    makeFunction(ctx, mod, once_tag, "*",  &llvm::IRBuilder<>::CreateFMul, type, type);
    Element::Type *type_bool = ctx->tr->type_bool;
    makeFunction(ctx, mod, once_tag, "=",  &llvm::IRBuilder<>::CreateFCmpOEQ, type_bool, type);
    makeFunction(ctx, mod, once_tag, "!=", &llvm::IRBuilder<>::CreateFCmpONE, type_bool, type);
    makeFunction(ctx, mod, once_tag, "<",  &llvm::IRBuilder<>::CreateFCmpOLT, type_bool, type);
    makeFunction(ctx, mod, once_tag, "<=", &llvm::IRBuilder<>::CreateFCmpOLE, type_bool, type);
    makeFunction(ctx, mod, once_tag, ">",  &llvm::IRBuilder<>::CreateFCmpOGT, type_bool, type);
    makeFunction(ctx, mod, once_tag, ">=", &llvm::IRBuilder<>::CreateFCmpOGE, type_bool, type);
    return;
}

void 
addUnsignedInt(Context *ctx,
               llvm::Module *mod,
               std::string *once_tag,
               Element::Type *type)
{
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "+", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateAdd((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "-", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateSub((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "/", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateSDiv((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "*", type, type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateMul((*iter)->value,
                                       (*(iter + 1))->value,
                                       bling)
                           );
        builder.CreateRet(res);
    }
    Element::Type *type_bool = ctx->tr->type_bool;
    Element::Type *type_int  = ctx->tr->type_int;
    makeFunction(ctx, mod, once_tag, "=",  &llvm::IRBuilder<>::CreateICmpEQ,  type_bool, type);
    makeFunction(ctx, mod, once_tag, "!=", &llvm::IRBuilder<>::CreateICmpNE,  type_bool, type);
    makeFunction(ctx, mod, once_tag, "<",  &llvm::IRBuilder<>::CreateICmpULT, type_bool, type);
    makeFunction(ctx, mod, once_tag, "<=", &llvm::IRBuilder<>::CreateICmpULE, type_bool, type);
    makeFunction(ctx, mod, once_tag, ">",  &llvm::IRBuilder<>::CreateICmpUGT, type_bool, type);
    makeFunction(ctx, mod, once_tag, ">=", &llvm::IRBuilder<>::CreateICmpUGE, type_bool, type);
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "<<", type, type, type_int);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        ParseResult temp;
        Operation::Cast::execute(ctx, block, (*(iter + 1))->value, 
                                 type_int, type, NULL, false, &temp);

        llvm::IRBuilder<> builder(temp.block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateShl((*iter)->value, temp.value,
                                                 bling)
                           );
        builder.CreateRet(res);
    }
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, ">>", type, type, type_int);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        ParseResult temp;
        Operation::Cast::execute(ctx, block, (*(iter + 1))->value, 
                                 type_int, type, NULL, false, &temp);

        llvm::IRBuilder<> builder(temp.block);
        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateLShr((*iter)->value, 
                                                  temp.value, bling)
                           );
        builder.CreateRet(res);
    }
    makeFunction(ctx, mod, once_tag, "&",  &llvm::IRBuilder<>::CreateAnd,     type,      type);
    makeFunction(ctx, mod, once_tag, "|",  &llvm::IRBuilder<>::CreateOr,      type,      type);
    makeFunction(ctx, mod, once_tag, "^",  &llvm::IRBuilder<>::CreateXor,     type,      type);

    {
        Element::Function *fn =
            addSimpleUnaryFunction(ctx, mod, once_tag, "~", type, type);

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);
        llvm::IRBuilder<> builder(block);

        llvm::Type *llvm_type =
            ctx->toLLVMType(type, NULL, false);
        if (!llvm_type) {
            return;
        }

        llvm::Value *max =
            llvm::ConstantInt::get(llvm_type, 0);

        llvm::Value *real_max =
            builder.CreateSub(
                max,
                llvm::ConstantInt::get(llvm_type, 1)
            );

        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateXor((*iter)->value, real_max)
                           );
        builder.CreateRet(res);
    }

    return;
}

void 
addEnum(Context *ctx,
        llvm::Module *mod,
        std::string *once_tag,
        Element::Type *enum_type,
        Element::Type *enum_int_type,
        llvm::Type *llvm_enum_int_type,
        int flinkage)
{
    std::vector<llvm::Value *> two_zero_indices;
    llvm::Value *llvm_native_zero = ctx->nt->getNativeInt(0);
    two_zero_indices.push_back(llvm_native_zero);
    two_zero_indices.push_back(llvm_native_zero);

    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "+", enum_type, enum_type, enum_type);
        fn->linkage = flinkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(flinkage)
        );

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);

        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        builder.CreateStore((*iter)->value,
                            new_ptr1);
        builder.CreateStore((*(iter + 1))->value,
                            new_ptr2);

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

        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateAdd(one, two, bling)
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
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "-", enum_type, enum_type, enum_type);
        fn->linkage = flinkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(flinkage)
        );

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);

        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        builder.CreateStore((*iter)->value,
                            new_ptr1);
        builder.CreateStore((*(iter + 1))->value,
                            new_ptr2);

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

        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateSub(one, two, bling)
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
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "*", enum_type, enum_type, enum_type);
        fn->linkage = flinkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(flinkage)
        );

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);

        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        builder.CreateStore((*iter)->value,
                            new_ptr1);
        builder.CreateStore((*(iter + 1))->value,
                            new_ptr2);

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

        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateMul(one, two, bling)
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
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "/", enum_type, enum_type, enum_type);
        fn->linkage = flinkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(flinkage)
        );

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                     fn->llvm_function);

        llvm::IRBuilder<> builder(block);

        llvm::Value *new_ptr1 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        llvm::Value *new_ptr2 = llvm::cast<llvm::Value>(
                                    builder.CreateAlloca(ctx->toLLVMType(enum_type, NULL, false))
                                );
        builder.CreateStore((*iter)->value,
                            new_ptr1);
        builder.CreateStore((*(iter + 1))->value,
                            new_ptr2);

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

        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateSDiv(one, two, bling)
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
    Element::Type *type_bool = ctx->tr->type_bool;
    makeEnumFunction(ctx, mod, once_tag, "=",  &llvm::IRBuilder<>::CreateICmpEQ,  type_bool, enum_type, flinkage);
    makeEnumFunction(ctx, mod, once_tag, "!=", &llvm::IRBuilder<>::CreateICmpNE,  type_bool, enum_type, flinkage);
    makeEnumFunction(ctx, mod, once_tag, "<",  &llvm::IRBuilder<>::CreateICmpULT, type_bool, enum_type, flinkage);
    makeEnumFunction(ctx, mod, once_tag, "<=", &llvm::IRBuilder<>::CreateICmpULE, type_bool, enum_type, flinkage);
    makeEnumFunction(ctx, mod, once_tag, ">",  &llvm::IRBuilder<>::CreateICmpUGT, type_bool, enum_type, flinkage);
    makeEnumFunction(ctx, mod, once_tag, ">=", &llvm::IRBuilder<>::CreateICmpUGE, type_bool, enum_type, flinkage);
    Element::Type *type_int = ctx->tr->type_int;
    {
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, "<<", enum_type, enum_type, type_int);
        fn->linkage = flinkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(flinkage)
        );

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

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
        Operation::Cast::execute(ctx, block, (*(iter + 1))->value, 
                                 type_int, enum_int_type, NULL, false, &temp);
        builder.SetInsertPoint(temp.block);

        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateShl(one, temp.value, bling)
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
        Element::Function *fn =
            addSimpleBinaryFunction(ctx, mod, once_tag, ">>", enum_type, enum_type, type_int);
        fn->linkage = flinkage;
        fn->llvm_function->setLinkage(
            ctx->toLLVMLinkage(flinkage)
        );

        std::vector<Element::Variable *>::iterator iter;
        iter = fn->parameter_types->begin();

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
        Operation::Cast::execute(ctx, block, (*(iter + 1))->value, 
                                 type_int, enum_int_type, NULL, false, &temp);
        builder.SetInsertPoint(temp.block);

        llvm::Twine bling;
        llvm::Value *res = llvm::cast<llvm::Value>(
                               builder.CreateLShr(one, temp.value, bling)
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

    makeEnumFunction(ctx, mod, once_tag, "&",  &llvm::IRBuilder<>::CreateAnd,     enum_type,       enum_type, flinkage);
    makeEnumFunction(ctx, mod, once_tag, "|",  &llvm::IRBuilder<>::CreateOr,      enum_type,       enum_type, flinkage);
    makeEnumFunction(ctx, mod, once_tag, "^",  &llvm::IRBuilder<>::CreateXor,     enum_type,       enum_type, flinkage);

    return;
}

}
}
