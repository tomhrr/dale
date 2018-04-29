#include "Value.h"

#include <string>
#include <vector>

#include "../../Error/Error.h"
#include "../../Linkage/Linkage.h"
#include "../../Operation/Cast/Cast.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../../Operation/Cast/Cast.h"
#include "../../Operation/Copy/Copy.h"
#include "../../Operation/Offsetof/Offsetof.h"
#include "../../Operation/Sizeof/Sizeof.h"
#include "../../Units/Units.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../../Decoder/Decoder.h"
#include "../Utils/Utils.h"
#include "../Literal/Literal.h"
#include "../ProcBody/ProcBody.h"
#include "llvm/ExecutionEngine/GenericValue.h"

using namespace dale::ErrorInst;

namespace dale {
bool isRvalue(Node *top) {
    return (top->is_list && (top->list->size() == 2) &&
            ((*top->list)[0]->is_token) &&
            (!(*top->list)[0]->token->str_value.compare("move")));
}

Function *createFunction(Units *units, Type *type, Node *top) {
    Context *ctx = units->top()->ctx;
    llvm::Type *llvm_return_type = ctx->toLLVMType(type, top, false);
    if (!llvm_return_type) {
        return NULL;
    }

    std::vector<llvm::Type *> empty_args;
    llvm::FunctionType *ft =
        getFunctionType(llvm_return_type, empty_args, false);

    std::string new_name;
    units->top()->getUnusedFunctionName(&new_name);

    llvm::Constant *const_fn =
        units->top()->module->getOrInsertFunction(new_name.c_str(), ft);

    llvm::Function *llvm_fn = llvm::cast<llvm::Function>(const_fn);
    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_C));

    std::vector<Variable *> args;
    Function *fn = new Function(type, &args, llvm_fn, 0, &new_name);

    fn->linkage = Linkage::Intern;

    return fn;
}

llvm::Constant *FormValueParse(Units *units, Type *type, Node *top,
                               int *size) {
    Context *ctx = units->top()->ctx;

    ParseResult pr;
    bool res = FormLiteralParse(units, type, top, size, &pr);
    if (res) {
        return llvm::dyn_cast<llvm::Constant>(pr.getValue(ctx));
    }

    bool is_rvalue = isRvalue(top);

    Function *fn = createFunction(units, type, top);
    if (!fn) {
        return NULL;
    }
    llvm::Function *llvm_fn = fn->llvm_function;

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node *> nodes;
    nodes.push_back(top);
    Node *wrapper_top = new Node(&nodes);

    std::vector<NSNode *> active_ns_nodes = ctx->active_ns_nodes;
    std::vector<NSNode *> used_ns_nodes = ctx->used_ns_nodes;
    if (!units->prefunction_ns) {
        units->prefunction_ns = ctx->active_ns_nodes.front()->ns;
    }
    ctx->popUntilNamespace(units->prefunction_ns);

    ctx->enableRetrievalLog();
    units->top()->pushGlobalFunction(fn);
    ctx->activateAnonymousNamespace();
    std::string anon_name = ctx->ns()->name;
    FormProcBodyParse(units, wrapper_top, fn, llvm_fn, 0, 0);
    ctx->deactivateNamespace(anon_name.c_str());
    units->top()->popGlobalFunction();

    ctx->active_ns_nodes = active_ns_nodes;
    ctx->used_ns_nodes = used_ns_nodes;

    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count_begin != error_count_end) {
        llvm_fn->replaceAllUsesWith(
            llvm::UndefValue::get(llvm_fn->getType()));
        llvm_fn->eraseFromParent();
        return NULL;
    }

    std::vector<llvm::Type *> empty_args;
    llvm::FunctionType *wrapper_ft = getFunctionType(
        ctx->toLLVMType(ctx->tr->type_pchar, NULL, false), empty_args,
        false);

    std::string wrapper_new_name;
    units->top()->getUnusedFunctionName(&wrapper_new_name);

    llvm::Constant *wrapper_const_fn =
        units->top()->module->getOrInsertFunction(
            wrapper_new_name.c_str(), wrapper_ft);
    llvm::Function *wrapper_fn =
        llvm::cast<llvm::Function>(wrapper_const_fn);

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*getContext(), "entry", wrapper_fn);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args;
    llvm::Value *ret = builder.CreateCall(
        llvm_fn, llvm::ArrayRef<llvm::Value *>(call_args));

    llvm::Type *llvm_return_type = ctx->toLLVMType(type, top, false);
    llvm::Value *ret_storage1 = builder.CreateAlloca(llvm_return_type);
    llvm::Value *ret_storage2 = builder.CreateAlloca(llvm_return_type);
    builder.CreateStore(ret, ret_storage2);

    std::vector<Type *> call_arg_types;
    Type *ptr_type = ctx->tr->getPointerType(type);
    STL::push_back2(&call_arg_types, ptr_type, ptr_type);

    std::vector<Type *> types;
    types.push_back(ptr_type);
    types.push_back(type);
    std::vector<bool> lvalues;
    lvalues.push_back(true);
    lvalues.push_back(false);
    Function *or_move = ctx->getFunction("setf-move-assign", &types,
                                         NULL, false, &lvalues);
    Function *or_setf =
        ctx->getFunction("setf-copy-assign", &call_arg_types, NULL, 0);

    if (or_move && is_rvalue) {
        or_setf = or_move;
    } else if (!Operation::IsCopyPermitted(ctx, top, type)) {
        return NULL;
    }

    if (or_setf) {
        std::vector<llvm::Value *> or_call_args;
        STL::push_back2(&or_call_args, ret_storage1, ret_storage2);
        builder.CreateCall(or_setf->llvm_function,
                           llvm::ArrayRef<llvm::Value *>(or_call_args));
    } else {
        builder.CreateStore(ret, ret_storage1);
    }

    ParseResult cast_pr;
    res = Operation::Cast(ctx, block, ret_storage1,
                          ctx->tr->getPointerType(type),
                          ctx->tr->type_pchar, top, 0, &cast_pr);
    if (!res) {
        return NULL;
    }
    block = cast_pr.block;
    llvm::Value *ret_cast = cast_pr.getValue(ctx);

    char data[256];
    memset(data, 0, 256);

    char ptr_int[64];
    snprintf(ptr_int, sizeof(ptr_int), "%lld",
             reinterpret_cast<long long int>(&data)); // NOLINT

    llvm::Value *ptr_value = ctx->nt->getConstantInt(
        llvm::IntegerType::get(*getContext(), sizeof(char *) * 8),
        ptr_int);
    ParseResult cast_pr_ptr;
    res = Operation::Cast(ctx, block, ptr_value, ctx->tr->type_intptr,
                          ctx->tr->type_pchar, top, 0, &cast_pr_ptr);
    if (!res) {
        return NULL;
    }
    builder.SetInsertPoint(cast_pr_ptr.block);

    llvm::Value *new_ptr_value = builder.CreateAlloca(
        ctx->toLLVMType(ctx->tr->type_pchar, NULL, false));
    Function *malloc = ctx->getFunction("malloc", NULL, NULL, 0);
    assert(malloc && "no memcpy function available");
    std::vector<llvm::Value *> malloc_args;
    malloc_args.push_back(
        llvm::ConstantInt::get(ctx->nt->getNativeSizeType(), 256));
    builder.CreateStore(
        builder.CreateBitCast(
            builder.CreateCall(
                malloc->llvm_function,
                llvm::ArrayRef<llvm::Value *>(malloc_args)),
            ctx->toLLVMType(ctx->tr->type_pchar, NULL, false)),
        new_ptr_value);

    Function *memcpy_fn = ctx->getFunction("memcpy", NULL, NULL, 0);
    assert(memcpy_fn && "no memcpy function available");

    size_t struct_size = Operation::SizeofGet(units->top(), type);
    char struct_size_str[8];
    snprintf(struct_size_str, sizeof(struct_size_str), "%u",
             static_cast<unsigned>(struct_size));

    std::vector<llvm::Value *> memcpy_args;
    memcpy_args.push_back(builder.CreateBitCast(
        builder.CreateLoad(new_ptr_value),
        ctx->toLLVMType(ctx->tr->type_pvoid, NULL, false)));
    memcpy_args.push_back(builder.CreateBitCast(
        ret_cast, ctx->toLLVMType(ctx->tr->type_pvoid, NULL, false)));
    memcpy_args.push_back(
        ctx->nt->getConstantInt((llvm::IntegerType *)ctx->toLLVMType(
                                    ctx->tr->type_size, NULL, false),
                                struct_size_str));

    builder.CreateCall(memcpy_fn->llvm_function,
                       llvm::ArrayRef<llvm::Value *>(memcpy_args));
    builder.CreateRet(builder.CreateLoad(new_ptr_value));

    if (units->debug) {
        functionDebugPass(llvm_fn);
        functionDebugPass(wrapper_fn);
    }

    cloneModuleIfRequired(units->top());

    llvm::Function *bf =
        units->top()->ee->FindFunctionNamed(wrapper_new_name.c_str());
    std::vector<llvm::GenericValue> values;

#if D_LLVM_VERSION_ORD >= 34
    units->top()->ee->getFunctionAddress(wrapper_new_name);
#endif

    llvm::GenericValue res2 = units->top()->ee->runFunction(bf, values);
    memcpy(data, res2.PointerVal, 256);

    error_count_begin = ctx->er->getErrorTypeCount(ErrorType::Error);

    llvm::Constant *parsed =
        decodeRawData(units, top, reinterpret_cast<char *>(&data),
                      type, size);
    ctx->disableRetrievalLog();

    wrapper_fn->eraseFromParent();
    llvm_fn->eraseFromParent();

    if (parsed) {
        return parsed;
    }

    error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count_begin != error_count_end) {
        return NULL;
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, top, type_str.c_str());
    ctx->er->addError(e);
    return NULL;
}
}
