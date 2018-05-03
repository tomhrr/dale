#include "Type.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../Form/TopLevel/GlobalVariable/GlobalVariable.h"
#include "../../Units/Units.h"
#include "../Parameter/Parameter.h"
#include "../Struct/Struct.h"
#include "../Value/Value.h"

using namespace dale::ErrorInst;

static int anon_struct_count = 0;

namespace dale {
int parseLiteralIntegerToInt(Node *node, ErrorReporter *er) {
    if (!node->is_token) {
        Error *e = new Error(UnexpectedElement, node, "symbol",
                             "integer", "list");
        er->addError(e);
        return -1;
    }
    if (node->token->type != TokenType::Int) {
        Error *e = new Error(UnexpectedElement, node, "integer",
                             "literal", node->token->tokenType());
        er->addError(e);
        return -1;
    }

    const char *num_str = node->token->str_value.c_str();
    char *end_ptr;
    unsigned long num =
        strtoul(num_str, &end_ptr, DECIMAL_RADIX);  // NOLINT

    if (STRTOUL_FAILED(num, num_str, end_ptr)) {
        Error *e = new Error(UnableToParseInteger, node,
                             node->token->str_value.c_str());
        er->addError(e);
        return -1;
    }

    return num;
}

Type *parseTypeToken(Units *units, Node *node) {
    Context *ctx = units->top()->ctx;

    Token *token = node->token;
    if (token->type != TokenType::String) {
        Error *e = new Error(IncorrectSingleParameterType, node,
                             "symbol", token->tokenType());
        ctx->er->addError(e);
        return NULL;
    }

    const char *type_str = token->str_value.c_str();

    int base_type = stringToBaseType(type_str);
    if (base_type != -1) {
        Type *type = ctx->tr->getBasicType(base_type);
        if (type) {
            if (!units->top()->is_x86_64 &&
                (type->base_type == BaseType::Int128 ||
                 type->base_type == BaseType::UInt128)) {
                Error *e = new Error(TypeNotSupported, node, type_str);
                ctx->er->addError(e);
                return NULL;
            }
            return type;
        }
    }

    Struct *st = NULL;
    if ((st = ctx->getStruct(type_str))) {
        std::string st_name;
        bool res = ctx->setFullyQualifiedStructName(type_str, &st_name);
        assert(res && "unable to set struct name");
        _unused(res);
        return ctx->tr->getStructType(st_name.c_str());
    }

    Error *err = new Error(TypeNotInScope, node, type_str);
    ctx->er->addError(err);
    return NULL;
}

Type *FormTypeParse(Units *units, Node *node, bool allow_anon_structs,
                    bool allow_bitfields, bool allow_refs,
                    bool allow_retvals) {
    Context *ctx = units->top()->ctx;

    node = units->top()->mp->parsePotentialMacroCall(node);
    if (!node) {
        return NULL;
    } else if (node->is_token) {
        return parseTypeToken(units, node);
    }

    std::vector<Node *> *lst = node->list;
    Node *name_node = (*lst)[0];

    if (!name_node->is_token) {
        Error *e = new Error(FirstListElementMustBeAtom, name_node);
        ctx->er->addError(e);
        return NULL;
    }

    Token *token = name_node->token;
    if (token->type != TokenType::String) {
        Error *e = new Error(FirstListElementMustBeSymbol, name_node);
        ctx->er->addError(e);
        return NULL;
    }

    std::vector<Node *> lst_tail;
    if (!strcmp(token->str_value.c_str(), "do")) {
        lst_tail.assign(lst->begin() + 1, lst->end());
        lst = &lst_tail;
        if (lst->size() == 1) {
            return FormTypeParse(units, (*lst)[0], allow_anon_structs,
                                 allow_bitfields, allow_refs,
                                 allow_retvals);
        }
    }

    if (lst->size() < 2) {
        Error *e = new Error(InvalidType, node);
        ctx->er->addError(e);
        return NULL;
    }

    Node *fst_node = (*lst)[0];
    Node *snd_node = (*lst)[1];

    /* If list is a two-element list, where the first element is
     * 'ref', then this is a reference type. */
    if (lst->size() == 2 && fst_node->is_token &&
        !(fst_node->token->str_value.compare("ref"))) {
        if (!allow_refs) {
            Error *e = new Error(RefsNotPermittedHere, node);
            ctx->er->addError(e);
            return NULL;
        }
        /* Reference types are only permitted at the 'node level' of
         * the type. */
        Type *reference_type = FormTypeParse(
            units, snd_node, allow_anon_structs, allow_bitfields);

        if (reference_type == NULL) {
            return NULL;
        }
        return ctx->tr->getReferenceType(reference_type);
    }

    /* If list is a two-element list, where the first element is
     * 'rv-ref', then this is an rvalue reference type. */
    if (lst->size() == 2 && fst_node->is_token &&
        !(fst_node->token->str_value.compare("rv-ref"))) {
        if (!allow_refs) {
            Error *e = new Error(RefsNotPermittedHere, node);
            ctx->er->addError(e);
            return NULL;
        }
        /* Reference types are only permitted at the 'node level' of
         * the type. */
        Type *rvalue_reference_type = FormTypeParse(
            units, snd_node, allow_anon_structs, allow_bitfields);

        if (rvalue_reference_type == NULL) {
            return NULL;
        }
        return ctx->tr->getRvalueReferenceType(rvalue_reference_type);
    }

    /* If list is a two-element list, where the first element is
     * 'retval', then this is a retval type. */
    if (lst->size() == 2 && fst_node->is_token &&
        !(fst_node->token->str_value.compare("retval"))) {
        if (!allow_retvals) {
            Error *e = new Error(RetvalsNotPermittedHere, node);
            ctx->er->addError(e);
            return NULL;
        }
        /* Retval types are only permitted at the 'node level' of
         * the type. */
        Type *retval_type = FormTypeParse(
            units, snd_node, allow_anon_structs, allow_bitfields);

        if (retval_type == NULL) {
            return NULL;
        }
        return ctx->tr->getRetvalType(retval_type);
    }

    /* If list is a two-element list, where the first element is
     * 'struct', then this is an anonymous struct.  If
     * allow_anon_structs is enabled, then construct a list that can
     * in turn be used to create that struct, call
     * parseStructDefinition, and then use that new struct name as the
     * value of this element. */
    if (allow_anon_structs && lst->size() == 2 && fst_node->is_token &&
        !(fst_node->token->str_value.compare("struct"))) {
        lst->insert((lst->begin() + 1), new Node("extern"));

        char buf[255];
        snprintf(buf, sizeof(buf), "__as%d", anon_struct_count++);

        int error_count_begin =
            ctx->er->getErrorTypeCount(ErrorType::Error);
        FormStructParse(units, new Node(lst), buf);
        int error_count_end =
            ctx->er->getErrorTypeCount(ErrorType::Error);
        if (error_count_begin != error_count_end) {
            return NULL;
        }

        Type *type = FormTypeParse(units, new Node(buf), false, false);
        assert(type && "unable to retrieve anonymous struct type");
        return type;
    }

    /* If list is a three-element list, where the first element is
     * 'bf', then this is a bitfield type.  Only return such a type
     * if allow_bitfields is enabled. */
    if (allow_bitfields && lst->size() == 3 && fst_node->is_token &&
        !(fst_node->token->str_value.compare("bf"))) {
        Type *type = FormTypeParse(units, snd_node, false, false);
        if (!type->isIntegerType()) {
            Error *e = new Error(BitfieldMustHaveIntegerType, node);
            ctx->er->addError(e);
            return NULL;
        }
        int size = parseLiteralIntegerToInt((*lst)[2], ctx->er);
        if (size == -1) {
            return NULL;
        }
        return ctx->tr->getBitfieldType(type, size);
    }

    if (fst_node->is_token &&
        !strcmp(fst_node->token->str_value.c_str(), "const")) {
        if (lst->size() != 2) {
            Error *e =
                new Error(IncorrectNumberOfArgs, node, "const", "1",
                          (static_cast<int>(lst->size()) - 1));
            ctx->er->addError(e);
            return NULL;
        }

        Type *const_type = FormTypeParse(
            units, snd_node, allow_anon_structs, allow_bitfields);

        if (const_type == NULL) {
            return NULL;
        }
        return ctx->tr->getConstType(const_type);
    }

    if (fst_node->is_token &&
        !strcmp(fst_node->token->str_value.c_str(), "array-of")) {
        if (lst->size() != 3) {
            Error *e =
                new Error(IncorrectNumberOfArgs, node, "array-of", "2",
                          (static_cast<int>(lst->size()) - 1));
            ctx->er->addError(e);
            return NULL;
        }

        Type *array_type = FormTypeParse(
            units, (*lst)[2], allow_anon_structs, allow_bitfields);

        if (array_type == NULL) {
            return NULL;
        }

        llvm::Constant *size_value = NULL;
        int unused_size;
        size_value = FormValueParse(units, ctx->tr->type_int, snd_node,
                                    &unused_size);
        if (!size_value) {
            return NULL;
        }
        llvm::ConstantInt *size_value_int =
            llvm::dyn_cast<llvm::ConstantInt>(size_value);
        if (!size_value_int) {
            Error *e = new Error(
                ErrorInst::UnableToParseIntegerNoString, snd_node);
            ctx->er->addError(e);
            return NULL;
        }

        int size = size_value_int->getZExtValue();

        return ctx->tr->getArrayType(array_type, size);
    }

    if (fst_node->is_token &&
        !strcmp(fst_node->token->str_value.c_str(), "p")) {
        if (!ctx->er->assertArgNums("p", node, 1, 1)) {
            return NULL;
        }

        Type *points_to_type = FormTypeParse(
            units, snd_node, allow_anon_structs, allow_bitfields);

        if (points_to_type == NULL) {
            return NULL;
        }
        return ctx->tr->getPointerType(points_to_type);
    }

    if (fst_node->is_token &&
        !strcmp(fst_node->token->str_value.c_str(), "fn")) {
        if (!ctx->er->assertArgNums("fn", node, 2, 2)) {
            return NULL;
        }

        Type *ret_type =
            FormTypeParse(units, snd_node, allow_anon_structs,
                          allow_bitfields, false, true);

        if (ret_type == NULL) {
            return NULL;
        }
        if (ret_type->is_array) {
            Error *e = new Error(ReturnTypesCannotBeArrays, node);
            ctx->er->addError(e);
            return NULL;
        }

        Node *params = (*lst)[2];

        if (!params->is_list) {
            Error *e = new Error(UnexpectedElement, node, "list",
                                 "fn parameters", "symbol");
            ctx->er->addError(e);
            return NULL;
        }

        std::vector<Type *> parameter_types;
        for (std::vector<Node *>::iterator b = params->list->begin(),
                                           e = params->list->end();
             b != e; ++b) {
            Variable *var = new Variable();
            var->type = NULL;

            FormParameterParse(units, var, (*b), allow_anon_structs,
                               allow_bitfields, true, false);
            if (var->type == NULL) {
                delete var;
                return NULL;
            }

            if (var->type->base_type == BaseType::Void) {
                delete var;
                if (params->list->size() != 1) {
                    Error *e =
                        new Error(VoidMustBeTheOnlyParameter, params);
                    ctx->er->addError(e);
                    return NULL;
                }
                break;
            }

            if (var->type->base_type == BaseType::VarArgs) {
                if ((params->list->end() - b) != 1) {
                    delete var;
                    Error *e =
                        new Error(VarArgsMustBeLastParameter, params);
                    ctx->er->addError(e);
                    return NULL;
                }
                parameter_types.push_back(var->type);
                break;
            }
            if (var->type->is_function) {
                delete var;
                Error *e = new Error(NonPointerFunctionParameter, (*b));
                ctx->er->addError(e);
                return NULL;
            }

            parameter_types.push_back(var->type);
        }

        Type *type = new Type();
        type->is_function = true;
        type->return_type = ret_type;
        type->parameter_types = parameter_types;

        return type;
    }

    Error *e = new Error(InvalidType, node);
    ctx->er->addError(e);

    return NULL;
}
}
