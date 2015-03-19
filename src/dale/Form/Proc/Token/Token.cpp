#include "Token.h"

#include "../../Literal/Enum/Enum.h"

#include <cstdio>

using namespace dale::ErrorInst;

namespace dale
{
llvm::Constant *
stringLiteralToConstant(Units *units, Type *type, Node *node, int *size)
{
    Context *ctx = units->top()->ctx;

    if (!node->is_token) {
        Error *e = new Error(UnexpectedElement, node,
                             "atom", "literal", "list");
        ctx->er->addError(e);
        return NULL;
    }
    Token *t = node->token;

    if (type->base_type == BaseType::Int) {
        if (t->type != TokenType::Int) {
            Error *e = new Error(UnexpectedElement, node,
                                 "integer", "literal", t->tokenType());
            ctx->er->addError(e);
            return NULL;
        }

        return
            ctx->nt->getConstantInt(ctx->nt->getNativeIntType(),
                                    t->str_value.c_str());
    }

    int underlying_type =
          (!type->base_type && type->points_to) ? type->points_to->base_type
        : (type->is_array)                      ? type->array_type->base_type
                                                : 0;

    if (underlying_type == BaseType::Char) {
        if (t->type != TokenType::StringLiteral) {
            Error *e = new Error(UnexpectedElement, node,
                                 "string", "literal", t->tokenType());
            ctx->er->addError(e);
            return NULL;
        }

        size_t pos = 0;
        std::string value = t->str_value;
        while ((pos = value.find("\\n", pos)) != std::string::npos) {
            value.replace(pos, 2, "\n");
        }

        *size = value.size() + 1;

        return getStringConstantArray(value.c_str());
    }

    std::string type_str;
    type->toString(&type_str);
    Error *e = new Error(CannotParseLiteral, node, type_str.c_str());
    ctx->er->addError(e);
    return NULL;
}

bool
parseStringLiteral(Units *units, Context *ctx, llvm::BasicBlock *block,
                   Node *node, ParseResult *pr)
{
    Type *type_char   = ctx->tr->type_char;
    Type *type_cchar  = ctx->tr->getConstType(type_char);
    Type *type_pcchar = ctx->tr->getPointerType(type_cchar);

    int size = 0;
    llvm::Constant *init = stringLiteralToConstant(units, type_pcchar,
                                                   node, &size);
    if (!init) {
        return false;
    }
    Type *str_type_sized =
        ctx->tr->getArrayType(ctx->tr->getConstType(type_char), size);

    llvm::Type *llvm_type = ctx->toLLVMType(str_type_sized, NULL, false);
    if (!llvm_type) {
        return false;
    }

    std::string varname;
    units->top()->getUnusedVarName(&varname);

    llvm::GlobalVariable *llvm_var =
        llvm::cast<llvm::GlobalVariable>(
            units->top()->module->getOrInsertGlobal(varname.c_str(),
                                                    llvm_type)
        );

    llvm_var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));
    llvm_var->setInitializer(init);
    llvm_var->setConstant(true);

    Variable *var = new Variable();
    var->name.append(varname.c_str());
    var->internal_name.append(varname);
    var->type = str_type_sized;
    var->value = llvm::cast<llvm::Value>(llvm_var);
    var->linkage = Linkage::Intern;

    bool res = ctx->ns()->addVariable(varname.c_str(), var);
    if (!res) {
        Error *e = new Error(RedefinitionOfVariable, node,
                             varname.c_str());
        ctx->er->addError(e);
        return false;
    }

    llvm::IRBuilder<> builder(block);
    llvm::Value *char_ptr =
        builder.CreateGEP(llvm::cast<llvm::Value>(var->value),
                          ctx->nt->getTwoLLVMZeros());
    pr->set(block, type_pcchar, char_ptr);
    return true;
}

void
parseFloatingPointLiteral(Context *ctx, Type *wanted_type,
                          llvm::BasicBlock *block, Token *t,
                          ParseResult *pr)
{
    if (wanted_type
            && wanted_type->base_type == BaseType::Float) {
        pr->set(block, ctx->tr->type_float,
                llvm::ConstantFP::get(
                    llvm::Type::getFloatTy(llvm::getGlobalContext()),
                    llvm::StringRef(t->str_value.c_str())
                ));
    } else if (wanted_type
                && wanted_type->base_type == BaseType::Double) {
        pr->set(block, ctx->tr->type_double,
                llvm::ConstantFP::get(
                    llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                    llvm::StringRef(t->str_value.c_str())
                ));
    } else if (wanted_type
                && wanted_type->base_type == BaseType::LongDouble) {
        pr->set(block, ctx->tr->type_longdouble,
                llvm::ConstantFP::get(
                    ctx->nt->getNativeLongDoubleType(),
                    llvm::StringRef(t->str_value.c_str())
                ));
    } else {
        pr->set(block, ctx->tr->type_float,
                llvm::ConstantFP::get(
                    llvm::Type::getFloatTy(llvm::getGlobalContext()),
                    llvm::StringRef(t->str_value.c_str())
                ));
    }

    return;
}

void
parseIntegerLiteral(Context *ctx, Type *wanted_type,
                    llvm::BasicBlock *block, Token *t,
                    ParseResult *pr)
{
    if (wanted_type && wanted_type->isIntegerType()) {
        int int_size =
            ctx->nt->internalSizeToRealSize(wanted_type->getIntegerSize());
        pr->set(block,
                ctx->tr->getBasicType(wanted_type->base_type),
                ctx->nt->getConstantInt(
                    llvm::IntegerType::get(llvm::getGlobalContext(),
                                           int_size),
                    t->str_value.c_str()
                ));
    } else {
        pr->set(block, ctx->tr->type_int,
                ctx->nt->getConstantInt(ctx->nt->getNativeIntType(),
                                        t->str_value.c_str()));
    }

    return;
}

void
parseBoolLiteral(Context *ctx, llvm::BasicBlock *block, Node *node,
                 ParseResult *pr)
{
    Token *t = node->token;
    int is_true  = !t->str_value.compare("true");
    int is_false = !t->str_value.compare("false");

    if (is_true || is_false) {
        pr->set(block, ctx->tr->type_bool,
                llvm::ConstantInt::get(
                    llvm::Type::getInt1Ty(llvm::getGlobalContext()),
                    is_true
                ));
    }
}

void
parseCharLiteral(Context *ctx, llvm::BasicBlock *block, Node *node,
                 ParseResult *pr)
{
    Token *t = node->token;

    if ((t->str_value.size() >= 3)
            && (t->str_value[0] == '#')
            && (t->str_value[1] == '\\')) {
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

        pr->set(block, ctx->tr->type_char,
                llvm::ConstantInt::get(ctx->nt->getNativeCharType(), c));
    }
}

void
parseVariableLiteral(Context *ctx, llvm::BasicBlock *block, Node *node,
                     bool get_address, ParseResult *pr)
{
    Token *t = node->token;
    Variable *var = ctx->getVariable(t->str_value.c_str());

    if (!var) {
        Error *e = new Error(VariableNotInScope, node,
                             t->str_value.c_str());
        ctx->er->addError(e);
        return;
    }

    llvm::IRBuilder<> builder(block);

    if (get_address) {
        pr->set(block, ctx->tr->getPointerType(var->type), var->value);
        return;
    }

    /* Array-type variables. */
    if (var->type->is_array) {
        llvm::Value *ptr_to_array =
            builder.CreateGEP(var->value, ctx->nt->getTwoLLVMZeros());

        pr->set(block, ctx->tr->getPointerType(var->type->array_type),
                ptr_to_array);

        pr->address_of_value = var->value;
        pr->value_is_lvalue = true;
        pr->type_of_address_of_value =
            ctx->tr->getPointerType(var->type);
        return;
    }

    /* All other variables. */
    pr->set(block, var->type,
            llvm::cast<llvm::Value>(builder.CreateLoad(var->value)));
    pr->address_of_value = var->value;
    pr->value_is_lvalue = 1;
}

bool
FormProcTokenParse(Units *units, Function *fn, llvm::BasicBlock *block,
                   Node *node, bool get_address, bool prefixed_with_core,
                   Type *wanted_type, ParseResult *pr)
{
    Context *ctx = units->top()->ctx;

    Token *t = node->token;

    if (t->type == TokenType::Int) {
        parseIntegerLiteral(ctx, wanted_type, block, t, pr);
        return true;
    } else if (t->type == TokenType::FloatingPoint) {
        parseFloatingPointLiteral(ctx, wanted_type, block, t, pr);
        return true;
    }

    Enum *enum_obj;
    if (wanted_type
            && (wanted_type->struct_name.size())
            && (enum_obj = ctx->getEnum(wanted_type->struct_name.c_str()))) {

        Struct *st = ctx->getStruct(wanted_type->struct_name.c_str());
        assert(st && "no struct associated with enum");

        int error_count_begin =
            ctx->er->getErrorTypeCount(ErrorType::Error);

        /* This will fail when the token is not a valid literal, so
         * in that case just continue onwards, because the token may
         * be validly parsed in other ways. */
        bool res = FormLiteralEnumParse(units, block, node, enum_obj,
                                        wanted_type, st, get_address,
                                        pr);
        if (res) {
            return res;
        } else {
            ctx->er->popErrors(error_count_begin);
        }
    }

    if (t->type == TokenType::String) {
        pr->value = NULL;
        parseBoolLiteral(ctx, block, node, pr);
        if (pr->value) {
            return true;
        }

        parseCharLiteral(ctx, block, node, pr);
        if (pr->value) {
            return true;
        }

        parseVariableLiteral(ctx, block, node, get_address, pr);
        if (pr->value) {
            return true;
        } else {
            return false;
        }
    } else if (t->type == TokenType::StringLiteral) {
        pr->value = NULL;
        return parseStringLiteral(units, ctx, block, node, pr);
    } else {
        Error *e = new Error(UnableToParseForm, node);
        ctx->er->addError(e);
        return false;
    }
}
}
