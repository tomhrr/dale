#include "Token.h"

#include <cstdio>
#include <string>

#include "../../../llvmUtils/llvmUtils.h"
#include "../../Literal/Literal.h"

using namespace dale::ErrorInst;

namespace dale {
void parseVariableLiteral(Context *ctx, llvm::BasicBlock *block,
                          Node *node, bool get_address,
                          Type *wanted_type, ParseResult *pr) {
    Token *t = node->token;
    Variable *var = ctx->getVariable(t->str_value.c_str());

    if (!var) {
        Error *e =
            new Error(VariableNotInScope, node, t->str_value.c_str());
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
        if (wanted_type && var->type->isEqualTo(wanted_type)) {
            llvm::Value *array = builder.CreateLoad(var->value);
            pr->set(block, var->type, array);
            pr->value_is_lvalue = false;
        } else {
            llvm::Value *ptr_to_array = builder.CreateGEP(
                var->value, ctx->nt->getTwoLLVMZeros());

            pr->set(block,
                    ctx->tr->getPointerType(var->type->array_type),
                    ptr_to_array);
            pr->address_of_value = var->value;
            pr->value_is_lvalue = true;
            pr->type_of_address_of_value =
                ctx->tr->getPointerType(var->type);
        }
        return;
    }

    if (!var->value) {
        Error *e = new Error(CannotDereferencePointerInContext, node);
        ctx->er->addError(e);
        return;
    }

    /* All other variables. */
    pr->set(block, var->type,
            llvm::cast<llvm::Value>(builder.CreateLoad(var->value)));
    pr->address_of_value = var->value;
    pr->value_is_lvalue = 1;
}

bool FormProcTokenParse(Units *units, Function *fn,
                        llvm::BasicBlock *block, Node *node,
                        bool get_address, bool prefixed_with_core,
                        Type *wanted_type, ParseResult *pr) {
    Context *ctx = units->top()->ctx;

    Token *t = node->token;

    if (t->type == TokenType::Int) {
        FormIntegerLiteralParse(ctx, wanted_type, block, t, pr);
        return true;
    } else if (t->type == TokenType::FloatingPoint) {
        FormFloatingPointLiteralParse(ctx, wanted_type, block, t, pr);
        return true;
    }

    if (t->type == TokenType::String) {
        pr->set(pr->block, pr->type, NULL);
        FormBoolLiteralParse(ctx, block, node, pr);
        if (pr->getValue(ctx)) {
            return true;
        }

        FormCharLiteralParse(ctx, block, node, pr);
        if (pr->getValue(ctx)) {
            return true;
        }

        parseVariableLiteral(ctx, block, node, get_address, wanted_type,
                             pr);
        if (pr->getValue(ctx)) {
            return true;
        } else {
            return false;
        }
    } else if (t->type == TokenType::StringLiteral) {
        pr->set(pr->block, pr->type, NULL);
        FormStringLiteralParse(units, ctx, block, node, pr);
        return true;
    } else {
        Error *e = new Error(UnableToParseForm, node);
        ctx->er->addError(e);
        return false;
    }
}
}
