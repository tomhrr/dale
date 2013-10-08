#include "GlobalVariable.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../../Operation/Sizeof/Sizeof.h"
#include "../../Linkage/Linkage.h"
#include "../../ProcBody/ProcBody.h"
#include "../../Type/Type.h"

namespace dale
{
namespace Form
{
namespace TopLevel
{
namespace GlobalVariable
{
llvm::FunctionType *getFunctionType(llvm::Type *t,
                                    std::vector<llvm::Type*> &v,
                                    bool b) {
    llvm::ArrayRef<llvm::Type*> temp(v);
    return llvm::FunctionType::get(t, temp, b);
}

static int myn = 0;
/* Size is only set when you are parsing a string literal - it
 * will contain the final size of the returned array. */
llvm::Constant *
parseLiteral(Generator *gen,
             Element::Type *type,
             Node *top,
             int *size)
{
    Context *ctx = gen->ctx;

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
        var = gen->parseOptionalMacroCall(var);
        if (var && var->is_token) {
            Element::Variable *gv =
                ctx->getVariable(var->token->str_value.c_str());
            if (!(type->points_to->isEqualTo(gv->type))) {
                std::string want;
                std::string got;
                gv->type->toStringProper(&got);
                type->toStringProper(&want);
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
    type->toStringProper(&str);

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
    char buf[255];
    sprintf(buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(buf, &new_name);

    if (gen->mod->getFunction(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::Constant *fnc =
        gen->mod->getOrInsertFunction(
            new_name.c_str(),
            ft
        );

    std::vector<Element::Variable*> args;

    llvm::Function *fn = llvm::cast<llvm::Function>(fnc);

    fn->setCallingConv(llvm::CallingConv::C);

    fn->setLinkage(ctx->toLLVMLinkage(dale::Linkage::Extern_C));

    Element::Function *dfn =
        new Element::Function(type, &args, fn, 0,
                              &new_name);
    dfn->linkage = dale::Linkage::Intern;
    int error_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    std::vector<Node*> nodes;
    nodes.push_back(top);
    Node *topwrapper = new Node(&nodes);

    Form::ProcBody::parse(gen, topwrapper, dfn, fn, 0, 0);
    int error_post_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);
    if (error_count != error_post_count) {
        return NULL;
    }

    llvm::Type *tttt = ctx->toLLVMType(
        ctx->tr->getBasicType(dale::Type::Void), NULL, true);
    llvm::FunctionType *wrapft =
        getFunctionType(
            tttt,
            mc_args,
            false
        );

    std::string wrap_new_name;
    char wrap_buf[255];
    sprintf(wrap_buf, "___myfn%d", myn++);
    ctx->ns()->nameToSymbol(wrap_buf, &wrap_new_name);

    if (gen->mod->getFunction(llvm::StringRef(wrap_new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "function already exists in module ('%s').\n",
                wrap_new_name.c_str());
        abort();
    }

    llvm::Constant *wrap_fnc =
        gen->mod->getOrInsertFunction(
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

    std::vector<Element::Type *> call_arg_types;
    Element::Type *ptype = ctx->tr->getPointerType(type);
    stl::push_back2(&call_arg_types, ptype, ptype);

    std::vector<llvm::Value *> call_args2;
    stl::push_back2(&call_args2, reta, reta2);

    if (Element::Function *or_setf =
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
        Operation::Cast::execute(ctx,
               block,
               reta,
               ctx->tr->getPointerType(type),
               ctx->tr->getPointerType(ctx->tr->getBasicType(dale::Type::Char)),
               top, 0, &temp_pr);
    if (!res) {
        return false;
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
        Operation::Cast::execute(ctx, 
               block,
               v,
               ctx->tr->getBasicType(dale::Type::IntPtr),
               ctx->tr->getPointerType(ctx->tr->getBasicType(dale::Type::Char)),
               top, 0, &storeor
              );
    if (!res) {
        return false;
    }
    llvm::Value *store = storeor.value;
    builder.SetInsertPoint(storeor.block);
    Element::Function *memcpy = ctx->getFunction("memcpy", NULL,
                                NULL, 0);
    if (!memcpy) {
        fprintf(stderr,
                "Internal error: no memcpy function available.\n");
        abort();
    }

    size_t struct_size =
        Operation::Sizeof::get(gen->unit_stack->top(), type);
    char buf5[5];
    sprintf(buf5, "%u", (unsigned) struct_size);

    std::vector<llvm::Value*> memcpy_args;
    memcpy_args.push_back(store);
    memcpy_args.push_back(retaa);
    memcpy_args.push_back(
        ctx->nt->getConstantInt(llvm::IntegerType::get(
                        llvm::getGlobalContext(),
                       32), buf5));
    builder.CreateCall(memcpy->llvm_function,
                       llvm::ArrayRef<llvm::Value*>(memcpy_args)
                      );

    /* Take this value, put it in the struct pointer. */

    builder.CreateRetVoid();

    void* fptr =
        gen->ee->getPointerToFunction(wrap_fn);
    if (!fptr) {
        fprintf(stderr,
                "Internal error: could not get pointer "
                "to function for literal.\n");
        abort();
    }

    ((void (*)(void)) fptr)();

    llvm::Constant *parsed =
        gen->parseLiteralElement(top, (char*) &thing, type, size);

    wrap_fn->eraseFromParent();
    (llvm::cast<llvm::Function>(fnc))->eraseFromParent();

    if (parsed) {
        return parsed;
    }

    std::string temp2;
    type->toStringProper(&temp2);
    Error *e = new Error(
        ErrorInst::Generator::CannotParseLiteral,
        top,
        temp2.c_str()
    );
    ctx->er->addError(e);
    return NULL;
}

bool parse(Generator *gen,
           Node *node)
{
    Context *ctx = gen->ctx;

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

    int linkage = Form::Linkage::parse(ctx, (*lst)[1]);

    Element::Type *r_type = Form::Type::parse(gen, (*lst)[2], false, false);
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
        n2 = gen->parseOptionalMacroCall((*lst)[3]);
        if (!n2) {
            return false;
        }
    }

    llvm::Constant *init =
        (has_initialiser)
        ? parseLiteral(gen, r_type, n2, &size)
        : NULL;

    if ((init == NULL) && (has_initialiser)) {
        return true;
    }

    std::string new_name;
    if (linkage == dale::Linkage::Extern_C) {
        new_name.append(name);
    } else {
        ctx->ns()->nameToSymbol(name, &new_name);
    }

    Element::Variable *check = ctx->getVariable(name);
    if (check
            && check->type->isEqualTo(r_type)
            && (check->linkage == linkage)
            && !has_initialiser) {
        /* Redeclaration of global variable - no problem. */
        return true;
    }

    /* Add the variable to the context. */

    Element::Variable *var2 = new Element::Variable();
    var2->name.append(name);
    var2->type = r_type;
    var2->internal_name.append(new_name);
    var2->once_tag = gen->current_once_tag;
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
        ((linkage != dale::Linkage::Auto)
         && (linkage != dale::Linkage::Intern));

    llvm::Type *rdttype =
        ctx->toLLVMType(r_type, top, false,
                       (has_extern_linkage && !has_initialiser));
    if (!rdttype) {
        return false;
    }

    /* Add the variable to the module. */

    if (gen->mod->getGlobalVariable(llvm::StringRef(new_name.c_str()))) {
        fprintf(stderr, "Internal error: "
                "global variable already exists in "
                "module ('%s').\n",
                new_name.c_str());
        abort();
    }

    llvm::GlobalVariable *var =
        llvm::cast<llvm::GlobalVariable>(
            gen->mod->getOrInsertGlobal(new_name.c_str(),
                                   rdttype)
        );

    var->setLinkage(ctx->toLLVMLinkage(linkage));

    if (init) {
        var->setInitializer(init);
    } else {
        if ((linkage != dale::Linkage::Extern)
                && (linkage != dale::Linkage::Extern_C)
                && (linkage != dale::Linkage::Extern_Weak)) {
            has_initialiser = 1;
            if (r_type->points_to) {
                llvm::ConstantPointerNull *mynullptr =
                    llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(rdttype)
                    );
                var->setInitializer(mynullptr);
            } else if (r_type->struct_name) {
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
}
}
}
