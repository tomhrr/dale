#include "Enum.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../BasicTypes/BasicTypes.h"
#include "../../../CoreForms/CoreForms.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../Linkage/Enum/Enum.h"
#include "../../Type/Type.h"
#include "../../ProcBody/ProcBody.h"

namespace dale
{
bool
FormTopLevelEnumParse(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    Node *top = node->list->at(2);
    const char *name = node->list->at(1)->token->str_value.c_str();

    symlist *lst = top->list;

    if (lst->size() < 4) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectMinimumNumberOfArgs,
            top,
            "enum", 3, (int) lst->size() - 1
        );
        ctx->er->addError(e);
        return false;
    }

    Node *lnk = (*lst)[1];
    int linkage = FormLinkageEnumParse(ctx, lnk);
    if (!linkage) {
        return false;
    }

    Node *enumtypen = (*lst)[2];

    Type *enumtype = FormTypeParse(units, enumtypen, false, false);
    if (!enumtype) {
        return false;
    }
    if (!enumtype->isIntegerType()) {
        Error *e = new Error(
            ErrorInst::Generator::EnumTypeMustBeInteger,
            enumtypen
        );
        ctx->er->addError(e);
        return false;
    }

    /* Enums have a maximum size of 64 bits. */
    llvm::Type *d_enumtype =
        ctx->toLLVMType(enumtype, NULL, false);
    if (!d_enumtype) {
        return false;
    }

    Node *elements = (*lst)[3];

    if (!elements->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            elements,
            "enum", "a list", "1", "an atom"
        );
        ctx->er->addError(e);
        return false;
    }

    Enum *enm = new Enum();
    enm->once_tag = units->top()->once_tag;
    enm->linkage = linkage;

    std::vector<Node *>::iterator iter =
        elements->list->begin();

    while (iter != elements->list->end()) {
        Node *n = (*iter);

        if (n->is_token) {
            if (n->token->type != TokenType::String) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    n,
                    "symbol", "enum element", n->token->tokenType()
                );
                ctx->er->addError(e);
                return false;
            }
            int res =
                enm->addMember(n->token->str_value.c_str());
            if (!res) {
                Error *e = new Error(
                    ErrorInst::Generator::RedeclarationOfEnumElement,
                    n, n->token->str_value.c_str()
                );
                ctx->er->addError(e);
                return false;
            }
        } else {
            n = units->top()->mp->parseOptionalMacroCall(n);
            if (!n) {
                return false;
            }
            symlist *mylst = n->list;
            if (mylst->size() != 2) {
                Error *e = new Error(
                    ErrorInst::Generator::IncorrectNumberOfArgs,
                    n, 2, mylst->size()
                );
                ctx->er->addError(e);
                return false;
            }
            Node *tn = (*mylst)[0];
            if (!tn->is_token) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    tn,
                    "atom", "enum element list", "list"
                );
                ctx->er->addError(e);
                return false;
            }
            if (tn->token->type != TokenType::String) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    tn,
                    "symbol", "enum element list",
                    tn->token->tokenType()
                );
                ctx->er->addError(e);
                return false;
            }
            Node *num = (*mylst)[1];
            if (!num->is_token) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    num,
                    "atom", "enum element list", "list"
                );
                ctx->er->addError(e);
                return false;
            }
            if (num->token->type != TokenType::Int) {
                Error *e = new Error(
                    ErrorInst::Generator::UnexpectedElement,
                    num,
                    "integer", "enum element index",
                    num->token->tokenType()
                );
                ctx->er->addError(e);
                return false;
            }

            llvm::ConstantInt *c =
                ctx->nt->getConstantInt(
                                   llvm::cast<llvm::IntegerType>(d_enumtype),
                                   num->token->str_value.c_str());
            int index = (int) c->getLimitedValue();
            int res =
                enm->addMember(tn->token->str_value.c_str(),
                                index);
            if (!res) {
                fprintf(stderr,
                        "Internal error: cannot add enum element.\n");
                abort();
            }
        }

        ++iter;
    }

    int res = ctx->ns()->addEnum(name, enm);
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfEnum,
            top,
            name
        );
        ctx->er->addError(e);
        return false;
    }

    Struct *enum_str = new Struct();
    enum_str->addMember("_enum_value", enumtype);
    enum_str->once_tag = units->top()->once_tag;
    enum_str->linkage =
        (linkage == EnumLinkage::Extern) ? StructLinkage::Extern
        : StructLinkage::Intern;

    std::vector<llvm::Type*> elements_llvm;
    elements_llvm.push_back(d_enumtype);

    /* Second arg here is 'ispacked'. */
    llvm::StructType *llvm_new_struct =
        llvm::StructType::create(llvm::getGlobalContext(),
                                 "new_enum_struct");
    llvm_new_struct->setBody(elements_llvm);

    std::string name2;
    name2.append("struct_");
    std::string name3;
    ctx->ns()->nameToSymbol(name, &name3);
    name2.append(name3);
    enum_str->internal_name.append(name2);

    llvm_new_struct->setName(name2.c_str());
    if (llvm_new_struct->getName() != llvm::StringRef(name2)) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfStruct,
            top,
            name
        );
        ctx->er->addError(e);
        return false;
    }

    enum_str->type = llvm_new_struct;

    res = ctx->ns()->addStruct(name, enum_str);
    if (!res) {
        Error *e = new Error(
            ErrorInst::Generator::RedeclarationOfStruct,
            top,
            name
        );
        ctx->er->addError(e);
        return false;
    }

    /* Got a struct type - return it. */
    Type *ttt = new Type();
    ttt->struct_name = name;

    std::vector<std::string> *new_namespaces =
        new std::vector<std::string>;

    ctx->setNamespacesForEnum(name, new_namespaces);
    ttt->namespaces = *new_namespaces;

    int flinkage = (linkage == EnumLinkage::Extern)
                   ? Linkage::Extern
                   : Linkage::Intern;

    BasicTypes::addEnum(ctx, units->top()->module, &(units->top()->once_tag), ttt,
                        enumtype, d_enumtype, flinkage);

    return true;
}
}
