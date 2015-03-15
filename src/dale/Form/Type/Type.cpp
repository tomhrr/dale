#include "Type.h"

#include "../../Units/Units.h"
#include "../Struct/Struct.h"
#include "../Argument/Argument.h"
#include "../Literal/Integer/Integer.h"

using namespace dale::ErrorInst::Generator;

static int anonstructcount = 0;

namespace dale {
Type *
parseTypeToken(Units *units, Node *node)
{
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
            if (!units->top()->is_x86_64
                    && (type->base_type == BaseType::Int128
                     || type->base_type == BaseType::UInt128)) {
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

Type *
FormTypeParse(Units *units, Node *node, bool allow_anon_structs,
              bool allow_bitfields, bool allow_refs, bool allow_retvals)
{
    if (!node) {
        return NULL;
    }

    Context *ctx = units->top()->ctx;

    if (node->is_token) {
        return parseTypeToken(units, node);
    }

    /* If here, node is a list node. Try for a macro call. */

    Node *newnode = units->top()->mp->parsePotentialMacroCall(node);

    if (newnode != node) {
        return FormTypeParse(units, newnode, allow_anon_structs,
                         allow_bitfields, allow_refs, allow_retvals);
    }

    std::vector<Node *> *lst = node->list;

    Node *n = (*lst)[0];

    if (!n->is_token) {
        Error *e = new Error(FirstListElementMustBeAtom, n);
        ctx->er->addError(e);
        return NULL;
    }

    Token *t = n->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(FirstListElementMustBeSymbol, n);
        ctx->er->addError(e);
        return NULL;
    }

    // If the first element is 'do', then skip that element.

    std::vector<Node*> templist;
    if (!(t->str_value.compare("do"))) {
        templist.assign(lst->begin() + 1, lst->end());
        lst = &templist;
        if (lst->size() == 1) {
            return FormTypeParse(units, lst->at(0), allow_anon_structs,
                             allow_bitfields);
        }
    }

    /* If list is a two-element list, where the first element is
     * 'ref', then this is a reference type. */
    if (lst->size() == 2
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("ref"))) {
        if (!allow_refs) {
            Error *e = new Error(RefsNotPermittedHere, node);
            ctx->er->addError(e);
            return NULL;
        }
        Node *new_type = units->top()->mp->parsePotentialMacroCall((*lst)[1]);
        if (!new_type) {
            return NULL;
        }

        /* Reference types are only permitted at the 'node level' of
         * the type. */
        Type *reference_type =
            FormTypeParse(units, (*lst)[1], allow_anon_structs,
                  allow_bitfields);

        if (reference_type == NULL) {
            return NULL;
        }

        return ctx->tr->getReferenceType(reference_type);
    }

    /* If list is a two-element list, where the first element is
     * 'retval', then this is a retval type. */
    if (lst->size() == 2
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("retval"))) {
        if (!allow_retvals) {
            Error *e = new Error(RetvalsNotPermittedHere, node);
            ctx->er->addError(e);
            return NULL;
        }
        Node *new_type = units->top()->mp->parsePotentialMacroCall((*lst)[1]);
        if (!new_type) {
            return NULL;
        }

        /* Retval types are only permitted at the 'node level' of
         * the type. */
        Type *retval_type =
            FormTypeParse(units, (*lst)[1], allow_anon_structs,
                  allow_bitfields);

        if (retval_type == NULL) {
            return NULL;
        }

        return ctx->tr->getRetvalType(retval_type);
    }

    /* If list is a two-element list, where the first element is
     * 'struct', then this is an anonymous struct. If
     * allow_anon_structs is enabled, then construct a list that
     * can in turn be used to create that struct, call
     * parseStructDefinition, and then use that new struct name
     * for the value of this element. */

    if (allow_anon_structs
            && lst->size() == 2
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("struct"))) {
        Token *li = new Token(TokenType::String);
        li->str_value.append("extern");
        lst->insert((lst->begin() + 1), new Node(li));
        char buf[255];
        sprintf(buf, "__as%d", anonstructcount++);
        int error_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);

        FormStructParse(units, new Node(lst), buf);

        int error_post_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);
        if (error_count != error_post_count) {
            return NULL;
        }

        Token *name = new Token(TokenType::String);
        name->str_value.append(buf);
        Type *myst = FormTypeParse(units, new Node(name), false,
                                   false);
        assert(myst && "unable to retrieve anonymous struct");
        return myst;
    }

    /* If list is a three-element list, where the first element is
     * 'bf', then this is a bitfield type. Only return such a type
     * if allow_bitfields is enabled. */

    if (allow_bitfields
            && lst->size() == 3
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("bf"))) {
        Type *bf_type =
            FormTypeParse(units, lst->at(1), false, false);
        if (!(bf_type->isIntegerType())) {
            Error *e = new Error(BitfieldMustHaveIntegerType, node);
            ctx->er->addError(e);
            return NULL;
        }
        int size = FormLiteralIntegerParse(lst->at(2), ctx->er);
        if (size == -1) {
            return NULL;
        }
        return ctx->tr->getBitfieldType(bf_type, size);
    }

    if (!strcmp(t->str_value.c_str(), "const")) {
        if (lst->size() != 2) {
            Error *e = new Error(IncorrectNumberOfArgs, node,
                                 "const", "1", ((int) lst->size() - 1));
            ctx->er->addError(e);
            return NULL;
        }

        Node *newnum = units->top()->mp->parsePotentialMacroCall((*lst)[1]);
        if (!newnum) {
            return NULL;
        }

        Type *const_type =
            FormTypeParse(units, (*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (const_type == NULL) {
            return NULL;
        }

        return ctx->tr->getConstType(const_type);
    }

    if (!strcmp(t->str_value.c_str(), "array-of")) {
        if (lst->size() != 3) {
            Error *e = new Error(IncorrectNumberOfArgs, node,
                                 "array-of", "2", ((int) lst->size() - 1));
            ctx->er->addError(e);
            return NULL;
        }

        Node *newnum = units->top()->mp->parsePotentialMacroCall((*lst)[1]);
        if (!newnum) {
            return NULL;
        }

        int size = FormLiteralIntegerParse(newnum, ctx->er);
        if (size == -1) {
            return NULL;
        }

        Type *array_type =
            FormTypeParse(units, (*lst)[2], allow_anon_structs,
                      allow_bitfields);

        if (array_type == NULL) {
            return NULL;
        }

        Type *type = ctx->tr->getArrayType(array_type, size);

        return type;
    }

    if (!strcmp(t->str_value.c_str(), "p")) {
        if (!ctx->er->assertArgNums("p", node, 1, 1)) {
            return NULL;
        }

        Type *points_to_type =
            FormTypeParse(units, (*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (points_to_type == NULL) {
            return NULL;
        }

        return ctx->tr->getPointerType(points_to_type);
    }

    if (!strcmp(t->str_value.c_str(), "fn")) {
        if (!ctx->er->assertArgNums("fn", node, 2, 2)) {
            return NULL;
        }

        Type *ret_type =
            FormTypeParse(units, (*lst)[1], allow_anon_structs,
                      allow_bitfields, false, true);

        if (ret_type == NULL) {
            return NULL;
        }
        if (ret_type->is_array) {
            Error *e = new Error(ReturnTypesCannotBeArrays, n);
            ctx->er->addError(e);
            return NULL;
        }

        Node *params = (*lst)[2];

        if (!params->is_list) {
            Error *e = new Error(UnexpectedElement, n,
                                 "list", "fn parameters", "symbol");
            ctx->er->addError(e);
            return NULL;
        }

        std::vector<Node *> *plst = params->list;

        Variable *var;

        std::vector<Type *> parameter_types;

        std::vector<Node *>::iterator node_iter;
        node_iter = plst->begin();

        while (node_iter != plst->end()) {
            var = new Variable();
            var->type = NULL;

            FormArgumentParse(units, var, (*node_iter),
                          allow_anon_structs,
                          allow_bitfields,
                          true);

            if (var->type == NULL) {
                delete var;
                return NULL;
            }

            if (var->type->base_type == BaseType::Void) {
                delete var;
                if (plst->size() != 1) {
                    Error *e = new Error(VoidMustBeTheOnlyParameter,
                                         params);
                    ctx->er->addError(e);
                    return NULL;
                }
                break;
            }

            /* Have to check that none come after this. */
            if (var->type->base_type == BaseType::VarArgs) {
                if ((plst->end() - node_iter) != 1) {
                    delete var;
                    Error *e = new Error(VarArgsMustBeLastParameter,
                                         params);
                    ctx->er->addError(e);
                    return NULL;
                }
                parameter_types.push_back(var->type);
                break;
            }
;
            if (var->type->is_function) {
                delete var;
                Error *e = new Error(NonPointerFunctionParameter,
                                     (*node_iter));
                ctx->er->addError(e);
                return NULL;
            }

            parameter_types.push_back(var->type);

            ++node_iter;
        }

        Type *type = new Type();
        type->is_function     = 1;
        type->return_type     = ret_type;
        type->parameter_types = parameter_types;
        return type;
    }

    Error *e = new Error(InvalidType, node);
    ctx->er->addError(e);

    return NULL;
}
}
