#include "Literal.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../Error/Error.h"
#include "../../Linkage/Linkage.h"
#include "../../Operation/Cast/Cast.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../Utils/Utils.h"

using namespace dale::ErrorInst;

namespace dale {
void substituteChars(std::string *str) {
    size_t pos = 0;
    while ((pos = str->find("\\n", pos)) != std::string::npos) {
        str->replace(pos, 2, "\n");
    }
    pos = 0;
    while ((pos = str->find("\\t", pos)) != std::string::npos) {
        str->replace(pos, 2, "\t");
    }
    pos = 0;
    while ((pos = str->find("\\\\", pos)) != std::string::npos) {
        str->replace(pos, 2, "\\");
    }
}

llvm::Constant *stringLiteralToConstant(Units *units, Type *type,
                                        Node *node, int *size) {
    Context *ctx = units->top()->ctx;

    if (!node->is_token) {
        Error *e = new Error(UnexpectedElement, node, "atom", "literal",
                             "list");
        ctx->er->addError(e);
        return NULL;
    }
    Token *t = node->token;

    if (type->base_type == BaseType::Int) {
        if (t->type != TokenType::Int) {
            Error *e = new Error(UnexpectedElement, node, "integer",
                                 "literal", t->tokenType());
            ctx->er->addError(e);
            return NULL;
        }

        return ctx->nt->getConstantInt(ctx->nt->getNativeIntType(),
                                       t->str_value.c_str());
    }

    int underlying_type =
        (!type->base_type && type->points_to)
            ? type->points_to->base_type
            : (type->is_array) ? type->array_type->base_type : 0;

    if (underlying_type == BaseType::Char) {
        if (t->type != TokenType::StringLiteral) {
            Error *e = new Error(UnexpectedElement, node, "string",
                                 "literal", t->tokenType());
            ctx->er->addError(e);
            return NULL;
        }

        substituteChars(&(t->str_value));
        std::string value = t->str_value.c_str();

        *size = value.size() + 1;

        return getStringConstantArray(value.c_str());
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, node, type_str.c_str());
    ctx->er->addError(e);
    return NULL;
}

void FormStringLiteralParse(Units *units, Context *ctx,
                            llvm::BasicBlock *block, Node *node,
                            ParseResult *pr) {
    Type *type_char = ctx->tr->type_char;
    Type *type_cchar = ctx->tr->getConstType(type_char);
    Type *type_pcchar = ctx->tr->getPointerType(type_cchar);

    TypeRegister *tr = ctx->tr;
    std::string var_name;
    units->top()->getUnusedVarName(&var_name);

    substituteChars(&(node->token->str_value));
    std::string value = node->token->str_value.c_str();

    int size = value.size() + 1;
    Type *char_array_type = tr->getArrayType(tr->type_char, size);
    llvm::Type *llvm_type =
        ctx->toLLVMType(char_array_type, NULL, false);

    llvm::Module *mod = units->top()->module;
    assert(!mod->getGlobalVariable(llvm::StringRef(var_name.c_str())));

    llvm::GlobalVariable *var = llvm::cast<llvm::GlobalVariable>(
        mod->getOrInsertGlobal(var_name.c_str(), llvm_type));

    llvm::Constant *constr_str = getStringConstantArray(value.c_str());
    var->setInitializer(constr_str);
    var->setConstant(true);
    var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));

    llvm::Constant *const_pchar = createConstantGEP(
        llvm::cast<llvm::Constant>(var), ctx->nt->getTwoLLVMZeros());
    pr->set(block, type_pcchar,
            llvm::dyn_cast<llvm::Value>(const_pchar));
}

void FormFloatingPointLiteralParse(Context *ctx, Type *wanted_type,
                                   llvm::BasicBlock *block, Token *t,
                                   ParseResult *pr) {
    if (wanted_type && wanted_type->base_type == BaseType::Float) {
        pr->set(block, ctx->tr->type_float,
                llvm::ConstantFP::get(
                    llvm::Type::getFloatTy(*getContext()),
                    llvm::StringRef(t->str_value.c_str())));
    } else if (wanted_type &&
               wanted_type->base_type == BaseType::Double) {
        pr->set(block, ctx->tr->type_double,
                llvm::ConstantFP::get(
                    llvm::Type::getDoubleTy(*getContext()),
                    llvm::StringRef(t->str_value.c_str())));
    } else if (wanted_type &&
               wanted_type->base_type == BaseType::LongDouble) {
        pr->set(block, ctx->tr->type_longdouble,
                llvm::ConstantFP::get(
                    ctx->nt->getNativeLongDoubleType(),
                    llvm::StringRef(t->str_value.c_str())));
    } else {
        pr->set(block, ctx->tr->type_float,
                llvm::ConstantFP::get(
                    llvm::Type::getFloatTy(*getContext()),
                    llvm::StringRef(t->str_value.c_str())));
    }

    return;
}

void FormIntegerLiteralParse(Context *ctx, Type *wanted_type,
                             llvm::BasicBlock *block, Token *t,
                             ParseResult *pr) {
    if (wanted_type && wanted_type->isIntegerType()) {
        int int_size = ctx->nt->internalSizeToRealSize(
            wanted_type->getIntegerSize());
        pr->set(block, ctx->tr->getBasicType(wanted_type->base_type),
                ctx->nt->getConstantInt(
                    llvm::IntegerType::get(*getContext(), int_size),
                    t->str_value.c_str()));
    } else {
        pr->set(block, ctx->tr->type_int,
                ctx->nt->getConstantInt(ctx->nt->getNativeIntType(),
                                        t->str_value.c_str()));
    }

    return;
}

void FormBoolLiteralParse(Context *ctx, llvm::BasicBlock *block,
                          Node *node, ParseResult *pr) {
    Token *t = node->token;
    int is_true = !t->str_value.compare("true");
    int is_false = !t->str_value.compare("false");

    if (is_true || is_false) {
        pr->set(block, ctx->tr->type_bool,
                llvm::ConstantInt::get(
                    llvm::Type::getInt8Ty(*getContext()), is_true));
    }
}

void FormCharLiteralParse(Context *ctx, llvm::BasicBlock *block,
                          Node *node, ParseResult *pr) {
    Token *t = node->token;

    if ((t->str_value.size() >= 3) && (t->str_value[0] == '#') &&
        (t->str_value[1] == '\\')) {
        const char *value = t->str_value.c_str();
        value += 2;
        char c;

        if (!strcmp(value, "NULL")) {
            c = '\0';
        } else if (!strcmp(value, "TAB")) {
            c = '\t';
        } else if (!strcmp(value, "SPACE")) {
            c = ' ';
        } else if (!strcmp(value, "NEWLINE")) {
            c = '\n';
        } else if (!strcmp(value, "CARRIAGE")) {
            c = '\r';
        } else if (!strcmp(value, "EOF")) {
            c = EOF;
        } else {
            if (strlen(value) != 1) {
                Error *e = new Error(InvalidChar, node, value);
                ctx->er->addError(e);
                return;
            }
            c = value[0];
        }

        pr->set(
            block, ctx->tr->type_char,
            llvm::ConstantInt::get(ctx->nt->getNativeCharType(), c));
    }
}

bool arrayLiteralParse(Units *units, Type *type, Node *node,
                       ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    if (!node->is_list) {
        return false;
    }
    std::vector<llvm::Constant *> constants;
    std::vector<Node *> *lst = node->list;
    if (lst->size() < 2) {
        return false;
    }
    Node *first = lst->at(0);
    if (!first->is_token || first->token->str_value.compare("array")) {
        return false;
    }

    for (std::vector<Node *>::iterator b = (lst->begin() + 1),
                                       e = lst->end();
         b != e; ++b) {
        ParseResult element_pr;
        bool res = FormLiteralParse(units, type->array_type, *b,
                                    &element_pr);
        if (!res) {
            return false;
        }
        llvm::Constant *constant =
            llvm::dyn_cast<llvm::Constant>(element_pr.getValue(ctx));

        constants.push_back(constant);
    }

    llvm::Constant *const_arr = llvm::ConstantArray::get(
        llvm::cast<llvm::ArrayType>(
            ctx->toLLVMType(type, node, false, false)),
        constants);
    pr->set(pr->block, type, llvm::dyn_cast<llvm::Value>(const_arr));
    return true;
}

bool structLiteralParse(Units *units, Type *type, Node *node,
                        ParseResult *pr) {
    Context *ctx = units->top()->ctx;
    Struct *st = ctx->getStruct(type);
    if (!node->is_list) {
        return false;
    }
    std::vector<llvm::Constant *> constants;
    std::vector<Node *> *lst = node->list;

    for (std::vector<Node *>::iterator b = lst->begin(), e = lst->end();
         b != e; ++b) {
        Node *member_node = (*b);
        if (!member_node->is_list) {
            return false;
        }
        std::vector<Node *> *member_lst = member_node->list;
        if (member_lst->size() != 2) {
            return false;
        }
        Node *name_node = (*member_lst)[0];
        Node *value_node = (*member_lst)[1];

        if (!name_node->is_token) {
            return false;
        }

        const char *name = name_node->token->str_value.c_str();
        Type *st_type = st->nameToType(name);
        if (!st_type) {
            return false;
        }

        ParseResult element_pr;
        bool res = FormLiteralParse(units, st_type, value_node,
                                    &element_pr);
        if (!res) {
            return false;
        }
        llvm::Constant *constant =
            llvm::dyn_cast<llvm::Constant>(element_pr.getValue(ctx));
        constants.push_back(constant);
    }

    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return false;
    }

    llvm::StructType *llvm_st = llvm::cast<llvm::StructType>(llvm_type);
    llvm::Constant *const_st =
        llvm::ConstantStruct::get(llvm_st, constants);
    pr->set(pr->block, type, llvm::dyn_cast<llvm::Value>(const_st));
    return true;
}

bool FormLiteralParse(Units *units, Type *type, Node *node,
                      ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    if (type->isIntegerType() && node->is_token &&
        (node->token->type == TokenType::Int)) {
        FormIntegerLiteralParse(ctx, type, pr->block, node->token, pr);
        return true;
    } else if (type->isFloatingPointType() && node->is_token &&
               (node->token->type == TokenType::FloatingPoint)) {
        FormFloatingPointLiteralParse(ctx, type, pr->block, node->token,
                                      pr);
        return true;
    } else if (node->is_token &&
               (node->token->type == TokenType::StringLiteral)) {
        FormStringLiteralParse(units, ctx, pr->block, node, pr);
        return true;
    } else if (type->array_type) {
        return arrayLiteralParse(units, type, node, pr);
    } else if (type->struct_name.size()) {
        return structLiteralParse(units, type, node, pr);
    } else {
        return false;
    }
}
}
