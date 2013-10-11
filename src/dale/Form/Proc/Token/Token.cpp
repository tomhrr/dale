#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../../Literal/Enum/Enum.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Token
{
llvm::Constant *
parseStringLiteral(Generator *gen,
                   Element::Type *type,
                   Node *top,
                   int *size) 
{
    Context *ctx = gen->ctx;

    if (type->base_type == Type::Int) {
        if (!top->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "atom", "literal", "list"
            );
            ctx->er->addError(e);
            return NULL;
        }
        dale::Token *t = top->token;

        if (t->type != TokenType::Int) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "integer", "literal", t->tokenType()
            );
            ctx->er->addError(e);
            return NULL;
        }

        llvm::Constant *myconstint =
            ctx->nt->getConstantInt(ctx->nt->getNativeIntType(),
                                    t->str_value.c_str());

        llvm::Value *myconstvalue =
            llvm::cast<llvm::Value>(myconstint);

        llvm::Constant *myconstint2 =
            llvm::cast<llvm::Constant>(myconstvalue);

        return myconstint2;
    }

    int underlying_type =
          (!type->base_type && type->points_to) ? type->points_to->base_type
        : (type->is_array)                      ? type->array_type->base_type
        : 0;

    if (underlying_type == Type::Char) {
        if (!top->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "atom", "literal", "list"
            );
            ctx->er->addError(e);
            return NULL;
        }
        dale::Token *t = top->token;

        if (t->type != TokenType::StringLiteral) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                top,
                "string", "literal", t->tokenType()
            );
            ctx->er->addError(e);
            return NULL;
        }

        size_t pos = 0;
        while ((pos = t->str_value.find("\\n", pos)) != std::string::npos) {
            t->str_value.replace(pos, 2, "\n");
        }

        *size = strlen(t->str_value.c_str()) + 1;

        return
            llvm::ConstantArray::get(llvm::getGlobalContext(),
                                     t->str_value.c_str(),
                                     true);
    }

    std::string temp;
    type->toStringProper(&temp);
    Error *e = new Error(
        ErrorInst::Generator::CannotParseLiteral,
        top,
        temp.c_str()
    );
    ctx->er->addError(e);
    return NULL;
}

bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           Element::Type *wanted_type,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;
    NativeTypes *nt = ctx->nt;
    Element::Type *type_char  = ctx->tr->getBasicType(Type::Char);
    Element::Type *type_pchar = ctx->tr->getPointerType(type_char);
    std::vector<llvm::Value *> two_zero_indices;
    stl::push_back2(&two_zero_indices,
                    nt->getLLVMZero(), nt->getLLVMZero());

    dale::Token *t = node->token;

    /* Check if we are expecting an enum. */

    Element::Enum *myenum2;
    if (wanted_type
            && (wanted_type->struct_name)
            && (myenum2 =
                    ctx->getEnum(wanted_type->struct_name->c_str()))) {

        Element::Struct *myenumstruct2 =
            ctx->getStruct(wanted_type->struct_name->c_str());

        if (!myenumstruct2) {
            fprintf(stderr,
                    "Internal error: no struct associated "
                    "with enum.\n");
            abort();
        }

        int original_error_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);

        /* Will fail here where the token is not a valid
            * literal, so in that case just continue onwards
            * (token could be a var name). */

        bool res =
            Form::Literal::Enum::parse(
                                gen, block, node,
                                myenum2,
                                wanted_type,
                                myenumstruct2,
                                get_address,
                                pr);

        if (res) {
            return res;
        } else {
            ctx->er->popErrors(original_error_count);
            goto tryvar;
        }
    } else if (t->type == TokenType::Int) {
        if (wanted_type
                && wanted_type->isIntegerType()) {
            int mysize =
                nt->internalSizeToRealSize(wanted_type->getIntegerSize());
            pr->set(
                        block,
                        ctx->tr->getBasicType(wanted_type->base_type),
                        nt->getConstantInt(
                            llvm::IntegerType::get(
                                llvm::getGlobalContext(),
                                mysize
                            ),
                            t->str_value.c_str()
                        )
                    );
            return true;
        } else {
            pr->set(
                        block,
                        ctx->tr->getBasicType(Type::Int),
                        nt->getConstantInt(
                            nt->getNativeIntType(),
                            t->str_value.c_str()
                        )
                    );
            return true;
        }
    } else if (t->type == TokenType::FloatingPoint) {
        if (wanted_type
                && wanted_type->base_type == Type::Float) {
            pr->set(
                        block,
                        ctx->tr->getBasicType(Type::Float),
                        llvm::ConstantFP::get(
                            llvm::Type::getFloatTy(llvm::getGlobalContext()),
                            llvm::StringRef(t->str_value.c_str())
                        )
                    );
            return true;
        } else if (wanted_type
                    && wanted_type->base_type == Type::Double) {
            pr->set(
                        block,
                        ctx->tr->getBasicType(Type::Double),
                        llvm::ConstantFP::get(
                            llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                            llvm::StringRef(t->str_value.c_str())
                        )
                    );
            return true;
        } else if (wanted_type
                    && wanted_type->base_type == Type::LongDouble) {
            pr->set(
                        block,
                        ctx->tr->getBasicType(Type::LongDouble),
                        llvm::ConstantFP::get(
                            nt->getNativeLongDoubleType(),
                            llvm::StringRef(t->str_value.c_str())
                        )
                    );
            return true;
        } else {
            pr->set(
                        block,
                        ctx->tr->getBasicType(Type::Float),
                        llvm::ConstantFP::get(
                            llvm::Type::getFloatTy(llvm::getGlobalContext()),
                            llvm::StringRef(t->str_value.c_str())
                        )
                    );
            return true;
        }
    } else if (t->type == TokenType::String) {
tryvar:
        /* Special cases - boolean values. */
        int is_true  = !t->str_value.compare("true");
        int is_false = !t->str_value.compare("false");

        if (is_true || is_false) {
            pr->set(
                        block,
                        ctx->tr->getBasicType(Type::Bool),
                        llvm::ConstantInt::get(
                            llvm::Type::getInt1Ty(llvm::getGlobalContext()),
                            is_true
                        )
                    );
            return true;
        }

        /* Special case - characters. */
        if ((t->str_value.size() >= 3)
                && (t->str_value.at(0) == '#')
                && (t->str_value.at(1) == '\\')) {
            const char *temp = t->str_value.c_str();
            temp += 2;
            char c;

            if (!strcmp(temp, "NULL")) {
                c = '\0';
            } else if (!strcmp(temp, "TAB")) {
                c = '\t';
            } else if (!strcmp(temp, "SPACE")) {
                c = ' ';
            } else if (!strcmp(temp, "NEWLINE")) {
                c = '\n';
            } else if (!strcmp(temp, "CARRIAGE")) {
                c = '\r';
            } else if (!strcmp(temp, "EOF")) {
                c = EOF;
            } else {
                if (strlen(temp) != 1) {
                    Error *e = new Error(
                        ErrorInst::Generator::InvalidChar,
                        node,
                        temp
                    );
                    ctx->er->addError(e);
                    return false;
                }
                c = t->str_value.at(2);
            }

            pr->set(
                        block,
                        ctx->tr->getBasicType(Type::Char),
                        llvm::ConstantInt::get(nt->getNativeCharType(), c)
                    );
            return true;
        }

        /* Plain string - has to be variable. */
        Element::Variable *var =
            ctx->getVariable(t->str_value.c_str());

        if (!var) {
            Error *e = new Error(
                ErrorInst::Generator::VariableNotInScope,
                node,
                t->str_value.c_str()
            );
            ctx->er->addError(e);
            return false;
        }

        llvm::IRBuilder<> builder(block);

        if (get_address) {
            pr->set(
                        block,
                        ctx->tr->getPointerType(var->type),
                        var->value
                    );
            return true;
        } else {
            if (var->type->is_array) {
                /* If the variable is an array, return a pointer of
                * the array's type. */
                llvm::Value *p_to_array =
                    builder.CreateGEP(
                        var->value,
                        llvm::ArrayRef<llvm::Value*>(two_zero_indices)
                    );

                pr->set(
                            block,
                            ctx->tr->getPointerType(var->type->array_type),
                            p_to_array
                        );
                return true;
            }

            /* Return the dereferenced variable. */
            pr->set(
                        block,
                        var->type,
                        llvm::cast<llvm::Value>(
                            builder.CreateLoad(var->value)
                        )
                    );
            return true;
        }
    } else if (t->type == TokenType::StringLiteral) {

        /* Add the variable to the module. */

        int size = 0;
        llvm::Constant *init = parseStringLiteral(gen, type_pchar, 
                                                  node, &size);
        if (!init) {
            return false;
        }
        Element::Type *temp = ctx->tr->getArrayType(type_char, size);

        llvm::Type *llvm_type =
            ctx->toLLVMType(temp, NULL, false);
        if (!llvm_type) {
            return false;
        }

        /* Have to check for existing variables with this
         * name, due to modules. */

        std::string varname;
        llvm::GlobalVariable *var;
        gen->getUnusedVarname(&varname);

        var =
            llvm::cast<llvm::GlobalVariable>(
                gen->mod->getOrInsertGlobal(varname.c_str(),
                                            llvm_type)
            );

        var->setLinkage(ctx->toLLVMLinkage(Linkage::Intern));
        var->setInitializer(init);
        var->setConstant(true);

        Element::Variable *var2 = new Element::Variable();
        var2->name.append(varname.c_str());
        var2->internal_name.append(varname);
        var2->type = temp;
        var2->value = llvm::cast<llvm::Value>(var);
        var2->linkage = Linkage::Intern;
        int avres = ctx->ns()->addVariable(varname.c_str(), var2);

        if (!avres) {
            Error *e = new Error(
                ErrorInst::Generator::RedefinitionOfVariable,
                node,
                varname.c_str()
            );
            ctx->er->addError(e);
            return false;
        }

        llvm::IRBuilder<> builder(block);

        llvm::Value *charpointer =
            builder.CreateGEP(
                llvm::cast<llvm::Value>(var2->value),
                llvm::ArrayRef<llvm::Value*>(two_zero_indices));

        pr->set(
                    block,
                    type_pchar,
                    charpointer
                );
        return true;
    } else {
        Error *e = new Error(
            ErrorInst::Generator::UnableToParseForm,
            node
        );
        ctx->er->addError(e);
        return false;
    }
}
}
}
}
}
