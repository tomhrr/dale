#include "Value.h"

#include <string>
#include <vector>

#include "../../Decoder/Decoder.h"
#include "../../Error/Error.h"
#include "../../Linkage/Linkage.h"
#include "../../Operation/Cast/Cast.h"
#include "../../Operation/Copy/Copy.h"
#include "../../Operation/Offsetof/Offsetof.h"
#include "../../Operation/Sizeof/Sizeof.h"
#include "../../Units/Units.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../Literal/Literal.h"
#include "../ProcBody/ProcBody.h"
#include "../Utils/Utils.h"
#include "llvm/ExecutionEngine/GenericValue.h"

#define MAX_SIZE 256

using namespace dale::ErrorInst;

namespace dale {
/* Parse the function body (represented by 'top') into the function
 * 'fn'. */
bool parseFunction(Units *units, Type *type, Node *top, Function *fn) {
    Context *ctx = units->top()->ctx;
    llvm::Function *llvm_fn = fn->llvm_function;
    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node *> nodes;
    nodes.push_back(top);
    Node *copy_top = new Node(&nodes);

    units->top()->pushGlobalFunction(fn);
    FormProcBodyParse(units, copy_top, fn, llvm_fn, 0, 0);
    units->top()->popGlobalFunction();

    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count_begin != error_count_end) {
        llvm_fn->replaceAllUsesWith(
            llvm::UndefValue::get(llvm_fn->getType()));
        llvm_fn->eraseFromParent();
        return false;
    }
    return true;
}

/* Create a function that allocates memory, stores the result of
 * calling 'fn' in that memory, and returns a pointer to that memory.
 * Also stores the name of the new function in 'name'. */
llvm::Function *createCopyFunction(Units *units, Type *type, Node *top,
                                   Function *fn, std::string *name) {
    Context *ctx = units->top()->ctx;

    Function *d_copy_fn =
        createFunction(units, ctx->tr->type_pchar, top);
    *name = d_copy_fn->symbol;
    llvm::Function *copy_fn = d_copy_fn->llvm_function;

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*getContext(), "entry", copy_fn);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args;
    llvm::Value *ret = builder.CreateCall(
        fn->llvm_function, llvm::ArrayRef<llvm::Value *>(call_args));

    llvm::Value *ret_storage = builder.CreateAlloca(ret->getType());
    builder.CreateStore(ret, ret_storage);

    ParseResult cast_pr;
    bool res = Operation::Cast(ctx, block, ret_storage,
                               ctx->tr->getPointerType(type),
                               ctx->tr->type_pchar, top, 0, &cast_pr);
    if (!res) {
        return NULL;
    }
    llvm::Value *ret_cast = cast_pr.getValue(ctx);

    Function *malloc = ctx->getFunction("malloc", NULL, NULL, 0);
    assert(malloc && "no malloc function available");

    Function *memcpy_fn = ctx->getFunction("memcpy", NULL, NULL, 0);
    assert(memcpy_fn && "no memcpy function available");

    llvm::Value *new_ptr_value = builder.CreateAlloca(
        ctx->toLLVMType(ctx->tr->type_pchar, NULL, false));
    std::vector<llvm::Value *> malloc_args;
    malloc_args.push_back(
        llvm::ConstantInt::get(ctx->nt->getNativeSizeType(), MAX_SIZE));
    builder.CreateStore(
        builder.CreateBitCast(
            builder.CreateCall(
                malloc->llvm_function,
                llvm::ArrayRef<llvm::Value *>(malloc_args)),
            ctx->toLLVMType(ctx->tr->type_pchar, NULL, false)),
        new_ptr_value);

    llvm::Type *llvm_pvoid_type =
        ctx->toLLVMType(ctx->tr->type_pvoid, NULL, false);
    size_t struct_size = Operation::SizeofGet(units->top(), type);

    std::vector<llvm::Value *> memcpy_args;
    memcpy_args.push_back(builder.CreateBitCast(
        builder.CreateLoad(new_ptr_value), llvm_pvoid_type));
    memcpy_args.push_back(
        builder.CreateBitCast(ret_cast, llvm_pvoid_type));
    memcpy_args.push_back(llvm::ConstantInt::get(
        ctx->nt->getNativeSizeType(), struct_size));

    builder.CreateCall(memcpy_fn->llvm_function,
                       llvm::ArrayRef<llvm::Value *>(memcpy_args));

    builder.CreateRet(builder.CreateLoad(new_ptr_value));
    return copy_fn;
}

llvm::Constant *FormValueParse(Units *units, Type *type, Node *top,
                               int *size) {
    Context *ctx = units->top()->ctx;

    /* Try to parse the value as a literal first. */

    ParseResult pr;
    bool res = FormLiteralParse(units, type, top, &pr);
    if (res) {
        return llvm::dyn_cast<llvm::Constant>(pr.getValue(ctx));
    }

    /* If the value can't be parsed as a literal, then process it as
     * if it were wrapped in an anonymous function.  Convert the
     * resulting raw data into an LLVM constant and return that
     * constant. */

    ctx->enableRetrievalLog();
    llvm::Module *prev_mod = units->top()->module;

    /* Most of the logic here is about creating a new LLVM module for
     * the function used to parse the value, to avoid needing to
     * recompile the entire existing module, and then linking the
     * appropriate bindings etc. into place in that new module. */

    Unit *unit = new Unit(units, ctx->er, ctx->nt, ctx->tr, NULL,
                          true, ctx, units->top()->mp,
                          units->top()->fp, NULL, NULL);
    units->push(unit);

    Function *fn = createFunction(units, type, top);
    fn->cto = true;
    if (!fn) {
        units->units.pop_back();
        ctx->regetPointers(units->top()->module);
        return NULL;
    }
    res = parseFunction(units, type, top, fn);
    if (!res) {
        units->units.pop_back();
        ctx->regetPointers(units->top()->module);
        return NULL;
    }
    std::string name;
    llvm::Function *copy_fn =
        createCopyFunction(units, type, top, fn, &name);

    std::vector<Function *> functions;
    ctx->getRetrievedFunctions(&functions);
    std::vector<Variable *> variables;
    ctx->getRetrievedVariables(&variables);
    linkRetrievedObjects(units->top()->module, top, &functions,
                         &variables);

    copy_fn->eraseFromParent();
    fn->llvm_function->eraseFromParent();

    fn = createFunction(units, type, top);
    fn->cto = true;
    if (!fn) {
        units->units.pop_back();
        ctx->regetPointers(units->top()->module);

        return NULL;
    }
    res = parseFunction(units, type, top, fn);
    if (!res) {
        units->units.pop_back();
        ctx->regetPointers(units->top()->module);

        return NULL;
    }
    name.clear();
    copy_fn =
        createCopyFunction(units, type, top, fn, &name);

    if (units->debug) {
        functionDebugPass(fn->llvm_function);
        functionDebugPass(copy_fn);
    }

    cloneModuleIfRequired(units->top());

    llvm::Function *copy_fn_retrieved =
        units->top()->ee->FindFunctionNamed(name.c_str());

#if D_LLVM_VERSION_ORD >= 34
    uint64_t addr = units->top()->ee->getFunctionAddress(name);
    if (!addr) {
        abort();
    }
#endif

    std::vector<llvm::GenericValue> values;
    llvm::GenericValue fn_result =
        units->top()->ee->runFunction(copy_fn_retrieved, values);

    char data[MAX_SIZE];
    memcpy(data, fn_result.PointerVal, MAX_SIZE);

    int error_count_begin =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    llvm::Constant *parsed =
        decodeRawData(units, top, reinterpret_cast<char *>(&data), type, size,
                      prev_mod);

    ctx->disableRetrievalLog();

    copy_fn->eraseFromParent();
    fn->llvm_function->eraseFromParent();

    llvm::Module *mod = units->top()->module;
    units->units.pop_back();
    ctx->regetPointers(units->top()->module);
    delete mod;

    if (parsed) {
        return parsed;
    }

    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
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
