#include "Enum.h"
#include "Config.h"
#include "../../../Units/Units.h"
#include "../../../BasicTypes/BasicTypes.h"
#include "../../../CoreForms/CoreForms.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../Linkage/Enum/Enum.h"
#include "../../Type/Type.h"
#include "../../ProcBody/ProcBody.h"

using namespace dale::ErrorInst;

namespace dale
{
Node *
isValidEnumElement(Units *units, Node *node)
{
    Context *ctx = units->top()->ctx;

    if (node->is_token) {
        if (node->token->type != TokenType::String) {
            Error *e = new Error(UnexpectedElement, node,
                                 "symbol", "enum element",
                                 node->token->tokenType());
            ctx->er->addError(e);
            return NULL;
        }
    } else {
        node = units->top()->mp->parsePotentialMacroCall(node);
        if (!node) {
            return NULL;
        }
        std::vector<Node *> *lst = node->list;
        if (lst->size() != 2) {
            Error *e = new Error(IncorrectNumberOfArgs,
                                 node, "enum element", 2, lst->size());
            ctx->er->addError(e);
            return NULL;
        }
        Node *token_node = (*lst)[0];
        if (!token_node->is_token) {
            Error *e = new Error(UnexpectedElement, token_node,
                                 "atom", "enum element list", "list");
            ctx->er->addError(e);
            return NULL;
        }
        if (token_node->token->type != TokenType::String) {
            Error *e = new Error(UnexpectedElement, token_node,
                                 "symbol", "enum element list",
                                 token_node->token->tokenType());
            ctx->er->addError(e);
            return NULL;
        }
        Node *number_node = (*lst)[1];
        if (!number_node->is_token) {
            Error *e = new Error(UnexpectedElement, number_node,
                                 "atom", "enum node element list", "list");
            ctx->er->addError(e);
            return NULL;
        }
        if (number_node->token->type != TokenType::Int) {
            Error *e = new Error(UnexpectedElement, number_node,
                                 "integer", "enum node element index",
                                 number_node->token->tokenType());
            ctx->er->addError(e);
            return NULL;
        }
    }

    return node;
}

bool
FormTopLevelEnumParse(Units *units, Node *node, const char *name)
{
    Context *ctx = units->top()->ctx;

    Node *top;
    if (!name) {
        Node *name_node = (*(node->list))[1];
        name = name_node->token->str_value.c_str();
        top = (*(node->list))[2];
    } else {
        top = node;
    }

    std::vector<Node *> *lst = top->list;
    Node *linkage_node   = (*lst)[1];
    Node *enum_type_node = (*lst)[2];
    Node *elements_node  = (*lst)[3];

    if (lst->size() < 4) {
        Error *e = new Error(IncorrectMinimumNumberOfArgs, top,
                             "enum", 3, (int) lst->size() - 1);
        ctx->er->addError(e);
        return false;
    }

    int linkage = FormLinkageEnumParse(ctx, linkage_node);
    if (!linkage) {
        return false;
    }

    Type *enum_type = FormTypeParse(units, enum_type_node, false, false);
    if (!enum_type) {
        return false;
    }
    if (!enum_type->isIntegerType()) {
        Error *e = new Error(EnumTypeMustBeInteger, enum_type_node);
        ctx->er->addError(e);
        return false;
    }

    llvm::Type *llvm_enum_type = ctx->toLLVMType(enum_type, NULL, false);
    if (!llvm_enum_type) {
        return false;
    }

    if (!elements_node->is_list) {
        Error *e = new Error(IncorrectArgType, elements_node,
                             "enum", "a list", "1", "an atom");
        ctx->er->addError(e);
        return false;
    }

    Enum *enum_obj = new Enum();
    enum_obj->once_tag = units->top()->once_tag;
    enum_obj->linkage  = linkage;

    for (std::vector<Node *>::iterator b = elements_node->list->begin(),
                                       e = elements_node->list->end();
            b != e;
            ++b) {
        Node *n = (*b);

        n = isValidEnumElement(units, n);
        if (!n) {
            return false;
        }
        bool res;
        if (n->is_token) {
            res = enum_obj->addMember(n->token->str_value.c_str());
        } else {
            std::vector<Node *> *lst = n->list;
            Node *token_node  = (*lst)[0];
            Node *number_node = (*lst)[1];

            llvm::ConstantInt *c =
                ctx->nt->getConstantInt(
                    llvm::cast<llvm::IntegerType>(llvm_enum_type),
                    number_node->token->str_value.c_str()
                );
            int index = (int) c->getLimitedValue();
            res = enum_obj->addMember(token_node->token->str_value.c_str(), index);
        }
        if (!res) {
            Error *e = new Error(RedeclarationOfEnumElement,
                                 n, n->token->str_value.c_str());
            ctx->er->addError(e);
            return false;
        }
    }

    bool res = ctx->ns()->addEnum(name, enum_obj);
    if (!res) {
        Error *e = new Error(RedeclarationOfEnum, top, name);
        ctx->er->addError(e);
        return false;
    }

    Struct *enum_st = new Struct();
    enum_st->addMember("_enum_value", enum_type);
    enum_st->once_tag = units->top()->once_tag;
    enum_st->linkage =
        (linkage == EnumLinkage::Extern)
            ? StructLinkage::Extern
            : StructLinkage::Intern;

    std::vector<llvm::Type*> elements_llvm;
    elements_llvm.push_back(llvm_enum_type);

    llvm::StructType *llvm_new_struct =
        llvm::StructType::create(llvm::getGlobalContext(), "new_enum_stuct");
    llvm_new_struct->setBody(elements_llvm);

    std::string symbol;
    ctx->ns()->nameToSymbol(name, &symbol);

    std::string llvm_name;
    llvm_name.append("struct_");
    llvm_name.append(symbol);

    enum_st->symbol.append(llvm_name);

    llvm_new_struct->setName(llvm_name.c_str());
    if (llvm_new_struct->getName() != llvm::StringRef(llvm_name)) {
        Error *e = new Error(RedeclarationOfStruct, top, name);
        ctx->er->addError(e);
        return false;
    }

    enum_st->type = llvm_new_struct;

    res = ctx->ns()->addStruct(name, enum_st);
    if (!res) {
        Error *e = new Error(RedeclarationOfStruct, top, name);
        ctx->er->addError(e);
        return false;
    }

    Type *final_enum_type = new Type();
    final_enum_type->struct_name = name;
    ctx->setNamespacesForEnum(name, &(final_enum_type->namespaces));

    int final_linkage =
        (linkage == EnumLinkage::Extern)
            ? Linkage::Extern
            : Linkage::Intern;

    BasicTypes::addEnum(ctx, units->top()->module, &(units->top()->once_tag),
                        final_enum_type, enum_type, llvm_enum_type,
                        final_linkage);

    for (std::map<std::string, int64_t>::iterator
            b = enum_obj->name_to_index.begin(),
            e = enum_obj->name_to_index.end();
            b != e;
            ++b) {
        if (ctx->ns()->getVariable(b->first.c_str())) {
            Error *e = new Error(RedefinitionOfVariable, top,
                                 b->first.c_str());
            ctx->er->addError(e);
            return false;
        }
        std::string new_name;
        /* todo: It would be ideal if this were not necessary, because
         * it will make external bindings a bit of a pain. */
        new_name.append("_enum_");
        if (linkage == Linkage::Extern_C) {
            new_name.append(b->first.c_str());
        } else {
            ctx->ns()->nameToSymbol(b->first.c_str(), &new_name);
        }

        llvm::Type *llvm_type = ctx->toLLVMType(enum_type, NULL,
                                                false, false);
        llvm::GlobalVariable *llvm_var =
            llvm::cast<llvm::GlobalVariable>(
                units->top()->module->getOrInsertGlobal(new_name.c_str(),
                                                        llvm_type)
            );

        llvm_var->setLinkage(ctx->toLLVMLinkage(linkage));
        llvm_var->setInitializer(
            llvm::ConstantInt::get(llvm_type, b->second)
        );
        llvm_var->setConstant(true);

        Variable *enum_var = new Variable();
        enum_var->name.append(b->first.c_str());
        enum_var->type = ctx->tr->getConstType(final_enum_type);
        enum_var->symbol.append(new_name);
        enum_var->once_tag = units->top()->once_tag;
        enum_var->linkage = linkage;
        enum_var->value = llvm_var;

        bool res = ctx->ns()->addVariable(b->first.c_str(), enum_var);
        assert(res && "unable to add enum element variable");
    }

    return true;
}
}
