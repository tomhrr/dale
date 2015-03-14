#include "GlobalVariable.h"
#include "../../../Units/Units.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../../Operation/Offsetof/Offsetof.h"
#include "../../Linkage/Linkage.h"
#include "../../ProcBody/ProcBody.h"
#include "../../Type/Type.h"
#include "Config.h"

using namespace dale::ErrorInst::Generator;

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
    NativeTypes *nt = ctx->nt;
    TypeRegister *tr = ctx->tr;

    /* data contains a char pointer, hence the cast. */
    char *str = *(char**) data;
    *size = strlen(str) + 1;
    llvm::Constant *constr_str = getStringConstantArray(str);

    std::string var_name;
    units->top()->getUnusedVarname(&var_name);

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

    llvm::Value *two_zero_indices[2];
    two_zero_indices[0] = nt->getLLVMZero();
    two_zero_indices[1] = nt->getLLVMZero();

    llvm::Constant *const_pchar =
        llvm::ConstantExpr::getGetElementPtr(llvm::cast<llvm::Constant>(var),
                                             two_zero_indices, 2);

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

    llvm::Constant *const_ptr =
        llvm::ConstantPointerNull::get(
            llvm::cast<llvm::PointerType>(llvm_type)
        );

    return const_ptr;
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

        /* Offset data by the index, cast to a char pointer, and copy
         * x elements into the new block. */
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

static int myn = 0;
llvm::Constant *
parseLiteral(Units *units, Type *type, Node *top, int *size)
{
    /* size is only set when you parsing a string literal; it will
     * contain the final size of the returned array. */

    Context *ctx = units->top()->ctx;

    /* Extreme special-case - if top is a two-element list, and
     * the first element is #, and the second element is a global
     * variable name, then return the address of that global
     * variable as a constant value. This is to get around the
     * fact that arbitrary pointer values returned from the
     * function created below will not be valid with respect to
     * global variables. (todo - not as useful as you thought it
     * was.) */

    if (top->is_list
            && (top->list->size() == 2)
            && (top->list->at(0)->is_token)
            && (!top->list->at(0)->token->str_value.compare("#"))
            && (type->points_to)) {
        Node *var = top->list->at(1);
        var = units->top()->mp->parsePotentialMacroCall(var);
        if (var && var->is_token) {
            Variable *gv =
                ctx->getVariable(var->token->str_value.c_str());
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
            llvm::Constant *pce =
                llvm::cast<llvm::Constant>(gv->value);
            return pce;
        }
    }

    std::string str;
    type->toString(&str);

    // Create an empty no-argument function that returns the
    // specified type.

    llvm::Type *llvm_return_type =
        ctx->toLLVMType(type, top, false);
    if (!llvm_return_type) {
        return NULL;
    }

    std::vector<llvm::Type*> mc_args;

    llvm::FunctionType *ft =
        getFunctionType(
            llvm_return_type,
            mc_args,
            false
        );

    std::string new_name;
    char buf[32];
    sprintf(buf, "_gv%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (units->top()->module->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        units->top()->module->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    std::vector<Variable*> args;

    llvm::Function *fn = llvm::cast<llvm::Function>(fnc);

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(Linkage::Extern_C));

    Function *dfn =
        new Function(type, &args, fn, 0,
                              &new_name);
    dfn->linkage = Linkage::Intern;
    int error_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node*> nodes;
    nodes.push_back(top);
    Node *topwrapper = new Node(&nodes);

    FormProcBodyParse(units, topwrapper, dfn, fn, 0, 0);
    int error_post_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count != error_post_count) {
        return NULL;
    }

    llvm::Type *tttt = ctx->toLLVMType(
        ctx->tr->type_void, NULL, true);
    llvm::FunctionType *wrapft =
        getFunctionType(
            tttt,
            mc_args,
            false
        );

    std::string wrap_new_name;
    char wrap_buf[32];
    sprintf(wrap_buf, "_gv%d", myn++);
    ctx->ns()->nameToSymbol(wrap_buf, &wrap_new_name);

    if (units->top()->module->getFunction(llvm::StringRef(wrap_new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                wrap_new_name.c_str());
        abort();
    }

    llvm::Constant *wrap_fnc =
        units->top()->module->getOrInsertFunction(
            wrap_new_name.c_str(),
            wrapft
        );

    llvm::Function *wrap_fn =
        llvm::cast<llvm::Function>(wrap_fnc);

    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                 "entry", wrap_fn);
    llvm::IRBuilder<> builder(block);

    std::vector<llvm::Value *> call_args;
    llvm::Value *ret = builder.CreateCall(
                           fn, llvm::ArrayRef<llvm::Value*>(call_args)
                       );

    llvm::Value *reta = builder.CreateAlloca(
                            llvm_return_type
                        );
    llvm::Value *reta2 = builder.CreateAlloca(
                             llvm_return_type
                         );
    builder.CreateStore(ret, reta2);

    std::vector<Type *> call_arg_types;
    Type *ptype = ctx->tr->getPointerType(type);
    STL::push_back2(&call_arg_types, ptype, ptype);

    std::vector<llvm::Value *> call_args2;
    STL::push_back2(&call_args2, reta, reta2);

    if (Function *or_setf =
                ctx->getFunction("setf-assign", &call_arg_types, NULL, 0)) {
        builder.CreateCall(
            or_setf->llvm_function,
            llvm::ArrayRef<llvm::Value*>(call_args2)
        );
    } else {
        builder.CreateStore(ret, reta);
    }

    ParseResult temp_pr;
    bool res =
        Operation::Cast(ctx,
               block,
               reta,
               ctx->tr->getPointerType(type),
               ctx->tr->type_pvoid,
               top, 0, &temp_pr);
    if (!res) {
        return NULL;
    }
    block = temp_pr.block;
    llvm::Value *retaa = temp_pr.value;

    typedef struct temp_t {
        char c[256];
    } temp;
    temp thing;
    memset(&thing, 0, 256);

    char buf6[100];
    sprintf(buf6, "%lld", (long long int) &thing);

    llvm::Value *v =
        ctx->nt->getConstantInt(
            llvm::IntegerType::get(
                llvm::getGlobalContext(),
                sizeof(char*) * 8
            ),
            buf6
        );
    ParseResult storeor;
    res =
        Operation::Cast(ctx, 
               block,
               v,
               ctx->tr->type_intptr,
               ctx->tr->type_pvoid,
               top, 0, &storeor
              );
    if (!res) {
        return NULL;
    }
    llvm::Value *store = storeor.value;
    builder.SetInsertPoint(storeor.block);
    Function *memcpy = ctx->getFunction("memcpy", NULL,
                                NULL, 0);
    if (!memcpy) {
        fprintf(stderr,
                "Internal error: no memcpy function available.\n");
        abort();
    }

    size_t struct_size =
        Operation::SizeofGet(units->top(), type);
    char buf5[5];
    sprintf(buf5, "%u", (unsigned) struct_size);

    std::vector<llvm::Value*> memcpy_args;
    memcpy_args.push_back(store);
    memcpy_args.push_back(retaa);
    memcpy_args.push_back(
        ctx->nt->getConstantInt(
            (llvm::IntegerType*) ctx->toLLVMType(ctx->tr->type_size, NULL, false),
            buf5
        )
    );

    builder.CreateCall(memcpy->llvm_function,
                       llvm::ArrayRef<llvm::Value*>(memcpy_args)
                      );

    /* Take this value, put it in the struct pointer. */

    builder.CreateRetVoid();

    void* fptr =
        units->top()->ee->getPointerToFunction(wrap_fn);
    if (!fptr) {
        fprintf(stderr,
                "Internal error: could not get pointer "
                "to function for literal.\n");
        abort();
    }

    ((void (*)(void)) fptr)();

    llvm::Constant *parsed =
        parseLiteralElement(units, top, (char*) &thing, type, size);

    wrap_fn->eraseFromParent();
    (llvm::cast<llvm::Function>(fnc))->eraseFromParent();

    if (parsed) {
        return parsed;
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, top, type_str.c_str());
    ctx->er->addError(e);
    return NULL;
}

bool
FormTopLevelGlobalVariableParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    Node *name_node = (*(node->list))[1];
    const char *name = name_node->token->str_value.c_str();
    Node *top = (*(node->list))[2];

    std::vector<Node *> *lst = top->list;
    int has_initialiser;

    if (lst->size() < 3) {
        Error *e = new Error(IncorrectMinimumNumberOfArgs, top,
                             "var", "2", (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    } else if (lst->size() == 3) {
        has_initialiser = 0;
    } else if (lst->size() == 4) {
        has_initialiser = 1;
    } else {
        Error *e = new Error(IncorrectMaximumNumberOfArgs, top,
                             "var", "3", (lst->size() - 1));
        ctx->er->addError(e);
        return false;
    }

    int linkage = FormLinkageParse(ctx, (*lst)[1]);

    Type *r_type = FormTypeParse(units, (*lst)[2], false, false);
    if (r_type == NULL) {
        return false;
    }
    if (r_type->array_type && (r_type->array_size == 0)) {
        Error *e = new Error(ZeroLengthGlobalArraysAreUnsupported, top);
        ctx->er->addError(e);
        return false;
    }

    int size = 0;

    Node *n2 = NULL;
    if (has_initialiser) {
        n2 = units->top()->mp->parsePotentialMacroCall((*lst)[3]);
        if (!n2) {
            return false;
        }
    }

    llvm::Constant *init =
        (has_initialiser)
        ? parseLiteral(units, r_type, n2, &size)
        : NULL;

    if ((init == NULL) && (has_initialiser)) {
        return true;
    }

    std::string new_name;
    if (linkage == Linkage::Extern_C) {
        new_name.append(name);
    } else {
        ctx->ns()->nameToSymbol(name, &new_name);
    }

    Variable *check = ctx->getVariable(name);
    if (check
            && check->type->isEqualTo(r_type)
            && (check->linkage == linkage)
            && !has_initialiser) {
        /* Redeclaration of global variable - no problem. */
        return true;
    }

    /* Add the variable to the context. */

    Variable *var2 = new Variable();
    var2->name.append(name);
    var2->type = r_type;
    var2->internal_name.append(new_name);
    var2->once_tag = units->top()->once_tag;
    var2->linkage = linkage;
    int avres = ctx->ns()->addVariable(name, var2);

    if (!avres) {
        Error *e = new Error(RedefinitionOfVariable, top, name);
        ctx->er->addError(e);
        return false;
    }

    /* todo: an 'is_extern_linkage' function. */
    int has_extern_linkage =
        ((linkage != Linkage::Auto)
         && (linkage != Linkage::Intern));

    llvm::Type *rdttype =
        ctx->toLLVMType(r_type, top, false,
                       (has_extern_linkage && !has_initialiser));
    if (!rdttype) {
        return false;
    }

    /* Add the variable to the module. */

    if (units->top()->module->getGlobalVariable(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "global variable already exists in "
                "module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::GlobalVariable *var =
        llvm::cast<llvm::GlobalVariable>(
            units->top()->module->getOrInsertGlobal(new_name.c_str(),
                                   rdttype)
        );

    var->setLinkage(ctx->toLLVMLinkage(linkage));

    if (init) {
        var->setInitializer(init);
    } else {
        if ((linkage != Linkage::Extern)
                && (linkage != Linkage::Extern_C)
                && (linkage != Linkage::Extern_Weak)) {
            has_initialiser = 1;
            if (r_type->points_to) {
                llvm::ConstantPointerNull *mynullptr =
                    llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(rdttype)
                    );
                var->setInitializer(mynullptr);
            } else if (r_type->struct_name.size()) {
                llvm::ConstantAggregateZero* const_values_init =
                    llvm::ConstantAggregateZero::get(rdttype);
                var->setInitializer(const_values_init);
            } else if (r_type->is_array) {
                llvm::ConstantAggregateZero* const_values_init =
                    llvm::ConstantAggregateZero::get(rdttype);
                var->setInitializer(const_values_init);
            } else if (r_type->isIntegerType()) {
                var->setInitializer(
                    ctx->nt->getConstantInt(
                        llvm::IntegerType::get(
                            llvm::getGlobalContext(),
                            ctx->nt->internalSizeToRealSize(
                                r_type->getIntegerSize()
                            )
                        ),
                        "0"
                    )
                );
            } else {
                has_initialiser = 0;
            }
            var2->has_initialiser = has_initialiser;
        }
    }

    var2->value = llvm::cast<llvm::Value>(var);

    return true;
}
}
