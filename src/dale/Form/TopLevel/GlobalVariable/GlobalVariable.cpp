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

namespace dale
{
llvm::Constant *
parseLiteralElement(Units *units,
                    Node *top,
                    char *thing,
                    Type *type,
                    int *size)
{
    Context *ctx = units->top()->ctx;
    NativeTypes *nt = ctx->nt;
    TypeRegister *tr = ctx->tr;

    std::string t;
    type->toString(&t);

    if (type->base_type == BaseType::Bool) {
        llvm::APInt myint(1,
                          *thing);
        llvm::ConstantInt *myconstint =
            llvm::ConstantInt::get(llvm::getGlobalContext(),
                                   myint);
        return llvm::cast<llvm::Constant>(myconstint);
    }

    if (type->base_type == BaseType::Char) {
        llvm::APInt myint(8,
                          *thing);
        llvm::ConstantInt *myconstint =
            llvm::ConstantInt::get(llvm::getGlobalContext(),
                                   myint);
        return llvm::cast<llvm::Constant>(myconstint);
    }

    if (type->isIntegerType()) {
        union mynum {
            unsigned char udata[8];
            uint64_t      nvalue;
        } bling;
        bling.nvalue = 0;
        int pr_size =
            nt->internalSizeToRealSize(type->getIntegerSize());
        int i;
        if (pr_size == 128) {
            uint64_t nvalues[2];
            for (i = 0; i < 8; i++) {
                bling.udata[i] = *(thing + i);
            }
            nvalues[0] = bling.nvalue;
            for (i = 8; i < 16; i++) {
                bling.udata[i - 8] = *(thing + i);
            }
            nvalues[1] = bling.nvalue;
            llvm::APInt myint((unsigned) pr_size,
                              2,
                              nvalues);
            llvm::ConstantInt *myconstint =
                llvm::ConstantInt::get(llvm::getGlobalContext(),
                                       myint);
            return llvm::cast<llvm::Constant>(myconstint);
        } else {
            bling.nvalue = 0;
            for (i = 0; i < (pr_size / 8); i++) {
                bling.udata[i] = *(thing + i);
            }
            llvm::APInt myint(pr_size,
                              bling.nvalue);
            llvm::ConstantInt *myconstint =
                llvm::ConstantInt::get(llvm::getGlobalContext(),
                                       myint);
            return llvm::cast<llvm::Constant>(myconstint);
        }
    }

    if (type->base_type == BaseType::Float) {
        union float_hex {
            unsigned char udata[4];
            float         fvalue;
        } bling;
        bling.udata[3] = thing[3];
        bling.udata[2] = thing[2];
        bling.udata[1] = thing[1];
        bling.udata[0] = thing[0];
        llvm::APFloat myfloat(bling.fvalue);
        llvm::ConstantFP *myconstfloat =
            llvm::ConstantFP::get(llvm::getGlobalContext(),
                                  myfloat);
        return llvm::cast<llvm::Constant>(myconstfloat);
    }

    if (type->base_type == BaseType::Double) {
        union double_hex {
            unsigned char udata[8];
            double        dvalue;
        } bling;
        bling.udata[7] = thing[7];
        bling.udata[6] = thing[6];
        bling.udata[5] = thing[5];
        bling.udata[4] = thing[4];

        bling.udata[3] = thing[3];
        bling.udata[2] = thing[2];
        bling.udata[1] = thing[1];
        bling.udata[0] = thing[0];
        llvm::APFloat mydouble(bling.dvalue);
        llvm::ConstantFP *myconstdouble =
            llvm::ConstantFP::get(llvm::getGlobalContext(),
                                  mydouble);
        return llvm::cast<llvm::Constant>(myconstdouble);
    }

    if (type->struct_name.size()) {
        std::vector<llvm::Constant *> constants;

        Struct *st = ctx->getStruct(type);
        assert(st);

        std::vector<Type *>::iterator begin =
            st->member_types.begin();

        int i = 0;
        int last_el_size = -1;
        int last_offset = -1;
        int incr = 0;

        while (begin != st->member_types.end()) {
            Type *current = (*begin);
            size_t el_size =
                Operation::SizeofGet(units->top(), current);
            size_t offset =
                Operation::OffsetofGetByIndex(units->top(), type, i);
            size_t padding = 0;
            if (i != 0) {
                padding = (offset - last_offset - last_el_size);
            }
            if (padding) {
                Error *e = new Error(
                    ErrorInst::Generator::StructContainsPadding,
                    top
                );
                ctx->er->addError(e);
            }
            incr += padding;
            char *addr = thing;
            addr += offset;
            char aligned[256];
            memcpy(aligned, addr, el_size);

            llvm::Constant *el =
                parseLiteralElement(units,
                                    top,
                                    (char*) aligned,
                                    current,
                                    size);
            if (!el) {
                return NULL;
            }
            constants.push_back(el);
            last_offset  = offset - incr;
            last_el_size = el_size;
            ++i;
            ++begin;
        }

        llvm::Type *llvm_type =
            ctx->toLLVMType(type, NULL, false);
        if (!llvm_type) {
            return NULL;
        }

        llvm::StructType *llvm_st =
            llvm::cast<llvm::StructType>(llvm_type);

        llvm::Constant *init =
            llvm::ConstantStruct::get(
                llvm_st,
                constants
            );

        return init;
    }

    if (type->points_to && (type->points_to->base_type == BaseType::Char)) {
        char *temp =
            *(char**)
            (((uintptr_t) thing));
        *size = strlen(temp) + 1;
        llvm::Constant *myconststr =
            llvm::cast<llvm::Constant>(
#if D_LLVM_VERSION_MINOR < 2
                llvm::ConstantArray::get(
#else
                llvm::ConstantDataArray::getString(
#endif
                                         llvm::getGlobalContext(),
                                         temp,
                                         true)
            );

        std::string varname2;
        units->top()->getUnusedVarname(&varname2);

        Type *archar =
            tr->getArrayType(tr->type_char, *size);

        if (units->top()->module->getGlobalVariable(llvm::StringRef(varname2.c_str()))) {
            fprintf(stderr, "Internal error: "
                    "global variable already exists "
                    "in module ('%s').\n",
                    varname2.c_str());
            abort();
        }

        llvm::GlobalVariable *svar2 =
            llvm::cast<llvm::GlobalVariable>(
                units->top()->module->getOrInsertGlobal(varname2.c_str(),
                                       ctx->toLLVMType(archar, NULL, false))
            );

        svar2->setInitializer(myconststr);
        svar2->setConstant(true);
        svar2->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

        llvm::Value *temps[2];
        temps[0] = nt->getLLVMZero();
        temps[1] = nt->getLLVMZero();

        llvm::Constant *pce =
            llvm::ConstantExpr::getGetElementPtr(
                llvm::cast<llvm::Constant>(svar2),
                temps,
                2
            );

        return pce;
    }

    if (type->points_to) {
        if (*thing) {
            uint64_t value = *(uint64_t*)thing;
            if (sizeof(char*) == 4) {
                value <<= 32;
                if (!value) {
                    goto a;
                }
            }
            Error *e = new Error(
                ErrorInst::Generator::NonNullPointerInGlobalStructDeclaration,
                top
            );
            ctx->er->addError(e);
        }
a:
        llvm::Type *llvm_type =
            ctx->toLLVMType(type, NULL, false);
        if (!llvm_type) {
            return NULL;
        }
        llvm::Constant *pce =
            llvm::ConstantPointerNull::get(
                llvm::cast<llvm::PointerType>(llvm_type)
            );
        return pce;
    }

    if (type->is_array) {
        /* Take the portion devoted to whatever the element is,
         * and re-call this function. */
        size_t el_size =
            Operation::SizeofGet(units->top(), type->array_type);
        int i = 0;
        int els = type->array_size;
        std::vector<llvm::Constant *> constants;

        char elmemm[256];
        char *elmem = elmemm;

        for (i = 0; i < els; i++) {
            // Memset it to nothing.

            memset(elmem, 0, 256);

            // Offset thing by the index, cast to a char pointer, and
            // copy x elements into the new block.
            char *mp = (char*) thing;
            mp += (i * el_size);
            memcpy(elmem, mp, el_size);

            // Re-call parseLiteralElement, push the new constant onto
            // the vector.
            llvm::Constant *mycon =
                parseLiteralElement(units,
                                    top,
                                    elmem,
                                    type->array_type,
                                    size);

            constants.push_back(mycon);
        }

        llvm::Constant *mine =
            llvm::ConstantArray::get(
                llvm::cast<llvm::ArrayType>(
                    ctx->toLLVMType(type, top, false, false)
                ),
                constants
            );

        return mine;
    }

    Error *e = new Error(
        ErrorInst::Generator::CannotParseLiteral,
        top,
        t.c_str()
    );
    ctx->er->addError(e);

    return NULL;
}

static int myn = 0;
/* Size is only set when you are parsing a string literal - it
 * will contain the final size of the returned array. */
llvm::Constant *
parseLiteral(Units *units,
             Type *type,
             Node *top,
             int *size)
{
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
                Error *e = new Error(
                    ErrorInst::Generator::IncorrectType,
                    top,
                    want.c_str(),
                    got.c_str()
                );
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

    std::string temp2;
    type->toString(&temp2);
    Error *e = new Error(
        ErrorInst::Generator::CannotParseLiteral,
        top,
        temp2.c_str()
    );
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
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            top,
            "var", "2"
        );
        char buf[100];
        sprintf(buf, "%d", (int) lst->size() - 1);
        e->addArgString(buf);
        ctx->er->addError(e);
        return false;
    } else if (lst->size() == 3) {
        has_initialiser = 0;
    } else if (lst->size() == 4) {
        has_initialiser = 1;
    } else {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMaximumNumberOfArgs,
            top,
            "var", "3"
        );
        char buf[100];
        sprintf(buf, "%d", (int) lst->size() - 1);
        e->addArgString(buf);
        ctx->er->addError(e);
        return false;
    }

    int linkage = FormLinkageParse(ctx, (*lst)[1]);

    Type *r_type = FormTypeParse(units, (*lst)[2], false, false);
    if (r_type == NULL) {
        return false;
    }
    if (r_type->array_type && (r_type->array_size == 0)) {
        Error *e = new Error(
            ErrorInst::Generator::ZeroLengthGlobalArraysAreUnsupported,
            top
        );
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
        Error *e = new Error(
            ErrorInst::Generator::RedefinitionOfVariable,
            top,
            name
        );
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
