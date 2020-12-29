#include "Struct.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../CoreForms/CoreForms.h"
#include "../../Function/Function.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Units/Units.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../../llvm_Function.h"
#include "../Linkage/Linkage.h"
#include "../Linkage/Struct/Struct.h"
#include "../Parameter/Parameter.h"
#include "../ProcBody/ProcBody.h"
#include "../Type/Type.h"

using namespace dale::ErrorInst;

namespace dale {
static int anon_struct_index = 0;
static int retain_struct_index = 0;

bool addOpaqueStruct(Units *units, const char *name, Node *top,
                     int linkage) {
    Context *ctx = units->top()->ctx;

    llvm::StructType *llvm_st =
        llvm::StructType::create(*getContext(), "created_opaque_type");

    std::string symbol;
    ctx->ns()->nameToSymbol(name, &symbol);

    std::string llvm_st_name;
    llvm_st_name.append("struct_");
    llvm_st_name.append(symbol);

    llvm_st->setName(llvm_st_name.c_str());

    Struct *st = new Struct();
    st->type = llvm_st;
    st->is_opaque = true;
    st->linkage = linkage;
    st->symbol.append(llvm_st_name.c_str());
    st->once_tag = units->top()->once_tag;

    if (!ctx->ns()->addStruct(name, st)) {
        /* If the struct already exists, this is not an error. */
        if (!ctx->getStruct(name)) {
            Error *e = new Error(UnableToParseForm, nullNode());
            ctx->er->addError(e);
            return false;
        }
    }

    /* If a struct is not used in a module, it is not included when
     * the module is serialised.  Add an intern function declaration
     * that takes no arguments and returns a value of the type of this
     * struct, so as to ensure the struct will always be included when
     * serialising. */

    char buf[16];
    snprintf(buf, sizeof(buf), "__rs%d", ++retain_struct_index);
    if (!units->top()->module->getFunction(buf)) {
        std::vector<llvm::Type *> args;
        args.push_back(llvm_st);
        llvm::FunctionType *ft =
            getFunctionType(ctx->toLLVMType(ctx->tr->type_void, NULL, true),
                            args, false);
        llvm::Function *fn = llvm::Function::Create(
            ft, llvm::GlobalValue::WeakODRLinkage, buf,
            units->top()->module);
        llvm::BasicBlock *block =
            llvm::BasicBlock::Create(*getContext(), "entry", fn);
        llvm::IRBuilder<> builder(block);
        builder.CreateRetVoid();
    }

    return true;
}

bool FormStructParse(Units *units, Node *top, const char *name) {
    Context *ctx = units->top()->ctx;

    if (!ctx->er->assertArgNums("struct", top, 1, 3)) {
        return false;
    }

    ++anon_struct_index;

    std::vector<Node *> *lst = top->list;

    int next_index = 1;
    int linkage = FormLinkageStructParse(ctx, (*lst)[next_index]);
    if (!linkage) {
        return false;
    }
    ++next_index;

    bool res = addOpaqueStruct(units, name, top, linkage);
    if (!res) {
        return false;
    }

    /* If the list contains two members (name and linkage), the struct
     * is actually opaque, so return now. */

    if (lst->size() == 2) {
        return true;
    }

    /* Parse the struct members. */

    Node *members_node = (*lst)[next_index];

    if (!members_node->is_list) {
        Error *e = new Error(IncorrectArgType, members_node, "struct",
                             "a list", "1", "an atom");
        ctx->er->addError(e);
        return false;
    }

    std::vector<Node *> *members = members_node->list;

    std::vector<Variable *> members_internal;
    for (std::vector<Node *>::iterator b = members->begin(),
                                       e = members->end();
         b != e; ++b) {
        Variable *var = new Variable();
        var->type = NULL;

        FormParameterParse(units, var, (*b), true, true, false, false);
        if (!var->type) {
            Error *e = new Error(InvalidType, (*b));
            ctx->er->addError(e);
            delete var;
            return false;
        }

        if (var->type->base_type == BaseType::Void) {
            Error *e = new Error(TypeNotAllowedInStruct, (*b), "void");
            ctx->er->addError(e);
            delete var;
            return false;
        }

        members_internal.push_back(var);
    }

    /* Convert the members to LLVM types and add the struct to the
     * module. */

    std::vector<llvm::Type *> members_llvm;
    for (std::vector<Variable *>::iterator b = members_internal.begin(),
                                           e = members_internal.end();
         b != e; ++b) {
        llvm::Type *type = ctx->toLLVMType((*b)->type, NULL, false);
        if (!type) {
            return false;
        }
        members_llvm.push_back(type);
    }

    std::string symbol;
    ctx->ns()->nameToSymbol(name, &symbol);

    /* If the struct does not already exist in context, then there has
     * been some strange error. */
    Struct *st = ctx->getStruct(name);
    if (!st) {
        Error *e = new Error(UnableToParseForm, top);
        ctx->er->addError(e);
        return false;
    }

    /* If it does exist, but is not opaque, then it cannot be
     * redefined. */
    if (!st->is_opaque) {
        Error *e = new Error(RedeclarationOfStruct, top, name);
        ctx->er->addError(e);
        return false;
    }

    /* Get the struct's type, cast it to a  StructType, and add the
     * members. */
    llvm::StructType *opaque_struct_type =
        llvm::cast<llvm::StructType>(st->type);
    opaque_struct_type->setBody(
        llvm::ArrayRef<llvm::Type *>(members_llvm));
    st->is_opaque = false;

    st->symbol.clear();
    st->symbol.append(symbol.c_str());

    st->linkage = linkage;

    for (std::vector<Variable *>::iterator b = members_internal.begin(),
                                           e = members_internal.end();
         b != e; ++b) {
        st->addMember((*b)->name.c_str(), (*b)->type);
        delete (*b);
    }

    return true;
}
}
