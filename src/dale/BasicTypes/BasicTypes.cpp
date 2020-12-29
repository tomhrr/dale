#include "BasicTypes.h"

#include <string>
#include <vector>

#include "../Form/Utils/Utils.h"
#include "../llvmUtils/llvmUtils.h"
#include "Config.h"

#define ADD_INTF(name, fn) \
    makeFunction(ctx, mod, once_tag, name, fn, type, type);
#define ADD_FLTF(name, fn) \
    makeFloatFunction(ctx, mod, once_tag, name, fn, type, type);
#define ADD_CMPF(name, fn) \
    makeFunction(ctx, mod, once_tag, name, fn, type_bool, type);

namespace dale {
namespace BasicTypes {
std::vector<std::string> basic_type_form_strings;
void initBasicTypeFormStrings(void) {
    const char *basic_type_form_strings_cc[317] = {
        "_Z1$2bii",
        "_Z1$2dii",
        "_Z1$2fii",
        "_Z1$2aii",
        "_Z1$25ii",
        "_Z1$26ii",
        "_Z1$7cii",
        "_Z1$5eii",
        "_Z1$3dii",
        "_Z2$21$3dii",
        "_Z1$3cii",
        "_Z2$3c$3dii",
        "_Z1$3eii",
        "_Z2$3e$3dii",
        "_Z2$3c$3cii",
        "_Z2$3e$3eii",
        "_Z1$2di",
        "_Z1$2bcc",
        "_Z1$2dcc",
        "_Z1$2fcc",
        "_Z1$2acc",
        "_Z1$25cc",
        "_Z1$26cc",
        "_Z1$7ccc",
        "_Z1$5ecc",
        "_Z1$3dcc",
        "_Z2$21$3dcc",
        "_Z1$3ccc",
        "_Z2$3c$3dcc",
        "_Z1$3ecc",
        "_Z2$3e$3dcc",
        "_Z2$3c$3cci",
        "_Z2$3e$3eci",
        "_Z1$2dc",
        "_Z1$2byy",
        "_Z1$2dyy",
        "_Z1$2fyy",
        "_Z1$2ayy",
        "_Z1$25yy",
        "_Z1$26yy",
        "_Z1$7cyy",
        "_Z1$5eyy",
        "_Z1$3dyy",
        "_Z2$21$3dyy",
        "_Z1$3cyy",
        "_Z2$3c$3dyy",
        "_Z1$3eyy",
        "_Z2$3e$3dyy",
        "_Z2$3c$3cyi",
        "_Z2$3e$3eyi",
        "_Z1$2dy",
        "_Z1$2bnn",
        "_Z1$2dnn",
        "_Z1$2fnn",
        "_Z1$2ann",
        "_Z1$25nn",
        "_Z1$26nn",
        "_Z1$7cnn",
        "_Z1$5enn",
        "_Z1$3dnn",
        "_Z2$21$3dnn",
        "_Z1$3cnn",
        "_Z2$3c$3dnn",
        "_Z1$3enn",
        "_Z2$3e$3dnn",
        "_Z2$3c$3cni",
        "_Z2$3e$3eni",
        "_Z1$2dn",
        "_Z1$2bpp",
        "_Z1$2dpp",
        "_Z1$2fpp",
        "_Z1$2app",
        "_Z1$25pp",
        "_Z1$26pp",
        "_Z1$7cpp",
        "_Z1$5epp",
        "_Z1$3dpp",
        "_Z2$21$3dpp",
        "_Z1$3cpp",
        "_Z2$3c$3dpp",
        "_Z1$3epp",
        "_Z2$3e$3dpp",
        "_Z2$3c$3cpi",
        "_Z2$3e$3epi",
        "_Z1$2dp",
        "_Z1$2brr",
        "_Z1$2drr",
        "_Z1$2frr",
        "_Z1$2arr",
        "_Z1$25rr",
        "_Z1$26rr",
        "_Z1$7crr",
        "_Z1$5err",
        "_Z1$3drr",
        "_Z2$21$3drr",
        "_Z1$3crr",
        "_Z2$3c$3drr",
        "_Z1$3err",
        "_Z2$3e$3drr",
        "_Z2$3c$3cri",
        "_Z2$3e$3eri",
        "_Z1$2dr",
        "_Z1$2bjj",
        "_Z1$2djj",
        "_Z1$2fjj",
        "_Z1$2ajj",
        "_Z1$25jj",
        "_Z1$26jj",
        "_Z1$7cjj",
        "_Z1$5ejj",
        "_Z1$3djj",
        "_Z2$21$3djj",
        "_Z1$3cjj",
        "_Z2$3c$3djj",
        "_Z1$3ejj",
        "_Z2$3e$3djj",
        "_Z2$3c$3cji",
        "_Z2$3e$3eji",
        "_Z1$7ej",
        "_Z1$2dj",
        "_Z1$2bll",
        "_Z1$2dll",
        "_Z1$2fll",
        "_Z1$2all",
        "_Z1$25ll",
        "_Z1$26ll",
        "_Z1$7cll",
        "_Z1$5ell",
        "_Z1$3dll",
        "_Z2$21$3dll",
        "_Z1$3cll",
        "_Z2$3c$3dll",
        "_Z1$3ell",
        "_Z2$3e$3dll",
        "_Z2$3c$3cli",
        "_Z2$3e$3eli",
        "_Z1$7el",
        "_Z1$2dl",
        "_Z1$2bxx",
        "_Z1$2dxx",
        "_Z1$2fxx",
        "_Z1$2axx",
        "_Z1$25xx",
        "_Z1$26xx",
        "_Z1$7cxx",
        "_Z1$5exx",
        "_Z1$3dxx",
        "_Z2$21$3dxx",
        "_Z1$3cxx",
        "_Z2$3c$3dxx",
        "_Z1$3exx",
        "_Z2$3e$3dxx",
        "_Z2$3c$3cxi",
        "_Z2$3e$3exi",
        "_Z1$7ex",
        "_Z1$2dx",
        "_Z1$2bww",
        "_Z1$2dww",
        "_Z1$2fww",
        "_Z1$2aww",
        "_Z1$25ww",
        "_Z1$26ww",
        "_Z1$7cww",
        "_Z1$5eww",
        "_Z1$3dww",
        "_Z2$21$3dww",
        "_Z1$3cww",
        "_Z2$3c$3dww",
        "_Z1$3eww",
        "_Z2$3e$3dww",
        "_Z2$3c$3cwi",
        "_Z2$3e$3ewi",
        "_Z1$7ew",
        "_Z1$2dw",
        "_Z1$2bmm",
        "_Z1$2dmm",
        "_Z1$2fmm",
        "_Z1$2amm",
        "_Z1$25mm",
        "_Z1$26mm",
        "_Z1$7cmm",
        "_Z1$5emm",
        "_Z1$3dmm",
        "_Z2$21$3dmm",
        "_Z1$3cmm",
        "_Z2$3c$3dmm",
        "_Z1$3emm",
        "_Z2$3e$3dmm",
        "_Z2$3c$3cmi",
        "_Z2$3e$3emi",
        "_Z1$7em",
        "_Z1$2dm",
        "_Z1$2boo",
        "_Z1$2doo",
        "_Z1$2foo",
        "_Z1$2aoo",
        "_Z1$25oo",
        "_Z1$26oo",
        "_Z1$7coo",
        "_Z1$5eoo",
        "_Z1$3doo",
        "_Z2$21$3doo",
        "_Z1$3coo",
        "_Z2$3c$3doo",
        "_Z1$3eoo",
        "_Z2$3e$3doo",
        "_Z2$3c$3coi",
        "_Z2$3e$3eoi",
        "_Z1$7eo",
        "_Z1$2do",
        "_Z1$2bqq",
        "_Z1$2dqq",
        "_Z1$2fqq",
        "_Z1$2aqq",
        "_Z1$25qq",
        "_Z1$26qq",
        "_Z1$7cqq",
        "_Z1$5eqq",
        "_Z1$3dqq",
        "_Z2$21$3dqq",
        "_Z1$3cqq",
        "_Z2$3c$3dqq",
        "_Z1$3eqq",
        "_Z2$3e$3dqq",
        "_Z2$3c$3cqi",
        "_Z2$3e$3eqi",
        "_Z1$7eq",
        "_Z1$2dq",
        "_Z1$2bss",
        "_Z1$2dss",
        "_Z1$2fss",
        "_Z1$2ass",
        "_Z1$25ss",
        "_Z1$26ss",
        "_Z1$7css",
        "_Z1$5ess",
        "_Z1$3dss",
        "_Z2$21$3dss",
        "_Z1$3css",
        "_Z2$3c$3dss",
        "_Z1$3ess",
        "_Z2$3e$3dss",
        "_Z2$3c$3csi",
        "_Z2$3e$3esi",
        "_Z1$7es",
        "_Z1$2ds",
        "_Z1$2btt",
        "_Z1$2dtt",
        "_Z1$2ftt",
        "_Z1$2att",
        "_Z1$25tt",
        "_Z1$26tt",
        "_Z1$7ctt",
        "_Z1$5ett",
        "_Z1$3dtt",
        "_Z2$21$3dtt",
        "_Z1$3ctt",
        "_Z2$3c$3dtt",
        "_Z1$3ett",
        "_Z2$3e$3dtt",
        "_Z2$3c$3cti",
        "_Z2$3e$3eti",
        "_Z1$2dt",
        "_Z1$2buu",
        "_Z1$2duu",
        "_Z1$2fuu",
        "_Z1$2auu",
        "_Z1$25uu",
        "_Z1$26uu",
        "_Z1$7cuu",
        "_Z1$5euu",
        "_Z1$3duu",
        "_Z2$21$3duu",
        "_Z1$3cuu",
        "_Z2$3c$3duu",
        "_Z1$3euu",
        "_Z2$3e$3duu",
        "_Z2$3c$3cui",
        "_Z2$3e$3eui",
        "_Z1$7eu",
        "_Z1$2du",
        "_Z1$2bff",
        "_Z1$2dff",
        "_Z1$2fff",
        "_Z1$2aff",
        "_Z1$25ff",
        "_Z1$3dff",
        "_Z2$21$3dff",
        "_Z1$3cff",
        "_Z2$3c$3dff",
        "_Z1$3eff",
        "_Z2$3e$3dff",
        "_Z1$2df",
        "_Z1$2bdd",
        "_Z1$2ddd",
        "_Z1$2fdd",
        "_Z1$2add",
        "_Z1$25dd",
        "_Z1$3ddd",
        "_Z2$21$3ddd",
        "_Z1$3cdd",
        "_Z2$3c$3ddd",
        "_Z1$3edd",
        "_Z2$3e$3ddd",
        "_Z1$2dd",
        "_Z1$2baa",
        "_Z1$2daa",
        "_Z1$2faa",
        "_Z1$2aaa",
        "_Z1$25aa",
        "_Z1$3daa",
        "_Z2$21$3daa",
        "_Z1$3caa",
        "_Z2$3c$3daa",
        "_Z1$3eaa",
        "_Z2$3e$3daa",
        "_Z1$2da",
    };
    for (int i = 0; i < 317; i++) {
        basic_type_form_strings.push_back(basic_type_form_strings_cc[i]);
    }
}

Function *addSimpleFunction(Context *ctx, llvm::Module *mod,
                            std::string *once_tag, const char *name,
                            int linkage, std::string *symbol_name,
                            Type *return_type,
                            std::vector<Variable *> *params) {
    std::vector<llvm::Type *> llvm_param_types;
    for (std::vector<Variable *>::iterator b = params->begin(),
                                           e = params->end();
         b != e; ++b) {
        llvm::Type *arg_type = ctx->toLLVMType((*b)->type, NULL, false);
        assert(arg_type && "unable to resolve argument type");
        llvm_param_types.push_back(arg_type);
    }

    llvm::Type *llvm_ret_type =
        ctx->toLLVMType(return_type, NULL, false);
    assert(llvm_ret_type && "unable to resolve return type");

    llvm::ArrayRef<llvm::Type *> llvm_array_param_types(
        llvm_param_types);
    llvm::FunctionType *ft = llvm::FunctionType::get(
        llvm_ret_type, llvm_array_param_types, false);

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name, &new_name, linkage, params);

    llvm::Function *llvm_fn = llvm::Function::Create(
        ft, llvm::GlobalValue::WeakODRLinkage, new_name.c_str(), mod);

    llvm_fn->setCallingConv(llvm::CallingConv::C);
    setStandardAttributes(llvm_fn);

    Function *fn =
        new Function(return_type, params, llvm_fn, 0, &new_name);
    fn->linkage = linkage;
    fn->once_tag = *once_tag;

    ctx->ns()->addFunction(name, fn, NULL);
    linkVariablesToFunction(params, llvm_fn);

    return fn;
}

Function *addSimpleUnaryFunction(Context *ctx, llvm::Module *mod,
                                 std::string *once_tag,
                                 const char *name, Type *return_type,
                                 Type *type1) {
    int linkage = Linkage::Extern;

    type1 = ctx->tr->getConstType(type1);

    std::vector<Variable *> new_args_ctx;
    new_args_ctx.push_back(new Variable("a", type1));

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name, &new_name, linkage,
                                    &new_args_ctx);

    return addSimpleFunction(ctx, mod, once_tag, name, linkage,
                             &new_name, return_type, &new_args_ctx);
}

Function *addSimpleBinaryFunction(Context *ctx, llvm::Module *mod,
                                  std::string *once_tag,
                                  const char *name, Type *return_type,
                                  Type *type1, Type *type2) {
    int linkage = Linkage::Extern;

    type1 = ctx->tr->getConstType(type1);
    type2 = ctx->tr->getConstType(type2);

    std::vector<Variable *> new_args_ctx;
    new_args_ctx.push_back(new Variable("a", type1));
    new_args_ctx.push_back(new Variable("b", type2));

    std::string new_name;
    ctx->ns()->functionNameToSymbol(name, &new_name, linkage,
                                    &new_args_ctx);

    return addSimpleFunction(ctx, mod, once_tag, name, linkage,
                             &new_name, return_type, &new_args_ctx);
}

void makeFloatFunction(Context *ctx, llvm::Module *mod,
                       std::string *once_tag, const char *name,
                       llvm::Value *(llvm::IRBuilder<>::*method_name)(
                           llvm::Value *, llvm::Value *,
                           const llvm::Twine &
#if D_LLVM_VERSION_ORD >= 32
                           ,
                           llvm::MDNode *
#endif
                           ),  // NOLINT
                       Type *ret_type, Type *type) {
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(((builder).*(method_name))(
            first->value, second->value, unused_twine
#if D_LLVM_VERSION_ORD >= 32
            ,
            NULL
#endif
            ));  // NOLINT

    builder.CreateRet(ret_val);
}

void makeFunction(Context *ctx, llvm::Module *mod,
                  std::string *once_tag, const char *name,
                  llvm::Value *(llvm::IRBuilder<>::*method_name)(
                      llvm::Value *, llvm::Value *,
                      const llvm::Twine &),
                  Type *ret_type, Type *type) {
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(((builder).*(method_name))(
            first->value, second->value, unused_twine));
    if (ret_type->base_type == BaseType::Bool) {
        ret_val = builder.CreateZExt(
            ret_val, ctx->toLLVMType(ret_type, NULL, false));
    }
    builder.CreateRet(ret_val);
}

void makeFunction(Context *ctx, llvm::Module *mod,
                  std::string *once_tag, const char *name,
                  llvm::Value *(llvm::IRBuilder<>::*method_name)(
                      llvm::Value *, llvm::Value *, const llvm::Twine &,
                      bool),
                  Type *ret_type, Type *type) {
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(((builder).*(method_name))(
            first->value, second->value, unused_twine, false));
    if (ret_type->base_type == BaseType::Bool) {
        ret_val = builder.CreateZExt(
            ret_val, ctx->toLLVMType(ret_type, NULL, false));
    }
    builder.CreateRet(ret_val);
}

void makeFunction(Context *ctx, llvm::Module *mod,
                  std::string *once_tag, const char *name,
                  llvm::Value *(llvm::IRBuilder<>::*method_name)(
                      llvm::Value *, llvm::Value *, const llvm::Twine &,
                      bool, bool),
                  Type *ret_type, Type *type) {
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(((builder).*(method_name))(
            first->value, second->value, unused_twine, false, true));
    if (ret_type->base_type == BaseType::Bool) {
        ret_val = builder.CreateZExt(
            ret_val, ctx->toLLVMType(ret_type, NULL, false));
    }
    builder.CreateRet(ret_val);
}

void makeFunction(Context *ctx, llvm::Module *mod,
                  std::string *once_tag, const char *name,
                  llvm::Value *(llvm::IRBuilder<>::*method_name)(
                      llvm::Value *, llvm::Value *, const llvm::Twine &,
                      llvm::MDNode *),
                  Type *ret_type, Type *type) {
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(((builder).*(method_name))(
            first->value, second->value, unused_twine, NULL));
    if (ret_type->base_type == BaseType::Bool) {
        ret_val = builder.CreateZExt(
            ret_val, ctx->toLLVMType(ret_type, NULL, false));
    }
    builder.CreateRet(ret_val);
}

void makeFunction(Context *ctx, llvm::Module *mod,
                  std::string *once_tag, const char *name,
                  llvm::Value *(llvm::IRBuilder<>::*method_name)(
                      llvm::Value *, llvm::Value *, const llvm::Twine &,
                      bool, bool),
                  Type *ret_type, Type *type1, Type *type2) {
    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, name,
                                           ret_type, type1, type2);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    llvm::IRBuilder<> builder(block);
    llvm::Twine unused_twine;
    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(((builder).*(method_name))(
            first->value, second->value, unused_twine, false, true));
    if (ret_type->base_type == BaseType::Bool) {
        ret_val = builder.CreateZExt(
            ret_val, ctx->toLLVMType(ret_type, NULL, false));
    }
    builder.CreateRet(ret_val);
}

void makeShlFunction(Context *ctx, llvm::Module *mod,
                     std::string *once_tag, Type *type) {
    Type *type_int = ctx->tr->type_int;

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, "<<",
                                           type, type, type_int);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    ParseResult cast_pr;
    Operation::Cast(ctx, block, second->value, type_int, type, NULL,
                    false, &cast_pr);

    llvm::IRBuilder<> builder(cast_pr.block);
    llvm::Twine unused_twine;
    llvm::Value *res = llvm::cast<llvm::Value>(builder.CreateShl(
        first->value, cast_pr.getValue(ctx), unused_twine));
    builder.CreateRet(res);
}

void makeShrFunction(Context *ctx, llvm::Module *mod,
                     std::string *once_tag, Type *type) {
    Type *type_int = ctx->tr->type_int;

    Function *fn = addSimpleBinaryFunction(ctx, mod, once_tag, ">>",
                                           type, type, type_int);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();
    Variable *first = *iter;
    Variable *second = *(++iter);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);

    ParseResult cast_pr;
    Operation::Cast(ctx, block, second->value, type_int, type, NULL,
                    false, &cast_pr);

    llvm::IRBuilder<> builder(cast_pr.block);
    llvm::Twine unused_twine;
    llvm::Value *res = llvm::cast<llvm::Value>(builder.CreateLShr(
        first->value, cast_pr.getValue(ctx), unused_twine));
    builder.CreateRet(res);
}

void makeUnaryMinus(Context *ctx, llvm::Module *mod,
                    std::string *once_tag, Type *type) {
    Function *fn =
        addSimpleUnaryFunction(ctx, mod, once_tag, "-", type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return;
    }

    llvm::Value *ret_val = llvm::cast<llvm::Value>(
        (type->isIntegerType())
            ? builder.CreateSub(llvm::ConstantInt::get(llvm_type, 0),
                                (*iter)->value)
            : builder.CreateFSub(llvm::ConstantFP::get(llvm_type, 0),
                                 (*iter)->value));
    builder.CreateRet(ret_val);
}

void addSignedInt(Context *ctx, llvm::Module *mod,
                  std::string *once_tag, Type *type) {
    Type *type_bool = ctx->tr->type_bool;

    ADD_INTF("+", &llvm::IRBuilder<>::CreateAdd);
    ADD_INTF("-", &llvm::IRBuilder<>::CreateSub);
    ADD_INTF("/", &llvm::IRBuilder<>::CreateSDiv);
    ADD_INTF("*", &llvm::IRBuilder<>::CreateMul);
    ADD_INTF("%", &llvm::IRBuilder<>::CreateSRem);

    ADD_INTF("&", &llvm::IRBuilder<>::CreateAnd);
    ADD_INTF("|", &llvm::IRBuilder<>::CreateOr);
    ADD_INTF("^", &llvm::IRBuilder<>::CreateXor);

    ADD_CMPF("=", &llvm::IRBuilder<>::CreateICmpEQ);
    ADD_CMPF("!=", &llvm::IRBuilder<>::CreateICmpNE);
    ADD_CMPF("<", &llvm::IRBuilder<>::CreateICmpSLT);
    ADD_CMPF("<=", &llvm::IRBuilder<>::CreateICmpSLE);
    ADD_CMPF(">", &llvm::IRBuilder<>::CreateICmpSGT);
    ADD_CMPF(">=", &llvm::IRBuilder<>::CreateICmpSGE);

    makeShlFunction(ctx, mod, once_tag, type);
    makeShrFunction(ctx, mod, once_tag, type);
    makeUnaryMinus(ctx, mod, once_tag, type);
}

void makeNegateFunction(Context *ctx, llvm::Module *mod,
                        std::string *once_tag, Type *type) {
    Function *fn =
        addSimpleUnaryFunction(ctx, mod, once_tag, "~", type, type);

    std::vector<Variable *>::iterator iter;
    iter = fn->parameters.begin();

    llvm::BasicBlock *block = llvm::BasicBlock::Create(
        *getContext(), "entry", fn->llvm_function);
    llvm::IRBuilder<> builder(block);

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return;
    }

    llvm::Value *zero = llvm::ConstantInt::get(llvm_type, 0);
    llvm::Value *max =
        builder.CreateSub(zero, llvm::ConstantInt::get(llvm_type, 1));

    llvm::Value *ret_val =
        llvm::cast<llvm::Value>(builder.CreateXor((*iter)->value, max));
    builder.CreateRet(ret_val);
}

void addUnsignedInt(Context *ctx, llvm::Module *mod,
                    std::string *once_tag, Type *type) {
    Type *type_bool = ctx->tr->type_bool;

    ADD_INTF("+", &llvm::IRBuilder<>::CreateAdd);
    ADD_INTF("-", &llvm::IRBuilder<>::CreateSub);
    ADD_INTF("/", &llvm::IRBuilder<>::CreateUDiv);
    ADD_INTF("*", &llvm::IRBuilder<>::CreateMul);
    ADD_INTF("%", &llvm::IRBuilder<>::CreateURem);

    ADD_INTF("&", &llvm::IRBuilder<>::CreateAnd);
    ADD_INTF("|", &llvm::IRBuilder<>::CreateOr);
    ADD_INTF("^", &llvm::IRBuilder<>::CreateXor);

    ADD_CMPF("=", &llvm::IRBuilder<>::CreateICmpEQ);
    ADD_CMPF("!=", &llvm::IRBuilder<>::CreateICmpNE);
    ADD_CMPF("<", &llvm::IRBuilder<>::CreateICmpULT);
    ADD_CMPF("<=", &llvm::IRBuilder<>::CreateICmpULE);
    ADD_CMPF(">", &llvm::IRBuilder<>::CreateICmpUGT);
    ADD_CMPF(">=", &llvm::IRBuilder<>::CreateICmpUGE);

    makeShlFunction(ctx, mod, once_tag, type);
    makeShrFunction(ctx, mod, once_tag, type);
    makeNegateFunction(ctx, mod, once_tag, type);
    makeUnaryMinus(ctx, mod, once_tag, type);
}

void addFloatingPoint(Context *ctx, llvm::Module *mod,
                      std::string *once_tag, Type *type) {
    Type *type_bool = ctx->tr->type_bool;

    ADD_FLTF("+", &llvm::IRBuilder<>::CreateFAdd);
    ADD_FLTF("-", &llvm::IRBuilder<>::CreateFSub);
    ADD_FLTF("/", &llvm::IRBuilder<>::CreateFDiv);
    ADD_FLTF("*", &llvm::IRBuilder<>::CreateFMul);
    ADD_FLTF("%", &llvm::IRBuilder<>::CreateFRem);

    ADD_CMPF("=", &llvm::IRBuilder<>::CreateFCmpOEQ);
    ADD_CMPF("!=", &llvm::IRBuilder<>::CreateFCmpONE);
    ADD_CMPF("<", &llvm::IRBuilder<>::CreateFCmpOLT);
    ADD_CMPF("<=", &llvm::IRBuilder<>::CreateFCmpOLE);
    ADD_CMPF(">", &llvm::IRBuilder<>::CreateFCmpOGT);
    ADD_CMPF(">=", &llvm::IRBuilder<>::CreateFCmpOGE);

    makeUnaryMinus(ctx, mod, once_tag, type);
}
}
}
