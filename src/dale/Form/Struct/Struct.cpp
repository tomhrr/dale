#include "Struct.h"
#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Function/Function.h"
#include "../../CoreForms/CoreForms.h"
#include "../Linkage/Linkage.h"
#include "../Linkage/Struct/Struct.h"
#include "../Type/Type.h"
#include "../Argument/Argument.h"
#include "../ProcBody/ProcBody.h"
#include "../../llvm_Function.h"

namespace dale
{
static int anonstructcount = 0;

bool
addOpaqueStruct(Generator *gen, const char *name, Node *top,
                int linkage, int must_init)
{
    Context *ctx = gen->ctx;

    if (!top) {
        top = new Node();
    }

    llvm::StructType *sty = llvm::StructType::create(
                                llvm::getGlobalContext(), "created_opaque_type");

    std::string name2;
    name2.append("struct_");
    std::string name3;
    ctx->ns()->nameToSymbol(name, &name3);
    name2.append(name3);

    sty->setName(name2.c_str());

    Struct *new_struct = new Struct();
    new_struct->must_init = must_init;
    new_struct->type = sty;
    new_struct->is_opaque = 1;
    new_struct->linkage = linkage;
    new_struct->internal_name.append(name2.c_str());
    new_struct->once_tag = gen->current_once_tag;

    if (!ctx->ns()->addStruct(name, new_struct)) {
        /* Only an error if there is not an existing struct. This
         * used to add an error message if the struct had already
         * been fully defined, but that is not an error in C, so
         * it won't be here, either. */
        Struct *temp = ctx->getStruct(name);
        if (!temp) {
            Error *e = new Error(
                ErrorInst::Generator::UnableToParseForm,
                top
            );
            ctx->er->addError(e);
            return false;
        }
    }

    /* On upgrading to 3, if a struct was not used in a module, it
     * was not included when the module was output. To maintain
     * the previous behaviour, add an intern function declaration
     * that takes no arguments and returns a value of the type of
     * this struct. */

    std::vector<llvm::Type*> args;
    llvm::FunctionType *ft =
        getFunctionType(
            sty,
            args,
            false
        );

    int index = 0;
    char buf[100];
    for (;;) {
        sprintf(buf, "__retain_struct_%d", index);
        if (!gen->mod->getFunction(buf)) {
            gen->mod->getOrInsertFunction(buf, ft);
            break;
        }
        ++index;
    }

    return true;
}

bool 
FormStructParse(Generator *gen,
      Node *top,
      const char *name)
{
    Context *ctx = gen->ctx;

    anonstructcount++;

    if (!ctx->er->assertArgNums("struct", top, 1, 3)) {
        return false;
    }

    symlist *lst = top->list;
    int must_init = 0;

    /* Struct attributes. */

    int next_index = 1;
    Node *test = ((*lst)[next_index]);
    if (test->is_list
            && test->list->at(0)->is_token
            && !(test->list->at(0)->token
                 ->str_value.compare("attr"))) {
        symlist *attr_list = test->list;
        std::vector<Node*>::iterator b = attr_list->begin(),
                                     e = attr_list->end();
        ++b;
        for (; b != e; ++b) {
            if ((*b)->is_list) {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                ctx->er->addError(e);
                return false;
            }
            if (!((*b)->token->str_value.compare("must-init"))) {
                must_init = 1;
            } else {
                Error *e = new Error(
                    ErrorInst::Generator::InvalidAttribute,
                    (*b)
                );
                ctx->er->addError(e);
                return false;
            }
        }
        ++next_index;
    }

    int linkage = FormLinkageStructParse(ctx, (*lst)[next_index]);
    if (!linkage) {
        return false;
    }
    ++next_index;

    bool res = addOpaqueStruct(gen, name, top, linkage, must_init);
    if (!res) {
        return false;
    }

    /* If the list contains two elements (name and linkage), or
     * three elements (name, attributes and linkage), the struct
     * is actually opaque, so return now. */

    if ((lst->size() == 2) || ((lst->size() == 3) && (next_index == 3))) {
        return true;
    }

    /* Parse elements - push onto the list that gets created. */

    Node *nelements = (*lst)[next_index];

    if (!nelements->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            nelements,
            "struct", "a list", "1", "an atom"
        );
        ctx->er->addError(e);
        return false;
    }

    symlist *elements = nelements->list;

    Variable *var;

    std::vector<Variable *> *elements_internal =
        new std::vector<Variable *>;

    std::vector<Node *>::iterator node_iter;
    node_iter = elements->begin();

    while (node_iter != elements->end()) {

        var = new Variable();
        var->type = NULL;

        FormArgumentParse(gen, var, (*node_iter), true, true, false);
        if (!var || !var->type) {
            Error *e = new Error(
                ErrorInst::Generator::InvalidType,
                (*node_iter)
            );
            ctx->er->addError(e);
            return false;
        }

        /* Can't have non-pointer void. */
        if (var->type->base_type == BaseType::Void) {
            Error *e = new Error(
                ErrorInst::Generator::TypeNotAllowedInStruct,
                (*node_iter),
                "void"
            );
            ctx->er->addError(e);
            delete var;
            return false;
        }

        /* This code can't be hit at the moment, but is left here
         * just in case. */
        if (var->type->base_type == BaseType::VarArgs) {
            Error *e = new Error(
                ErrorInst::Generator::TypeNotAllowedInStruct,
                (*node_iter),
                "varargs"
            );
            ctx->er->addError(e);
            delete var;
            return false;
        }

        elements_internal->push_back(var);

        ++node_iter;
    }

    /* Convert to llvm args and add the struct to the module. */

    std::vector<llvm::Type*> elements_llvm;

    std::vector<Variable *>::iterator iter;
    iter = elements_internal->begin();
    llvm::Type *temp;

    while (iter != elements_internal->end()) {
        temp = ctx->toLLVMType((*iter)->type, NULL, false);
        if (!temp) {
            return false;
        }
        elements_llvm.push_back(temp);
        ++iter;
    }

    std::string name2;

    std::string name3;
    ctx->ns()->nameToSymbol(name, &name3);
    name2.append(name3);

    bool already_exists = true;

    Struct *new_struct;

    if (already_exists) {
        /* If the struct does not already exist in context, then
         * there has been some strange error. */
        Struct *temp = ctx->getStruct(name);
        if (!temp) {
            Error *e = new Error(
                ErrorInst::Generator::UnableToParseForm,
                top
            );
            ctx->er->addError(e);
            return false;
        }

        /* If it does exist, but is not opaque, then it cannot be
         * redefined. */

        if (!temp->is_opaque) {
            Error *e = new Error(
                ErrorInst::Generator::RedeclarationOfStruct,
                top,
                name
            );
            ctx->er->addError(e);
            return false;
        }

        /* If it does exist, and is opaque, get its Type, cast it
         * to a StructType and add the elements. */

        llvm::StructType *opaque_struct_type =
            llvm::cast<llvm::StructType>(temp->type);
        opaque_struct_type->setBody(
            llvm::ArrayRef<llvm::Type*>(elements_llvm)
        );
        new_struct = temp;
        new_struct->is_opaque = 0;
    } else {
        new_struct = new Struct();
    }

    /* Re-get the type from the module, because the type in
     * llvm_new_struct will be buggered if the module factors
     * types out (e.g. where the current structure is the same as
     * some previously defined structure and has a
     * self-reference). */

    new_struct->internal_name.clear();
    new_struct->internal_name.append(name2.c_str());

    new_struct->linkage = linkage;

    iter = elements_internal->begin();

    while (iter != elements_internal->end()) {
        new_struct->addMember((*iter)->name.c_str(),
                              (*iter)->type);
        delete (*iter);
        ++iter;
    }

    elements_internal->clear();
    delete elements_internal;

    if (!already_exists) {
        if (!ctx->ns()->addStruct(name, new_struct)) {
            printf("Does not already exist, but can't add\n");
            Error *e = new Error(
                ErrorInst::Generator::RedeclarationOfStruct,
                top,
                name
            );
            ctx->er->addError(e);
            return false;
        }
    }

    return true;
}
}
