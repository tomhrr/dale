#include "GlobalVariable.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../Operation/Offsetof/Offsetof.h"
#include "../../Linkage/Linkage.h"
#include "../../ProcBody/ProcBody.h"
#include "../../Type/Type.h"
#include "Config.h"

using namespace dale::ErrorInst;

namespace dale
{
llvm::Constant *
parseLiteralElement(Units *units, Node *top, char *data, Type *type,
                    int *size);

llvm::Constant *
apIntToConstant(llvm::APInt & ap_int)
{
    llvm::ConstantInt *const_int =
        llvm::ConstantInt::get(llvm::getGlobalContext(), ap_int);
    return llvm::cast<llvm::Constant>(const_int);
}

llvm::Constant *
apFloatToConstant(llvm::APFloat & ap_float)
{
    llvm::ConstantFP *const_float =
        llvm::ConstantFP::get(llvm::getGlobalContext(), ap_float);
    return llvm::cast<llvm::Constant>(const_float);
}

llvm::Constant *
parseSmallLiteralInteger(int size, const char *data)
{
    llvm::APInt ap_int(size, *data);
    return apIntToConstant(ap_int);
}

llvm::Constant *
parseLiteralInteger(int size, const char *data)
{
    union uchar_uint64 {
        unsigned char c[8];
        uint64_t      n;
    } num;
    num.n = 0;

    int i;
    if (size == 128) {
        uint64_t ns[2];
        for (i = 0; i < 8; i++) {
            num.c[i] = data[i];
        }
        ns[0] = num.n;
        for (i = 8; i < 16; i++) {
            num.c[i - 8] = data[i];
        }
        ns[1] = num.n;
        llvm::APInt myint((unsigned) size, 2, ns);
        return apIntToConstant(myint);
    } else {
        num.n = 0;
        for (i = 0; i < (size / 8); i++) {
            num.c[i] = data[i];
        }
        llvm::APInt myint(size, num.n);
        return apIntToConstant(myint);
    }
}

llvm::Constant *
parseLiteralFloat(char *data)
{
    union uchar_float {
        unsigned char c[4];
        float         n;
    } num;
    num.n = 0;

    for (int i = 3; i >= 0; i--) {
        num.c[i] = data[i];
    }
    llvm::APFloat ap_float(num.n);
    return apFloatToConstant(ap_float);
}

llvm::Constant *
parseLiteralDouble(char *data)
{
    union uchar_double {
        unsigned char c[8];
        double        n;
    } num;
    num.n = 0;

    for (int i = 7; i >= 0; i--) {
        num.c[i] = data[i];
    }
    llvm::APFloat ap_float(num.n);
    return apFloatToConstant(ap_float);
}

llvm::Constant *
parseLiteralStruct(Units *units, Node *top, char *data, Type *type,
                   int *size)
{
    Context *ctx = units->top()->ctx;

    std::vector<llvm::Constant *> constants;

    Struct *st = ctx->getStruct(type);
    assert(st);

    int last_member_size = -1;
    int last_offset      = -1;
    int index            = 0;
    int total_padding    = 0;

    for (std::vector<Type *>::iterator b = st->member_types.begin(),
                                       e = st->member_types.end();
            b != e;
            ++b) {
        Type *member_type = (*b);

        size_t member_size = Operation::SizeofGet(units->top(), member_type);
        size_t offset = Operation::OffsetofGetByIndex(units->top(),
                                                      type, index);
        size_t padding = 0;
        if (index != 0) {
            padding = (offset - last_offset - last_member_size);
        }
        if (padding) {
            Error *e = new Error(StructContainsPadding, top);
            ctx->er->addError(e);
        }
        total_padding += padding;

        assert((member_size <= 256) && "struct member size too large");
        char *addr = data + offset;
        char aligned[256];
        memcpy(aligned, addr, member_size);

        llvm::Constant *member_value =
            parseLiteralElement(units, top, (char*) aligned, member_type, size);
        if (!member_value) {
            return NULL;
        }
        constants.push_back(member_value);

        last_offset = offset - total_padding;
        last_member_size = member_size;
        ++index;
    }

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return NULL;
    }

    llvm::StructType *llvm_st = llvm::cast<llvm::StructType>(llvm_type);
    llvm::Constant *const_st = llvm::ConstantStruct::get(llvm_st, constants);

    return const_st;
}

llvm::Constant *
parseLiteralString(Units *units, Node *top, char *data, Type *type,
                   int *size)
{
    Context *ctx = units->top()->ctx;
    TypeRegister *tr = ctx->tr;

    /* data contains a char pointer, hence the cast. */
    char *str = *(char**) data;
    *size = strlen(str) + 1;
    llvm::Constant *constr_str = getStringConstantArray(str);

    std::string var_name;
    units->top()->getUnusedVarName(&var_name);

    Type *char_array_type = tr->getArrayType(tr->type_char, *size);
    llvm::Type *llvm_type = ctx->toLLVMType(char_array_type, NULL, false);

    llvm::Module *mod = units->top()->module;
    assert(!mod->getGlobalVariable(llvm::StringRef(var_name.c_str())));

    llvm::GlobalVariable *var =
        llvm::cast<llvm::GlobalVariable>(
            mod->getOrInsertGlobal(var_name.c_str(), llvm_type)
        );

    var->setInitializer(constr_str);
    var->setConstant(true);
    var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::Constant *const_pchar =
        llvm::ConstantExpr::getGetElementPtr(llvm::cast<llvm::Constant>(var),
                                             ctx->nt->getTwoLLVMZeros());

    return const_pchar;
}

llvm::Constant *
parseLiteralPointer(Units *units, Node *top, char *data, Type *type,
                    int *size)
{
    Context *ctx = units->top()->ctx;

    if (*data) {
        uint64_t value = *(uint64_t *) data;
        if (sizeof(char *) == 4) {
            value <<= 32;
        }
        if (value) {
            Error *e = new Error(NonNullPointerInGlobalStructDeclaration, top);
            ctx->er->addError(e);
        }
    }

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return NULL;
    }

    return getNullPointer(llvm_type);
}

llvm::Constant *
parseLiteralArray(Units *units, Node *top, char *data, Type *type,
                  int *size)
{
    Context *ctx = units->top()->ctx;

    size_t member_size = Operation::SizeofGet(units->top(), type->array_type);
    assert((member_size <= 256) && "array member size too large");
    int members = type->array_size;
    std::vector<llvm::Constant *> constants;

    char mem_array[256];
    char *mem = mem_array;

    for (int i = 0; i < members; i++) {
        memset(mem, 0, 256);
        char *member_ptr = ((char*) data) + (i * member_size);
        memcpy(mem, member_ptr, member_size);

        llvm::Constant *const_member =
            parseLiteralElement(units, top, mem, type->array_type, size);

        constants.push_back(const_member);
    }

    llvm::Constant *const_arr =
        llvm::ConstantArray::get(
            llvm::cast<llvm::ArrayType>(
                ctx->toLLVMType(type, top, false, false)
            ),
            constants
        );

    return const_arr;
}

llvm::Constant *
parseLiteralElement(Units *units, Node *top, char *data, Type *type,
                    int *size)
{
    Context *ctx = units->top()->ctx;
    NativeTypes *nt = ctx->nt;

    if (type->base_type == BaseType::Bool) {
        return parseSmallLiteralInteger(1, data);
    }

    if (type->base_type == BaseType::Char) {
        return parseSmallLiteralInteger(8, data);
    }

    if (type->isIntegerType()) {
        int size = nt->internalSizeToRealSize(type->getIntegerSize());
        return parseLiteralInteger(size, data);
    }

    if (type->base_type == BaseType::Float) {
        return parseLiteralFloat(data);
    }

    if (type->base_type == BaseType::Double) {
        return parseLiteralDouble(data);
    }

    if (type->struct_name.size()) {
        return parseLiteralStruct(units, top, data, type, size);
    }

    if (type->points_to && (type->points_to->base_type == BaseType::Char)) {
        return parseLiteralString(units, top, data, type, size);
    }

    if (type->points_to) {
        return parseLiteralPointer(units, top, data, type, size);
    }

    if (type->is_array) {
        return parseLiteralArray(units, top, data, type, size);
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, top, type_str.c_str());
    ctx->er->addError(e);

    return NULL;
}

llvm::Constant *
parseLiteral(Units *units, Type *type, Node *top, int *size)
{
    /* The size argument is only set when parsing a string literal; it
     * will contain the final size of the returned array. */

    Context *ctx = units->top()->ctx;

    /* If the argument node is an address-of form for a global
     * variable name, return the address of that global
     * variable as a constant value.  This is to get around the
     * fact that arbitrary pointer values returned from the
     * function created below will not be valid with respect to
     * global variables. */

    if (top->is_list
            && (top->list->size() == 2)
            && (top->list->at(0)->is_token)
            && (!top->list->at(0)->token->str_value.compare("#"))
            && (type->points_to)) {
        Node *var_node = top->list->at(1);
        var_node = units->top()->mp->parsePotentialMacroCall(var_node);
        if (var_node && var_node->is_token) {
            Variable *gv =
                ctx->getVariable(var_node->token->str_value.c_str());
            if (!(type->points_to->isEqualTo(gv->type))) {
                std::string want;
                std::string got;
                gv->type->toString(&got);
                type->toString(&want);
                Error *e = new Error(IncorrectType, top,
                                     want.c_str(), got.c_str());
                ctx->er->addError(e);
                return NULL;
            }
            llvm::Constant *const_ptr =
                llvm::cast<llvm::Constant>(gv->value);
            return const_ptr;
        }
    }

    std::string str;
    type->toString(&str);

    /* Create an empty no-argument function that returns the specified
     * type. */

    llvm::Type *llvm_return_type = ctx->toLLVMType(type, top, false);
    if (!llvm_return_type) {
        return NULL;
    }

    std::vector<llvm::Type*> empty_args;
    llvm::FunctionType *ft = getFunctionType(llvm_return_type,
                                             empty_args, false);

    std::string new_name;
    units->top()->getUnusedFunctionName(&new_name);

    llvm::Constant *const_fn =
        units->top()->module->getOrInsertFunction(new_name.c_str(), ft);

    llvm::Function *llvm_fn = llvm::cast<llvm::Function>(const_fn);
    llvm_fn->setCallingConv(llvm::CallingConv::C);
    llvm_fn->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_C));

    std::vector<Variable*> args;
    Function *fn = new Function(type, &args, llvm_fn, 0, &new_name);

    fn->linkage = Linkage::Intern;
    int error_count_begin = ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node*> nodes;
    nodes.push_back(top);
    Node *wrapper_top = new Node(&nodes);

    FormProcBodyParse(units, wrapper_top, fn, llvm_fn, 0, 0);
    int error_count_end = ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count_begin != error_count_end) {
        return NULL;
    }

    llvm::Type *llvm_type_void =
        ctx->toLLVMType(ctx->tr->type_void, NULL, true);
    llvm::FunctionType *wrapper_ft =
        getFunctionType(llvm_type_void, empty_args, false);

    std::string wrapper_new_name;
    units->top()->getUnusedFunctionName(&wrapper_new_name);

    llvm::Constant *wrapper_const_fn =
        units->top()->module->getOrInsertFunction(
            wrapper_new_name.c_str(), wrapper_ft
        );
    llvm::Function *wrapper_fn =
        llvm::cast<llvm::Function>(wrapper_const_fn);

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry",
                                 wrapper_fn);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args;
    llvm::Value *ret =
        builder.CreateCall(llvm_fn, llvm::ArrayRef<llvm::Value*>(call_args));

    llvm::Value *ret_storage1 = builder.CreateAlloca(llvm_return_type);
    llvm::Value *ret_storage2 = builder.CreateAlloca(llvm_return_type);
    builder.CreateStore(ret, ret_storage2);

    std::vector<Type *> call_arg_types;
    Type *ptr_type = ctx->tr->getPointerType(type);
    STL::push_back2(&call_arg_types, ptr_type, ptr_type);

    if (Function *or_setf =
            ctx->getFunction("setf-assign", &call_arg_types, NULL, 0)) {
        std::vector<llvm::Value *> or_call_args;
        STL::push_back2(&or_call_args, ret_storage1, ret_storage2);
        builder.CreateCall(
            or_setf->llvm_function,
            llvm::ArrayRef<llvm::Value*>(or_call_args)
        );
    } else {
        builder.CreateStore(ret, ret_storage1);
    }

    ParseResult cast_pr;
    bool res = Operation::Cast(ctx, block, ret_storage1,
                               ctx->tr->getPointerType(type),
                               ctx->tr->type_pvoid, top, 0, &cast_pr);
    if (!res) {
        return NULL;
    }
    block = cast_pr.block;
    llvm::Value *ret_cast = cast_pr.value;

    char data[256];
    memset(data, 0, 256);

    char ptr_int[64];
    sprintf(ptr_int, "%lld", (long long int) &data);

    llvm::Value *ptr_value =
        ctx->nt->getConstantInt(
            llvm::IntegerType::get(llvm::getGlobalContext(),
                                   sizeof(char*) * 8),
            ptr_int
        );
    ParseResult cast_pr_ptr;
    res = Operation::Cast(ctx, block, ptr_value, ctx->tr->type_intptr,
                          ctx->tr->type_pvoid, top, 0, &cast_pr_ptr);
    if (!res) {
        return NULL;
    }

    llvm::Value *store = cast_pr_ptr.value;
    builder.SetInsertPoint(cast_pr_ptr.block);
    Function *memcpy = ctx->getFunction("memcpy", NULL, NULL, 0);
    assert(memcpy && "no memcpy function available");

    size_t struct_size = Operation::SizeofGet(units->top(), type);
    char struct_size_str[8];
    sprintf(struct_size_str, "%u", (unsigned) struct_size);

    std::vector<llvm::Value*> memcpy_args;
    memcpy_args.push_back(store);
    memcpy_args.push_back(ret_cast);
    memcpy_args.push_back(
        ctx->nt->getConstantInt(
            (llvm::IntegerType*) ctx->toLLVMType(ctx->tr->type_size, NULL,
                                                 false),
            struct_size_str
        )
    );

    builder.CreateCall(memcpy->llvm_function,
                       llvm::ArrayRef<llvm::Value*>(memcpy_args));
    builder.CreateRetVoid();

    void* fptr = units->top()->ee->getPointerToFunction(wrapper_fn);
    assert(fptr && "could not get pointer to function for literal");

    ((void (*)()) fptr)();

    llvm::Constant *parsed =
        parseLiteralElement(units, top, (char*) &data, type, size);

    wrapper_fn->eraseFromParent();
    (llvm::cast<llvm::Function>(const_fn))->eraseFromParent();

    if (parsed) {
        return parsed;
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, top, type_str.c_str());
    ctx->er->addError(e);
    return NULL;
}

void
zeroInitialise(Context *ctx, llvm::GlobalVariable *llvm_var,
               llvm::Type *llvm_type, Type *type, bool *has_initialiser)
{
    if (type->points_to) {
        llvm_var->setInitializer(getNullPointer(llvm_type));
    } else if (type->struct_name.size() || type->is_array) {
        llvm_var->setInitializer(
            llvm::ConstantAggregateZero::get(llvm_type)
        );
    } else if (type->isIntegerType()) {
        llvm_var->setInitializer(
            ctx->nt->getConstantInt(
                llvm::IntegerType::get(
                    llvm::getGlobalContext(),
                    ctx->nt->internalSizeToRealSize(type->getIntegerSize())
                ),
                "0"
            )
        );
    } else {
        *has_initialiser = false;
    }
    return;
}

bool
FormTopLevelGlobalVariableParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    std::vector<Node *> *top_lst = node->list;
    Node *name_node = (*top_lst)[1];
    Node *def_node  = (*top_lst)[2];

    const char *name = name_node->token->str_value.c_str();

    std::vector<Node *> *lst = def_node->list;
    Node *linkage_node = (*lst)[1];
    Node *type_node    = (*lst)[2];
    Node *value_node   = (*lst)[3];

    bool has_initialiser;
    if (lst->size() < 3) {
        Error *e = new Error(IncorrectMinimumNumberOfArgs, def_node,
                             "var", "2", (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    } else if (lst->size() == 3) {
        has_initialiser = false;
    } else if (lst->size() == 4) {
        has_initialiser = true;
    } else {
        Error *e = new Error(IncorrectMaximumNumberOfArgs, def_node,
                             "var", "3", (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    }

    int linkage = FormLinkageParse(ctx, linkage_node);

    Type *ret_type = FormTypeParse(units, type_node, false, false);
    if (ret_type == NULL) {
        return false;
    }
    if (ret_type->array_type && (ret_type->array_size == 0)) {
        Error *e = new Error(ZeroLengthGlobalArraysAreUnsupported, def_node);
        ctx->er->addError(e);
        return false;
    }

    if (has_initialiser) {
        value_node = units->top()->mp->parsePotentialMacroCall(value_node);
        if (!value_node) {
            return false;
        }
    }

    int size = 0;
    llvm::Constant *init = NULL;
    if (has_initialiser) {
        init = parseLiteral(units, ret_type, value_node, &size);
        if (!init) {
            return false;
        }
    }

    std::string new_name;
    if (linkage == Linkage::Extern_C) {
        new_name.append(name);
    } else {
        ctx->ns()->nameToSymbol(name, &new_name);
    }

    Variable *existing_var = ctx->getVariable(name);
    if (existing_var
            && existing_var->type->isEqualTo(ret_type)
            && (existing_var->linkage == linkage)
            && !has_initialiser) {
        /* A redeclaration of a global variable is a no-op. */
        return true;
    }

    Variable *var = new Variable();
    var->name.append(name);
    var->type = ret_type;
    var->internal_name.append(new_name);
    var->once_tag = units->top()->once_tag;
    var->linkage = linkage;

    bool res = ctx->ns()->addVariable(name, var);
    if (!res) {
        Error *e = new Error(RedefinitionOfVariable, def_node, name);
        ctx->er->addError(e);
        return false;
    }

    llvm::Type *llvm_ret_type =
        ctx->toLLVMType(ret_type, def_node, false,
                        (Linkage::isExternAll(linkage) && !has_initialiser));
    if (!llvm_ret_type) {
        return false;
    }

    assert(!units->top()->module->getGlobalVariable(
               llvm::StringRef(new_name.c_str())
           ) && "variable already exists in module");

    llvm::GlobalVariable *llvm_var =
        llvm::cast<llvm::GlobalVariable>(
            units->top()->module->getOrInsertGlobal(new_name.c_str(),
                                                    llvm_ret_type)
        );
    llvm_var->setLinkage(ctx->toLLVMLinkage(linkage));

    if (init) {
        llvm_var->setInitializer(init);
    } else if (!Linkage::isExternAll(linkage)) {
        has_initialiser = true;
        zeroInitialise(ctx, llvm_var, llvm_ret_type, ret_type,
                       &has_initialiser);
        var->has_initialiser = has_initialiser;
    }

    var->value = llvm::cast<llvm::Value>(llvm_var);

    return true;
}
}
