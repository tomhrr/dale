#include "Type.h"

#include "../../Generator/Generator.h"
#include "../Struct/Struct.h"

static int anonstructcount = 0;

namespace dale { namespace Form { namespace Type {
Element::Type *
parse(Generator *gen, Node *top, bool allow_anon_structs,
      bool allow_bitfields, bool allow_refs, bool allow_retvals)
{
    if (!top) {
        return NULL;
    }

    Context *ctx = gen->ctx;

    if (top->is_token) {
        Token *t = top->token;

        if (t->type != TokenType::String) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectSingleParameterType,
                top,
                "symbol", t->tokenType()
            );
            ctx->er->addError(e);
            return NULL;
        }

        const char *typs = t->str_value.c_str();

        int bmt =
              (!strcmp(typs, "int" ))        ? dale::Type::Int
            : (!strcmp(typs, "void"))        ? dale::Type::Void
            : (!strcmp(typs, "char"))        ? dale::Type::Char
            : (!strcmp(typs, "bool"))        ? dale::Type::Bool
            : (!strcmp(typs, "uint" ))       ? dale::Type::UInt
            : (!strcmp(typs, "int8"))        ? dale::Type::Int8
            : (!strcmp(typs, "uint8"))       ? dale::Type::UInt8
            : (!strcmp(typs, "int16"))       ? dale::Type::Int16
            : (!strcmp(typs, "uint16"))      ? dale::Type::UInt16
            : (!strcmp(typs, "int32"))       ? dale::Type::Int32
            : (!strcmp(typs, "uint32"))      ? dale::Type::UInt32
            : (!strcmp(typs, "int64"))       ? dale::Type::Int64
            : (!strcmp(typs, "uint64"))      ? dale::Type::UInt64
            : (!strcmp(typs, "int128"))      ? dale::Type::Int128
            : (!strcmp(typs, "uint128"))     ? dale::Type::UInt128
            : (!strcmp(typs, "intptr"))      ? dale::Type::IntPtr
            : (!strcmp(typs, "size"))        ? dale::Type::Size
            : (!strcmp(typs, "ptrdiff"))     ? dale::Type::PtrDiff
            : (!strcmp(typs, "float"))       ? dale::Type::Float
            : (!strcmp(typs, "double"))      ? dale::Type::Double
            : (!strcmp(typs, "long-double")) ? dale::Type::LongDouble
                                             : -1;

        if (bmt != -1) {
            Element::Type *mt = gen->ctx->tr->getBasicType(bmt);

            if (mt) {
                if (!gen->is_x86_64
                        && (mt->base_type == dale::Type::Int128
                            || mt->base_type == dale::Type::UInt128)) {
                    Error *e = new Error(
                        ErrorInst::Generator::TypeNotSupported,
                        top,
                        typs
                    );
                    ctx->er->addError(e);
                    return NULL;
                }
                return mt;
            }
        }

        /* Not a simple type - check if it is a struct. */

        Element::Struct *temp_struct;

        if ((temp_struct = ctx->getStruct(typs))) {
            std::string fqsn;
            bool b = ctx->setFullyQualifiedStructName(typs, &fqsn);
            if (!b) {
                fprintf(stderr, "Internal error: unable to set struct "
                                "name (%s).\n", typs);
                abort();
            }
            return ctx->tr->getStructType(fqsn);
        }

        Error *err = new Error(
            ErrorInst::Generator::TypeNotInScope,
            top,
            typs
        );
        ctx->er->addError(err);
        return NULL;
    }

    /* If here, node is a list node. Try for a macro call. */

    Node *newtop = gen->parseOptionalMacroCall(top);

    if (newtop != top) {
        return parse(gen, newtop, allow_anon_structs,
                         allow_bitfields);
    }

    symlist *lst = top->list;

    Node *n = (*lst)[0];

    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        ctx->er->addError(e);
        return NULL;
    }

    Token *t = n->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            n
        );
        ctx->er->addError(e);
        return NULL;
    }

    // If the first element is 'do', then skip that element.

    std::vector<Node*> templist;
    if (!(t->str_value.compare("do"))) {
        templist.assign(lst->begin() + 1, lst->end());
        lst = &templist;
        if (lst->size() == 1) {
            return parse(gen, lst->at(0), allow_anon_structs,
                             allow_bitfields);
        }
    }

    /* If list is a two-element list, where the first element is
     * 'ref', then this is a reference type. */
    if (lst->size() == 2
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("ref"))) {
        if (!allow_refs) {
            Error *e = new Error(
                ErrorInst::Generator::RefsNotPermittedHere,
                top
            );
            ctx->er->addError(e);
            return NULL;
        }
        Node *new_type = gen->parseOptionalMacroCall((*lst)[1]);
        if (!new_type) {
            return NULL;
        }

        /* Reference types are only permitted at the 'top level' of
         * the type. */
        Element::Type *reference_type =
            parse(gen, (*lst)[1], allow_anon_structs,
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
            Error *e = new Error(
                ErrorInst::Generator::RetvalsNotPermittedHere,
                top
            );
            ctx->er->addError(e);
            return NULL;
        }
        Node *new_type = gen->parseOptionalMacroCall((*lst)[1]);
        if (!new_type) {
            return NULL;
        }

        /* Retval types are only permitted at the 'top level' of
         * the type. */
        Element::Type *retval_type =
            parse(gen, (*lst)[1], allow_anon_structs,
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
        Token *li = new Token(TokenType::String,0,0,0,0);
        li->str_value.append("extern");
        lst->insert((lst->begin() + 1), new Node(li));
        char buf[255];
        sprintf(buf, "__as%d", anonstructcount++);
        int error_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);

        Form::Struct::parse(gen, new Node(lst), buf);

        int error_post_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);
        if (error_count != error_post_count) {
            return NULL;
        }

        Token *name = new Token(TokenType::String,0,0,0,0);
        name->str_value.append(buf);
        Element::Type *myst = parse(gen, new Node(name), false,
                                        false);
        if (!myst) {
            fprintf(stderr, "Unable to retrieve anonymous struct.\n");
            abort();
        }
        return myst;
    }

    /* If list is a three-element list, where the first element is
     * 'bf', then this is a bitfield type. Only return such a type
     * if allow_bitfields is enabled. */

    if (allow_bitfields
            && lst->size() == 3
            && lst->at(0)->is_token
            && !(lst->at(0)->token->str_value.compare("bf"))) {
        Element::Type *bf_type =
            parse(gen, lst->at(1), false, false);
        if (!(bf_type->isIntegerType())) {
            Error *e = new Error(
                ErrorInst::Generator::BitfieldMustHaveIntegerType,
                top
            );
            ctx->er->addError(e);
            return NULL;
        }
        int size = gen->parseInteger(lst->at(2));
        if (size == -1) {
            return NULL;
        }
        return ctx->tr->getBitfieldType(bf_type, size);
    }

    if (!strcmp(t->str_value.c_str(), "const")) {
        if (lst->size() != 2) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                top,
                "const", "1"
            );
            char buf[100];
            sprintf(buf, "%d", (int) lst->size() - 1);
            e->addArgString(buf);
            ctx->er->addError(e);
            return NULL;
        }

        Node *newnum = gen->parseOptionalMacroCall((*lst)[1]);
        if (!newnum) {
            return NULL;
        }

        Element::Type *const_type =
            parse(gen, (*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (const_type == NULL) {
            return NULL;
        }

        return ctx->tr->getConstType(const_type);
    }

    if (!strcmp(t->str_value.c_str(), "array-of")) {
        if (lst->size() != 3) {
            Error *e = new Error(
                ErrorInst::Generator::IncorrectNumberOfArgs,
                top,
                "array-of", "2"
            );
            char buf[100];
            sprintf(buf, "%d", (int) lst->size() - 1);
            e->addArgString(buf);
            ctx->er->addError(e);
            return NULL;
        }

        Node *newnum = gen->parseOptionalMacroCall((*lst)[1]);
        if (!newnum) {
            return NULL;
        }

        int size = gen->parseInteger(newnum);
        if (size == -1) {
            return NULL;
        }

        Element::Type *array_type =
            parse(gen, (*lst)[2], allow_anon_structs,
                      allow_bitfields);

        if (array_type == NULL) {
            return NULL;
        }

        Element::Type *type = ctx->tr->getArrayType(array_type, size);

        return type;
    }

    if (!strcmp(t->str_value.c_str(), "p")) {
        if (!ctx->er->assertArgNums("p", top, 1, 1)) {
            return NULL;
        }

        Element::Type *points_to_type =
            parse(gen, (*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (points_to_type == NULL) {
            return NULL;
        }

        return ctx->tr->getPointerType(points_to_type);
    }

    if (!strcmp(t->str_value.c_str(), "fn")) {
        if (!ctx->er->assertArgNums("fn", top, 2, 2)) {
            return NULL;
        }

        Element::Type *ret_type =
            parse(gen, (*lst)[1], allow_anon_structs,
                      allow_bitfields);

        if (ret_type == NULL) {
            return NULL;
        }
        if (ret_type->is_array) {
            Error *e = new Error(
                ErrorInst::Generator::ReturnTypesCannotBeArrays,
                n
            );
            ctx->er->addError(e);
            return NULL;
        }

        Node *params = (*lst)[2];

        if (!params->is_list) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement,
                n,
                "list", "fn parameters", "symbol"
            );
            ctx->er->addError(e);
            return NULL;
        }

        symlist *plst = params->list;

        Element::Variable *var;

        std::vector<Element::Type *> *parameter_types =
            new std::vector<Element::Type *>;

        std::vector<Node *>::iterator node_iter;
        node_iter = plst->begin();

        while (node_iter != plst->end()) {
            var = new Element::Variable();
            var->type = NULL;

            gen->parseArgument(var, (*node_iter),
                          allow_anon_structs,
                          allow_bitfields,
                          true);

            if (var->type == NULL) {
                delete var;
                return NULL;
            }

            if (var->type->base_type == dale::Type::Void) {
                delete var;
                if (plst->size() != 1) {
                    Error *e = new Error(
                        ErrorInst::Generator::VoidMustBeTheOnlyParameter,
                        params
                    );
                    ctx->er->addError(e);
                    return NULL;
                }
                break;
            }

            /* Have to check that none come after this. */
            if (var->type->base_type == dale::Type::VarArgs) {
                if ((plst->end() - node_iter) != 1) {
                    delete var;
                    Error *e = new Error(
                        ErrorInst::Generator::VarArgsMustBeLastParameter,
                        params
                    );
                    ctx->er->addError(e);
                    return NULL;
                }
                parameter_types->push_back(var->type);
                break;
            }

            if (var->type->is_function) {
                delete var;
                Error *e = new Error(
                    ErrorInst::Generator::NonPointerFunctionParameter,
                    (*node_iter)
                );
                ctx->er->addError(e);
                return NULL;
            }

            parameter_types->push_back(var->type);

            ++node_iter;
        }

        Element::Type *type = new Element::Type();
        type->is_function     = 1;
        type->return_type     = ret_type;
        type->parameter_types = parameter_types;
        return type;
    }

    Error *e = new Error(
        ErrorInst::Generator::InvalidType,
        top
    );
    ctx->er->addError(e);

    return NULL;
}
}}}
