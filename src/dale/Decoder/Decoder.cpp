#include "Decoder.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../Form/Linkage/Linkage.h"
#include "../Form/Proc/Token/Token.h"
#include "../Form/ProcBody/ProcBody.h"
#include "../Form/Type/Type.h"
#include "../Form/Utils/Utils.h"
#include "../Linkage/Linkage.h"
#include "../Node/Node.h"
#include "../Operation/Cast/Cast.h"
#include "../Operation/Copy/Copy.h"
#include "../Operation/Offsetof/Offsetof.h"
#include "../Operation/Sizeof/Sizeof.h"
#include "../Units/Units.h"
#include "../llvmUtils/llvmUtils.h"
#include "Config.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/Debug.h"

using namespace dale::ErrorInst;

namespace dale {
llvm::Constant *apIntToConstant(llvm::APInt const &ap_int) {
    llvm::ConstantInt *const_int =
        llvm::ConstantInt::get(*getContext(), ap_int);
    return llvm::cast<llvm::Constant>(const_int);
}

llvm::Constant *apFloatToConstant(llvm::APFloat const &ap_float) {
    llvm::ConstantFP *const_float =
        llvm::ConstantFP::get(*getContext(), ap_float);
    return llvm::cast<llvm::Constant>(const_float);
}

llvm::Constant *parseSmallLiteralInteger(int size, const char *data) {
    llvm::APInt ap_int(size, *data);
    return apIntToConstant(ap_int);
}

llvm::Constant *decodeRawInteger(int size, const char *data) {
    union uchar_uint64 {
        unsigned char c[8];
        uint64_t n;
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
        llvm::APInt myint((unsigned)size, 2, ns);
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

llvm::Constant *decodeRawFloat(char *data) {
    union uchar_float {
        unsigned char c[4];
        float n;
    } num;
    num.n = 0;

    for (int i = 3; i >= 0; i--) {
        num.c[i] = data[i];
    }
    llvm::APFloat ap_float(num.n);
    return apFloatToConstant(ap_float);
}

llvm::Constant *decodeRawDouble(char *data) {
    union uchar_double {
        unsigned char c[8];
        double n;
    } num;
    num.n = 0;

    for (int i = 7; i >= 0; i--) {
        num.c[i] = data[i];
    }
    llvm::APFloat ap_float(num.n);
    return apFloatToConstant(ap_float);
}

llvm::Constant *decodeRawStruct(Units *units, Node *top, char *data,
                                Type *type, int *size,
                                llvm::Module *prev_mod) {
    Context *ctx = units->top()->ctx;

    std::vector<llvm::Constant *> constants;

    Struct *st = ctx->getStruct(type);
    assert(st);

    int last_member_size = -1;
    int last_offset = -1;
    int index = 0;
    int total_padding = 0;

    for (std::vector<Type *>::iterator b = st->member_types.begin(),
                                       e = st->member_types.end();
         b != e; ++b) {
        Type *member_type = (*b);

        size_t member_size =
            Operation::SizeofGet(units->top(), member_type);
        size_t offset =
            Operation::OffsetofGetByIndex(units->top(), type, index);
        size_t padding = 0;
        if (index != 0) {
            padding = (offset - last_offset - last_member_size);
        }
        total_padding += padding;

        assert((member_size <= 256) && "struct member size too large");
        char *addr = data + offset;
        char aligned[256];
        memcpy(aligned, addr, member_size);

        llvm::Constant *member_value =
            decodeRawData(units, top, reinterpret_cast<char *>(aligned),
                          member_type, size, prev_mod);
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
    llvm::Constant *const_st =
        llvm::ConstantStruct::get(llvm_st, constants);

    return const_st;
}

llvm::Constant *decodeRawString(Units *units, Node *top, char *data,
                                Type *type, int *size,
                                llvm::Module *prev_mod) {
    Context *ctx = units->top()->ctx;
    TypeRegister *tr = ctx->tr;

    uint64_t value = *(reinterpret_cast<uint64_t *>(data));
    if (!value) {
        llvm::Type *llvm_type =
            ctx->toLLVMType(type, NULL, false);
        if (!llvm_type) {
            return NULL;
        }
        return getNullPointer(llvm_type);
    }

    /* data contains a char pointer, hence the cast. */
    char *str = *(reinterpret_cast<char **>(data));
    *size = strlen(str) + 1;
    llvm::Constant *constr_str = getStringConstantArray(str);

    std::string var_name;
    units->top()->getUnusedVarName(&var_name);

    Type *char_array_type = tr->getArrayType(tr->type_char, *size);
    llvm::Type *llvm_type =
        ctx->toLLVMType(char_array_type, NULL, false);

    assert(!prev_mod->getGlobalVariable(llvm::StringRef(var_name.c_str())));

    llvm::GlobalVariable *var = llvm::cast<llvm::GlobalVariable>(
        prev_mod->getOrInsertGlobal(var_name.c_str(), llvm_type));

    var->setInitializer(constr_str);
    var->setConstant(true);
    var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::Constant *const_pchar = createConstantGEP(
        llvm::cast<llvm::Constant>(var), ctx->nt->getTwoLLVMZeros());
    return const_pchar;
}

llvm::Constant *decodeRawPointer(Units *units, Node *top, char *data,
                                 Type *type, int *size) {
    Context *ctx = units->top()->ctx;

    uint64_t value = *(reinterpret_cast<uint64_t *>(data));
    if (value) {
        std::vector<Variable *> retrieved_var;
        ctx->getRetrievedVariables(&retrieved_var);
        for (std::vector<Variable *>::iterator
                 b = retrieved_var.begin(),
                 e = retrieved_var.end();
             b != e; ++b) {
            Variable *var = *b;
            uint64_t address = variableToAddress(units->top()->ee, var);
            if (address == value) {
                /* Iterate over the modules to find the 'real'
                 * variable. */
                for (std::vector<Unit *>::reverse_iterator
                        b = units->units.rbegin() + 1,
                        e = units->units.rend();
                        b != e;
                        ++b) {
                    llvm::Module *mod = (*b)->module;
                    llvm::GlobalVariable *gv =
                        mod->getGlobalVariable(var->symbol, true);
                    if (gv && !gv->isDeclaration()) {
                        return llvm::cast<llvm::Constant>(gv);
                    }
                }
            }
        }
        std::vector<Function *> retrieved_fn;
        ctx->getRetrievedFunctions(&retrieved_fn);
        for (std::vector<Function *>::iterator b = retrieved_fn.begin(),
                                               e = retrieved_fn.end();
             b != e; ++b) {
            Function *fn = *b;
            uint64_t address = functionToAddress(units->top(), fn);
            if (address == value) {
                /* Iterate over the modules to find the 'real'
                 * function. */
                for (std::vector<Unit *>::reverse_iterator
                        b = units->units.rbegin() + 1,
                        e = units->units.rend();
                        b != e;
                        ++b) {
                    llvm::Module *mod = (*b)->module;
                    llvm::Function *llvm_fn =
                        mod->getFunction(fn->symbol);
                    if (llvm_fn && !llvm_fn->isDeclaration()) {
                        return llvm::cast<llvm::Constant>(llvm_fn);
                    }
                }
            }
        }
        Error *e = new Error(UnableToResolvePointerAddress, top);
        ctx->er->addError(e);
        return NULL;
    }

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return NULL;
    }

    return getNullPointer(llvm_type);
}

llvm::Constant *decodeRawArray(Units *units, Node *top, char *data,
                               Type *type, int *size,
                               llvm::Module *prev_mod) {
    Context *ctx = units->top()->ctx;

    size_t member_size =
        Operation::SizeofGet(units->top(), type->array_type);
    assert((member_size <= 256) && "array member size too large");
    int members = type->array_size;
    std::vector<llvm::Constant *> constants;

    char mem_array[256];
    char *mem = mem_array;

    for (int i = 0; i < members; i++) {
        memset(mem, 0, 256);
        char *member_ptr =
            reinterpret_cast<char *>(data) + (i * member_size);
        memcpy(mem, member_ptr, member_size);

        llvm::Constant *const_member =
            decodeRawData(units, top, mem, type->array_type, size, prev_mod);
        if (!const_member) {
            return NULL;
        }

        constants.push_back(const_member);
    }

    llvm::Constant *const_arr = llvm::ConstantArray::get(
        llvm::cast<llvm::ArrayType>(
            ctx->toLLVMType(type, top, false, false)),
        constants);

    return const_arr;
}

llvm::Constant *decodeRawData(Units *units, Node *top, char *data,
                              Type *type, int *size,
                              llvm::Module *prev_mod) {
    Context *ctx = units->top()->ctx;
    NativeTypes *nt = ctx->nt;

    if (type->base_type == BaseType::Bool) {
        return parseSmallLiteralInteger(8, data);
    }

    if (type->base_type == BaseType::Char) {
        return parseSmallLiteralInteger(8, data);
    }

    if (type->isIntegerType()) {
        int real_size = nt->internalSizeToRealSize(type->getIntegerSize());
        return decodeRawInteger(real_size, data);
    }

    if (type->base_type == BaseType::Float) {
        return decodeRawFloat(data);
    }

    if (type->base_type == BaseType::Double) {
        return decodeRawDouble(data);
    }

    if (type->struct_name.size()) {
        return decodeRawStruct(units, top, data, type, size, prev_mod);
    }

    if (type->points_to &&
        (type->points_to->base_type == BaseType::Char)) {
        return decodeRawString(units, top, data, type, size, prev_mod);
    }

    if (type->points_to) {
        return decodeRawPointer(units, top, data, type, size);
    }

    if (type->is_array) {
        return decodeRawArray(units, top, data, type, size, prev_mod);
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, top, type_str.c_str());
    ctx->er->addError(e);

    return NULL;
}
}
